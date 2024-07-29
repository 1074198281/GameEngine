struct PresentOut
{
    float4 position : SV_POSITION;
    float2 texUV : TEX;
};

SamplerState LinearWarp : register(s16);
SamplerState LinearClamp : register(s17);
SamplerState PointWarp : register(s18);
SamplerState PointClamp : register(s19);
SamplerState AnisotropicWarp : register(s20);
SamplerState AnisotropicClamp : register(s21);

Texture2D<float4> PresentMap : register(t0);

cbuffer OverlaySubParams : register(b0)
{
    float2 _ScreenSize;
    float2 _padding0;
};

float4 main(PresentOut PresentIn) : SV_Target0
{
    float2 uv = float2(PresentIn.texUV.x, PresentIn.texUV.y);
    uv.x *= _ScreenSize.x / _ScreenSize.y;
    
    float2 finalUV = PresentIn.texUV + uv;
    finalUV.x /= (_ScreenSize.x / _ScreenSize.y + 1);
    finalUV.y /= 2;
    return PresentMap.Sample(AnisotropicWarp, finalUV);
}