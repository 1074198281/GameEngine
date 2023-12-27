Texture2D<float4> tex0 : register(t0);
SamplerState sam : register(s10);


struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float2 Tex : TEXCOORD;
};

float4 main(VertexOut pin) : SV_Target
{
	float4 result = tex0.Sample(sam, pin.Tex);
	return result;
}
