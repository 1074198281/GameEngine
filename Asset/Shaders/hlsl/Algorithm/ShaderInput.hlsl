#include "CommonMath.hlsl"

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

cbuffer PerBatchConstants : register(b1)
{
    float4x4 gModelMatrix;
};

// cb2
cbuffer PerFrameConstants : register(b2)
{
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
    float4 gCameraPosition;
    int gLightNum;
    int clip_space_type; //0: opengl ,1: others 裁剪空间类型
    float2 padding2;
};

// cb3
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
    float padding0;
};

cbuffer LightInfo : register(b3)
{
    cLight glightinfo[MAX_LIGHT_NUM];
}

struct VertexIn
{
	float3 Position  : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 ProjectedPosition : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TextureUV : TEXCOORD;
};

struct PerLightPro
{
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float4 LightPosition;
    float4 LightColor;
    float4 LightDirection;
    float Insensity;
    int LightType;
    float2 padding0;
};

#define MAX_LIGHT_NUM 16

// in light properties, position is in the world coordinate
struct LightProperties
{
    PerLightPro light[MAX_LIGHT_NUM];
    int LightNum;
    float3 padding0;
};

