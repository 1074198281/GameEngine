

cbuffer Infi_Light_Batch : register(b1)
{
    float4x4 gModelMatrix;
};

cbuffer Infi_Light_Frame : register(b2)
{
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
    float4 gLightPos;
    
};

struct VertexIn
{
    float3 Position : POSITION;
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

VertexOut mainVS(VertexIn vin)
{
    VertexOut vout;
    
    
    
    
    return vout;
}