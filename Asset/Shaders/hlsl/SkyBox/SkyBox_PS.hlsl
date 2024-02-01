SamplerState defaultSampler : register(s10);
SamplerState cubeMapSampler : register(s12);

cbuffer PSConstants : register(b0)
{
    float TextureLevel;
};

TextureCube<float3> radianceIBLTexture      : register(t10);

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 viewDir : TEXCOORD3;
};

float4 main(VSOutput vsOutput) : SV_Target0
{
    return float4(radianceIBLTexture.SampleLevel(defaultSampler, vsOutput.viewDir, TextureLevel), 1);
}
