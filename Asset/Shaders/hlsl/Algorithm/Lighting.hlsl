#include "CommonMath.hlsl"
#define PBR

#ifdef LAMBERT
float4 LambertLighting(float Insensity, float4 LightColor, float3 LightDir, float3 NormalDir)
{
    return float4(Insensity * LightColor.xyz * max(0.0f, dot(LightDir, NormalDir)), 1.0f);
}
#endif

#ifdef PBR

struct SurfaceProperties
{
    float4 PositionWorld;
    float3 Normal_Vec;
    float3 View_Vec;
    float N_dot_V;
    float roughness;
    float roughness_sq;
    float roughness_mul_4;
    
    float3 Albedo;
    float Metallic;
    float Roughness;
    float AmbientOcclusion;
};

struct PerLightPro
{
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float4 LightPosition;
    float4 LightColor;
    float4 LightDirection;
    int ShadowMapIndex;
    int LightType;
    float Insensity;
    half IsCastShadow;
};

// in light properties, position is in the world coordinate
struct LightProperties
{
    PerLightPro light[MAX_LIGHT_NUM];
    int LightNum;
    float3 padding0;
};

// padding0 contains 
// bool iscastshadow
// uint8 lightindex
// char padding[2]
struct cLight
{
    float4x4 gLightViewMatrix;
    float4x4 gLightProjectMatrix;
    float4 gLightPosition;
    float4 gLightColor;
    float4 gLightDirection;
    int gShadowMapIndex;
    int gLightType;
    float gInsensity;
    uint padding0;
};

// >> bits represents its pos int a int
// for biggest 8bits locates at right side
uint GetIsCastShadow(uint padding)
{
    return ((padding >> 0) << 24) >> 24;
}

uint GetLightIndex(uint padding)
{
    return ((padding >> 8) << 24) >> 24;
}

float3 LinearTosRGB(float3 input)
{
    input = input / (input + float3(1.0, 1.0, 1.0)); 
    input = pow(abs(input), float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)); 
    return input;
}

float3 sRGBToLinear(float3 input)
{
    input = log(input) / log(2.2);
    input = input / (float3(1.0, 1.0, 1.0) - input);
    return input;
}

/*
* FresnalSchlick is reflectance on the surface
* FresnelSchlickRoughness used for IBL 
*/

float3 FresnalSchlick(float costheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - costheta, 5.0);
}

float3 FresnelSchlickRoughness(float costheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - costheta, 5.0);
}

float DistributionGGX(float3 Normal, float3 HalfVec, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(Normal, HalfVec), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1 - k) + k;
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
 
    return ggx1 * ggx2;
}

#endif