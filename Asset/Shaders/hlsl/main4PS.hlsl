struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 Tex: TEXCOORD;
};
float4 main(VertexOut pin) : SV_Target
{
    return float4(pin.Normal, 1.0f);
}
