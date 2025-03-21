/*
for volumetric light, about how light scattering occurs
https://advances.realtimerendering.com/s2014/wronski/bwronski_volumetric_fog_siggraph2014.pdf
considering Mie scattering and Rayleigh scattering

calculate phase function (HG phase function)
*/

#include "Lighting.hlsl"
#define MAX_LIGHT_NUM 100

struct VolumetricLightVSOut
{
    float4 position : SV_POSITION;
    float4 positionCS : POSITION;
    float2 texUV : TEX;
};

cbuffer gLightInfo : register(b0)
{
    cLight gLights[MAX_LIGHT_NUM];
};

cbuffer VolumetricLightConstants : register(b1)
{
    float4x4 gInvViewProj;
    float4 gCameraPos;
    float gScreenWidth;
    float gScreenHeight;
    float gMarchingStep;
    float gSampleIntensity;
};

SamplerState LinearWarp : register(s16);
SamplerState LinearClamp : register(s17);

Texture2D<float> LightDepthMap[MAX_LIGHT_NUM] : register(t0);
Texture2D<float4> PresentMap : register(t101);

float GetCurrentPositionIntensity(float4 currPos, cLight l)
{
    float4 light_pos_vec = currPos - l.gLightPosition;
    float angle = dot(light_pos_vec, l.gLightDirection) / (length(light_pos_vec) * length(l.gLightDirection));
    if (acos(angle) > l.penumbraAngle)
    {
        return 0;
    }
    
    float4 lightProjPos = mul(mul(currPos, l.gLightViewMatrix), l.gLightProjectMatrix);
    float depthInLight = LightDepthMap[l.lightIndex].Load(int3(lightProjPos.x / lightProjPos.w * gScreenWidth, lightProjPos.y / lightProjPos.w * gScreenHeight, 0));
    if (lightProjPos.z / lightProjPos.w > depthInLight)
    {
        return 0;
    }
    
    return 
}

float4 main(VolumetricLightVSOut PresentIn) : SV_Target0
{
    float4 worldPos = mul(PresentIn.positionCS, gInvViewProj);
    float4 marchingDir = worldPos - gCameraPos;
    float marchingLength = length(marchingDir);
    float _step = marchingLength / gMarchingStep;
    for (int i = 0; i < MAX_LIGHT_NUM; i++)
    {
        cLight l = gLights[i];
        if(l.isCastShadow == 0)
        {
            continue;
        }
        
        float _intensity = 0;
        for (float m = _step; m < marchingLength; m += _step)
        {
            float4 currPos = gCameraPos + marchingDir * m;
            _intensity += GetCurrentPositionIntensity(currPos, l);

        }
        _intensity /= gMarchingStep;
        
        

    }
    
    return PresentMap.Sample(LinearWarp, PresentIn.texUV);
}