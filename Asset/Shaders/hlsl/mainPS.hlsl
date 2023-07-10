cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
};


struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(VertexOut pin) : SV_Target
{
    return pin.Color;
}
