#include "../Algorithm/Lighting.hlsl"

Texture2D<float4> BaseColorTexture : register(t0);
Texture2D<float4> MetallicRoughnessTexture : register(t1);
Texture2D<float4> OcclusionTexture : register(t2); 
Texture2D<float4> EmissiveTexture : register(t3);
Texture2D<float4> NormalTexture : register(t4);

TextureCube<float3> RadianceIBLTexture : register(t20);
TextureCube<float3> IrradianceIBLTexture : register(t21);
Texture2D<float4> BRDF_LUT : register(t32);

Texture2D<float> ShadowMaps[100] : register(t100);

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
    float depth_bias;
    float2 padding1;
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
    float gScreenWidth;
    float gScreenHeight;
};

// cb3
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

float3 CalculateDirectionalLighting(SurfaceProperties surface, cLight light)
{
    float3 irradiance = float3(0.0, 0.0, 0.0);
    // calculate direct and indirect lighting
    // direct lighting
    float4 Light_Position = HomogeneousCoordinates(light.gLightPosition);
    float4 World_Position = HomogeneousCoordinates(surface.PositionWorld);    
    float3 Light_Vec = normalize(Light_Position.xyz - World_Position.xyz);
    float3 Half_Vec = normalize(Light_Vec + surface.View_Vec);
    float N_dot_L = max(dot(surface.Normal_Vec, Light_Vec), 0);

    float distance = length(World_Position - Light_Position);
    float attenuation = 1.0 / (distance * distance);
    float insensity = light.gInsensity;
    float3 Light_Color = light.gLightColor.rgb;
    float3 radiance = insensity * Light_Color * attenuation;

    // indirect lighting, based on Cook-Torrence
    
    // calc metal albedo, interpolate by metallic to calculate albedo
    float3 F_Value = lerp(F0, surface.Albedo, surface.Metallic);
    float3 F = FresnalSchlick(surface.N_dot_V, surface.Albedo);
    
    // calc Normal Distribution by GGX
    float3 D = DistributionGGX(surface.Normal_Vec, Half_Vec, surface.Roughness);
    
    // calc Geometry Distribution by Smith
    float3 G = GeometrySmith(surface.Normal_Vec, surface.View_Vec, Light_Vec, surface.Roughness);
    
    // calc specular FDG / 4(n * view)(n * light);
    float3 specular = F * D * G / max(0.001, 4 * saturate(surface.N_dot_V) * saturate(N_dot_L));

    // calc kd, F = kS
    float3 kD = (1 - F) * (1 - surface.Metallic);

    // calc diffuse
    float3 diffuse = kD * surface.Albedo / PI;
        
    irradiance = (diffuse + specular) * radiance * N_dot_L;// * light.isCastShadow;

    return irradiance;
}

float4 main(VertexOut pin) : SV_Target
{
#ifdef LAMBERT
    float LumenIns = 4.0;
    float4 LightColor = float4(1.0, 1.0, 1.0, 1.0);
    
    float3 NormalDir = normalize(mul(float4(pin.WorldNormal, 0.0f), gModelMatrix).xyz);
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
    surface.View_Vec = normalize((gCameraPosition - surface.PositionWorld).xyz);
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
    
    // consider reuse about float4 and float
    float4 p1 = float4(0, 0, 0, 0);
    float p2 = 0;
    
    // add direct and reflect light    
    int idx = 0;
    for (; idx < gLightNum; idx++)
    {
        if (!glightinfo[idx].isCastShadow)
        {
            continue;
        }
        
        if (glightinfo[idx].gLightType == 0)
        {
            
        }
        else if (glightinfo[idx].gLightType == 1)
        {
            //spot light clac angle if needs to end
            p1 = pin.WorldPosition - glightinfo[idx].gLightPosition;
            p2 = dot(p1, glightinfo[idx].gLightDirection) / (length(p1) * length(glightinfo[idx].gLightDirection));
            if (acos(p2) > glightinfo[idx].penumbraAngle)
            {
                continue;
            }
        }
        else if (glightinfo[idx].gLightType == 2)
        {

        }
        else if (glightinfo[idx].gLightType == 3)
        {
            
        }
        
        Texture2D<float> shadow = ShadowMaps[glightinfo[idx].gDescriptorOffset];
        // calculate cuurent world pos in light view coordinate to get if has light cast
        p1 = mul(pin.WorldPosition, transpose(glightinfo[idx].gLightViewMatrix));
        p1 = mul(p1, transpose(glightinfo[idx].gLightProjectMatrix));
        // in dx coordinate, screen coordinate x+ points to right and y+ points to down, so when get depth from shadowmap,
        // we have to convert to x+ points to right and y+ points to up in its coordinate
        int2 uv = int2(((p1.x / p1.w) + 1) * 0.5  * gScreenWidth, (-p1.y / p1.w + 1) * 0.5 * gScreenHeight);
        p2 = shadow.Load(int3(uv, 0));
        if ((p1.z / p1.w) - depth_bias > p2)
        {
            continue;
        }
        
        colorResult += CalculateDirectionalLighting(surface, glightinfo[idx]);
    }

    // add IBL part
    colorResult += CalculateIBL(surface);
    
    //colorResult = LinearTosRGB(colorResult);

    return float4(colorResult, 1.0);
#endif
    
}
