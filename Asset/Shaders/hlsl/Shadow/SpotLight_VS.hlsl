

cbuffer Spot_Light_Batch : register(b1)
{
    float4x4 gModelMatrix;
};

cbuffer Spot_Light_Frame : register(b2)
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

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    float4x4 ProjectedMat = transpose(mul(gProjMatrix, gViewMatrix)); // V^T * P^T = (P * V)^T

    vout.WorldPosition = mul(float4(vin.Position, 1.0f), gModelMatrix); // X * M = Xworld
    vout.ProjectedPosition = mul(vout.WorldPosition, ProjectedMat); // X*M*V*P = Xworld * (P * V)^ T
    vout.WorldNormal = mul(float4(vin.Normal, 0.0f), gModelMatrix).xyz;
    vout.TextureUV = vin.Tex;
    return vout;
}