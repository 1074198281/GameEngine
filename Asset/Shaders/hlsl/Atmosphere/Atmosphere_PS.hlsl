#include "AtmosphereDefine.hlsl"

struct AtmosphereVSOut
{
    float4 position : SV_POSITION;
    float2 texUV : TEX;
};

cbuffer AtmosphereConstants : register(b0)
{
    AtmosphereParam gAtmosphereParam;
};  

float4 main(AtmosphereVSOut pin) : SV_Target
{
    float4 color = float4(0.0, 0.0, 0.0, 1.0);

    
    return color;
}