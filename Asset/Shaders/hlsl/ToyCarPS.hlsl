Texture2D<float4> MyTexture0 : register(t0);
SamplerState MySampler : register(s0);

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Normal : NORMAL;
    float2 Tex: TEXCOORD;
};
float4 main(VertexOut pin) : SV_Target
{
    float4 result = MyTexture0.Sample(MySampler, pin.Tex);
    return result;
}
