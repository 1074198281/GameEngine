struct VertexIn
{
	float3 PosL  : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 Tex : TEXCOORD;
};

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// Just pass vertex color into the pixel shader.
	vout.Normal = vin.Normal;
	vout.Tangent = vin.Tangent;
	vout.Tex = vin.Tex;

	return vout;
}