Texture2D<float4> MyTexture0[5] : register(t0);
SamplerState MySampler : register(s10);

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Normal : NORMAL;
    float2 Tex: TEXCOORD;
};
float4 main(VertexOut pin) : SV_Target
{
    float4 result = MyTexture0[0].Sample(MySampler, pin.Tex);
    return result;
}
