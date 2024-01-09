#include "../Algorithm/Lighting.hlsl"

Texture2D<float4> tex0[3] : register(t0);
SamplerState sam : register(s10);

cbuffer cbMaterialConstants : register(b0)
{
    float4 gEyePos;
    float4 gGlobalInfiniteLightPos;
    int gLightNum;
    float4 gLightDir[MAX_LIGHT_NUM];
};

cbuffer cbCommon : register(b1)
{
    float4x4 gModelMatrix;
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

float4 main(VertexOut pin) : SV_Target
{
    float LumenIns = 1.0f;
    float4 LightColor = tex0[0].Sample(sam, pin.Tex);
    
    float4 WorldPos = pin.PosL.xyzw / pin.PosL.w;
    float3 LightDir = normalize((gGlobalInfiniteLightPos - WorldPos).xyz);
    
    float3 NormalDir = normalize(mul(float4(pin.Normal, 0.0f), transpose(gModelMatrix)).xyz);
    
    return LambertLighting(LumenIns, LightColor, LightDir, NormalDir);
}
