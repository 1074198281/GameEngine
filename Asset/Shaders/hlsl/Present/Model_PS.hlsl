#include "../Algorithm/Lighting.hlsl"

Texture2D<float4> tex0[5] : register(t0);

Texture2D<float4> BaseColorTexture : register(t0);
Texture2D<float4> MetallicRoughnessTexture : register(t1);
Texture2D<float4> OcclusionTexture : register(t2); 
Texture2D<float4> EmissiveTexture : register(t3);
Texture2D<float4> NormalTexture : register(t4);

TextureCube<float3> RadianceIBLTexture : register(t10);
TextureCube<float3> IrradianceIBLTexture : register(t11);
Texture2D<float4> BRDF_LUT : register(t12);

SamplerState DefaultSampler : register(s10);

cbuffer cbMaterialConstants : register(b0)
{
    float4 BaseColorFactor;
    float3 EmissiveFactor;
    float NormalTextureScale;
    float2 MetallicRoughnessFactor;
};

cbuffer cbCommon : register(b1)
{
    float4 gLightPosition[MAX_LIGHT_NUM];
    float4 gLightColor[MAX_LIGHT_NUM];
    float4x4 gModelMatrix;
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
    float4 gViewerPos;
    float3 SunDirection;
    float3 SunInsensity;
    int gLightNum;
    float IBLRange;
    float IBLBias;
};

struct VertexOut
{
    float4 ProjectedPosition : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TextureUV : TEXCOORD;
};

float3 CalculateIBL(SurfaceProperties surface)
{
    // IBL part
    float3 IBLResult = float3(0.0, 0.0, 0.0);
    float3 ks = FresnelSchlickRoughness(saturate(dot(surface.View_Vec, surface.Normal_Vec)), surface.Albedo, surface.Roughness);
    float3 kd = (1 - ks) * (1 - surface.Metallic);

    // diffuse part
    float3 irradiance = IrradianceIBLTexture.Sample(DefaultSampler, surface.Normal_Vec);
    float3 diffuse = irradiance * surface.Albedo.xyz * kd;
    IBLResult += diffuse;
    
    // specular part
    const float MAX_LOD = 11.0;
    float3 Reflect_Vec = reflect(-surface.View_Vec, surface.Normal_Vec);
    float3 prefilterColor = RadianceIBLTexture.SampleLevel(DefaultSampler, Reflect_Vec, surface.roughness * MAX_LOD);
    float2 env = BRDF_LUT.Sample(DefaultSampler, float2(saturate(surface.N_dot_V), surface.roughness)).xy;
    float3 specular = prefilterColor * (ks * env.x + env.y);
    IBLResult += specular;
    
    return IBLResult;
}

float4 main(VertexOut pin) : SV_Target
{
    float LumenIns = 4.0;
    float4 LightColor = float4(1.0, 1.0, 1.0, 1.0);

    float4 WorldPos = pin.ProjectedPosition.xyzw / pin.ProjectedPosition.w;
    float3 defaultLightPos = gLightPosition[0].xyz;
    float3 LightDir = normalize((defaultLightPos - WorldPos.xyz));
    
    float3 NormalDir = normalize(mul(float4(pin.WorldNormal, 0.0f), transpose(gModelMatrix)).xyz);

#ifdef LAMBERT
    return LambertLighting(LumenIns, LightColor, LightDir, NormalDir);
#endif
#ifdef PBR
    float3 colorResult = float3(0.0, 0.0, 0.0);

    float4 baseColor = BaseColorFactor * BaseColorTexture.Sample(DefaultSampler, pin.TextureUV);
    float metallic = MetallicRoughnessFactor.x * MetallicRoughnessTexture.Sample(DefaultSampler, pin.TextureUV).b;
    float roughness = MetallicRoughnessFactor.y * MetallicRoughnessTexture.Sample(DefaultSampler, pin.TextureUV).g;
    float ambientocclusion = OcclusionTexture.Sample(DefaultSampler, pin.TextureUV).r;
    float3 emissive = EmissiveFactor * EmissiveTexture.Sample(DefaultSampler, pin.TextureUV).rgb;
    float3 normal = normalize(pin.WorldNormal);

    SurfaceProperties surface;
    surface.PositionWorld = HomogeneousCoordinates(pin.WorldPosition);
    surface.Normal_Vec = normal;
    surface.View_Vec = normalize((surface.PositionWorld - gViewerPos).xyz);
    surface.N_dot_V = dot(surface.Normal_Vec, surface.View_Vec);
    surface.roughness = roughness;
    surface.roughness_sq = roughness * roughness;
    surface.roughness_mul_4 = surface.roughness_sq * surface.roughness_sq;
    surface.Albedo = baseColor.rgb;
    surface.Metallic = metallic;
    surface.Roughness = roughness;
    surface.AmbientOcclusion = ambientocclusion;
    
    // add emissive color
    colorResult += emissive;

    // add direct and reflect light
    LightProperties light;
    light.LightPosition = gLightPosition;
    light.LightColor = gLightColor;
    light.LightNum = gLightNum;
    colorResult += CalculateDirectionalLighting(surface, light);

    // add IBL part
    colorResult += CalculateIBL(surface);
    
    colorResult = LinearTosRGB(colorResult);

    return float4(colorResult, 1.0);
#endif
    
}
