cbuffer cbCommon : register(b1)
{
    float4x4 gModelMatrix;
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
};

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
};

VertexOut main(in VertexIn vin)
{
    VertexOut vout;
    
    float4x4 mat = mul(transpose(gModelMatrix), mul(transpose(gViewMatrix), transpose(gProjMatrix)));

    vout.PosH = mul(float4(vin.PosL, 1.0f), mat);
    vout.PosW = mul(float4(vin.PosL, 1.0f), transpose(gModelMatrix));
    
    return vout;
}