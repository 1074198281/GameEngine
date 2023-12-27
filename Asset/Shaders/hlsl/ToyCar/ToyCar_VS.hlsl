

cbuffer cbCommon : register(b1)
{
    float4x4 gModelMatrix;
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;
	
    float4x4 mat = mul(transpose(gModelMatrix), mul(transpose(gViewMatrix), transpose(gProjMatrix)));

    vout.PosH = mul(float4(vin.PosL, 1.0f), mat);
    vout.PosL = mul(float4(vin.PosL, 1.0f), transpose(gModelMatrix));
	vout.Normal = vin.Normal;
	vout.Tex = vin.Tex;

	return vout;
}