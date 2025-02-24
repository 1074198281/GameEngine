

Texture2D<float4> ShadowMap : register(t0);
SamplerState DefaultSampler : register(s10);

cbuffer Spot_Light_Batch : register(b1)
{
    float4x4 gModelMatrix;
};

cbuffer Spot_Light_Frame : register(b2)
{
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
    float4 gLightPos;
    float gScreenWidth;
    float gScreenHeight;
    float2 padding0;
};

//struct VertexIn
//{
//    float3 Position : POSITION;
//    float3 Normal : NORMAL;
//    float2 Tex : TEXCOORD;
//};

struct VertexOut
{
    float4 ProjectedPosition : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TextureUV : TEXCOORD;
};

float4 main(VertexOut pin) : SV_Target
{
    float2 uv = float2(pin.ProjectedPosition.x / gScreenWidth, pin.ProjectedPosition.y / gScreenHeight);
    float4 color = ShadowMap.Sample(DefaultSampler, uv);
    return color;
}