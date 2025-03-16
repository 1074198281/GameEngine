struct VolumetricLightVSOut
{
    float4 position : SV_POSITION;
    float2 texUV : TEX;
};

SamplerState LinearWarp : register(s16);
SamplerState LinearClamp : register(s17);

Texture2D<float4> PresentMap : register(t0);

float4 main(VolumetricLightVSOut Presentin) : SV_Target0
{
    return PresentMap.Sample(LinearWarp, Presentin.texUV);
}