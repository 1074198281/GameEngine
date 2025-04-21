/*
for volumetric light, about how light scattering occurs
https://advances.realtimerendering.com/s2014/wronski/bwronski_volumetric_fog_siggraph2014.pdf
considering Mie scattering and Rayleigh scattering

calculate phase function (HG phase function)
*/

#include "Lighting.hlsl"

struct VolumetricLightVSOut
{
    float4 position : SV_POSITION;
    float2 texUV : TEX;
};

cbuffer gLightInfo : register(b0)
{
    cLight gLights[3];
};

cbuffer VolumetricLightConstants : register(b1)
{
    float4x4 gInvViewProj;
    float4 gCameraPos;
    float gScreenWidth;
    float gScreenHeight;
    float gMarchingStep;
    float gSampleIntensity;
    float gCameraNearZ;
    float gCameraFarZ;
    float2 padding0;
};

SamplerState LinearWarp : register(s16);
SamplerState LinearClamp : register(s17);

Texture2D<float> gCameraDepthMap : register(t0);
Texture2D<float> LightDepthMap[3] : register(t1);
Texture2D<float4> PresentMap : register(t101);

float GetCurrentPositionIntensity(float4 currPos, cLight l)
{
    float4 light_pos_vec = currPos - l.gLightPosition;
    float angle = dot(light_pos_vec, l.gLightDirection) / (length(light_pos_vec) * length(l.gLightDirection));
    angle = acos(angle);
    if (angle > l.penumbraAngle)
    {
        return 0;
    }
    
    float4 lightProjPos = mul(mul(currPos, transpose(l.gLightViewMatrix)), transpose(l.gLightProjectMatrix));
    int3 load = int3((lightProjPos.x / lightProjPos.w + 1) * 0.5 * gScreenWidth, (1 - lightProjPos.y / lightProjPos.w * 0.5) * gScreenHeight, 0);
    float depthInLight = LightDepthMap[l.gDescriptorOffset].Load(load);
    if (lightProjPos.z / lightProjPos.w > depthInLight)
    {
        return 0;
    }
    
    return gSampleIntensity;
}

float4 main(VolumetricLightVSOut PresentIn) : SV_Target0
{
    float depth = gCameraDepthMap.Load(int3(PresentIn.texUV.x * gScreenWidth, PresentIn.texUV.y * gScreenHeight, 0));
    float2 interp = lerp(float2(-1, 1), float2(1, -1), PresentIn.texUV);
    float4 projPos = float4(interp, depth, 1.0);
    float4 worldPos = mul(projPos, gInvViewProj);
    worldPos = worldPos / worldPos.w;
    float4 marchingDir = worldPos - gCameraPos;
    float marchingLength = length(marchingDir);
    marchingDir = marchingDir / length(marchingDir);
    float _step = marchingLength / gMarchingStep;
    
    float4 color = float4(0.0, 0.0, 0.0, 0.0);
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
        //_intensity /= gMarchingStep;
        
        color += _intensity * l.gLightColor;
    }
    
    return color + PresentMap.Sample(LinearWarp, PresentIn.texUV);
}