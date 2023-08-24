Texture2D<float4> MyTexture : register(t1);
SamplerState MySampler : register(s0);


struct VertexOut
{
    float4 PosH  : SV_POSITION;
    float3 Normal : NORMAL;
    float4 Tangent : TANGENT;
    float2 Tex: TEXCOORD;
};
float4 main(VertexOut pin) : SV_Target
{
    // Sample the texture using the sampler
    float4 sampledColor = MyTexture.Sample(MySampler, pin.Tex);
    return sampledColor;
}
