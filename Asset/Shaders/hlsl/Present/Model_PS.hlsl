#include "../Algorithm/Lighting.hlsl"
#include "../Algorithm/CommonMath.hlsl"

Texture2D<float4> BaseColorTexture : register(t0);
Texture2D<float4> MetallicRoughnessTexture : register(t1);
Texture2D<float4> OcclusionTexture : register(t2); 
Texture2D<float4> EmissiveTexture : register(t3);
Texture2D<float4> NormalTexture : register(t4);

TextureCube<float3> RadianceIBLTexture : register(t20);
TextureCube<float3> IrradianceIBLTexture : register(t21);
Texture2D<float4> BRDF_LUT : register(t32);

SamplerState DefaultSampler : register(s10);

cbuffer cbMaterialConstants : register(b0)
{
    float4 BaseColorFactor;
    float3 EmissiveFactor;
    float NormalTextureScale;
    float2 MetallicRoughnessFactor;
    float2 padding0;
    float4 BaseColorTextureTransform;
    float4 MetallicRoughnessTextureTransform;
    float4 OcclusionTransform;
    float4 EmissiveTextureTransform;
    float4 NormalTextureTransform;
    float BaseColorRotation;
    float MetallicRoughnessRotation;
    float OcclusionRotation;
    float EmissiveRotation;
    float NormalRotation;
    float3 padding1;
};

cbuffer cbPerBatchConstants : register(b1)
{
    float4x4 gModelMatrix;
};

// cb2
cbuffer cbPerFrameConstants : register(b2)
{
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
    float4 gCameraPosition;
    int gLightNum;
    int clip_space_type; //0: opengl ,1: others ≤√ºÙø’º‰¿‡–Õ
    float2 padding2;
};

// cb3
struct cLight
{
    float4 gLightPosition;
    float4 gLightColor;
    float4 gLightDirection;
    float gInsensity;
};
cbuffer LightInfo : register(b3)
{
    cLight glightinfo[MAX_LIGHT_NUM];
}

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
    
    float3 NormalDir = normalize(mul(float4(pin.WorldNormal, 0.0f), transpose(gModelMatrix)).xyz);

#ifdef LAMBERT
    return LambertLighting(LumenIns, LightColor, LightDir, NormalDir);
#endif
#ifdef PBR
    float3 colorResult = float3(0.0, 0.0, 0.0);

    float4 baseColor = BaseColorFactor * BaseColorTexture.Sample(
    DefaultSampler, getTransformedUV(BaseColorTextureTransform[0], BaseColorTextureTransform[2], BaseColorRotation, pin.TextureUV));
    float metallic = MetallicRoughnessFactor.x * MetallicRoughnessTexture.Sample(
    DefaultSampler, getTransformedUV(MetallicRoughnessTextureTransform[0], MetallicRoughnessTextureTransform[2], MetallicRoughnessRotation, pin.TextureUV)).b;
    float roughness = MetallicRoughnessFactor.y * MetallicRoughnessTexture.Sample(
    DefaultSampler, getTransformedUV(MetallicRoughnessTextureTransform[0], MetallicRoughnessTextureTransform[2], MetallicRoughnessRotation, pin.TextureUV)).g;
    float ambientocclusion = OcclusionTexture.Sample(
    DefaultSampler, getTransformedUV(OcclusionTransform[0], OcclusionTransform[2], OcclusionRotation, pin.TextureUV)).r;
    float3 emissive = EmissiveFactor * EmissiveTexture.Sample(
    DefaultSampler, getTransformedUV(EmissiveTextureTransform[0], EmissiveTextureTransform[2], EmissiveRotation, pin.TextureUV)).rgb;
    float3 normal = normalize(pin.WorldNormal);
    
    SurfaceProperties surface;
    surface.PositionWorld = HomogeneousCoordinates(pin.WorldPosition);
    surface.Normal_Vec = normal;
    surface.View_Vec = normalize((surface.PositionWorld - gCameraPosition).xyz);
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
    for (int i = 0; i < gLightNum; i++)
    {
        light.light[i] = glightinfo[i];
        light.LightNum = gLightNum;
    }
    colorResult += CalculateDirectionalLighting(surface, light);

    // add IBL part
    colorResult += CalculateIBL(surface);
    
    //colorResult = LinearTosRGB(colorResult);

    return float4(colorResult, 1.0);
#endif
    
}
