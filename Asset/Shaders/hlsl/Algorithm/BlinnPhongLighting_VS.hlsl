#include "CommonMath.hlsl"

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


struct LVertexIn
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

struct LVertexOut
{
    float4 ProjectedPosition : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TextureUV : TEXCOORD;
};

LVertexOut blinnPhongLighting(LVertexIn vInput)
{
    LVertexOut vOutput;
    
    vOutput.ProjectedPosition = mul(mul(mul(float4(vInput.Position, 1.0f), gModelMatrix), transpose(gViewMatrix)), transpose(gProjMatrix));
    vOutput.WorldPosition = mul(float4(vInput.Position, 1.0f), gModelMatrix);
    vOutput.TextureUV = vInput.Tex;
    vOutput.WorldNormal = mul(float4(vInput.Normal, 1.0f), gModelMatrix).xyz;
    
    return vOutput;
}