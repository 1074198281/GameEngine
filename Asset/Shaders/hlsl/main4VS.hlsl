cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 Tex: TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 Tex: TEXCOORD;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
    vout.Normal = vin.Normal;
    vout.Tangent = vin.Tangent;
    vout.Tex = vin.Tex;

	return vout;
}