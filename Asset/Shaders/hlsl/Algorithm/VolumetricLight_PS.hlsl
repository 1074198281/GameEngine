/*
for volumetric light, about how light scattering occurs
https://advances.realtimerendering.com/s2014/wronski/bwronski_volumetric_fog_siggraph2014.pdf
considering Mie scattering and Rayleigh scattering

calculate phase function (HG phase function)
*/

#include "Atmosphere/BaseScattering.hlsl"
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
    int gLightNum;
    float padding0;
};

SamplerState LinearWarp : register(s16);
SamplerState LinearClamp : register(s17);

Texture2D<float> gCameraDepthMap : register(t0);
Texture2D<float> LightDepthMap : register(t1);
Texture2D<float4> PresentMap : register(t101);

float3 GetCurrentPositionIntensity(float4 currPos, cLight l)
{
    float4 light_pos_vec = currPos - l.gLightPosition;
    float dis = length(light_pos_vec);
    float angle = dot(light_pos_vec, l.gLightDirection) / (dis * length(l.gLightDirection));
    angle = acos(angle);
    if (angle > l.penumbraAngle)
    {
        return 0;
    }
    
    float4 lightProjPos = mul(mul(currPos, transpose(l.gLightViewMatrix)), transpose(l.gLightProjectMatrix));
    //int3 load = int3((lightProjPos.x / lightProjPos.w + 1) * 0.5 * gScreenWidth, (1 - lightProjPos.y / lightProjPos.w * 0.5) * gScreenHeight, 0);
    int3 load = int3(lerp(float2(-1, 1), float2(1, -1), float2(lightProjPos.x / lightProjPos.w, lightProjPos.y / lightProjPos.w)), 0);
    float depthInLight = LightDepthMap.Load(load);
    if (lightProjPos.z / lightProjPos.w >= depthInLight)
    {
        return 0;
    }
    
    float3 viewDir = normalize(gCameraPos.xyz - currPos.xyz);
    float3 lightDir = normalize(l.gLightPosition.xyz - currPos.xyz);
    float cos_theta = dot(viewDir, lightDir);
    
    AtmosphereParam atParam;
    atParam.Rayliegh_Scalar_Height = 8500;
    atParam.Mie_Scalar_Height = 1200;
    atParam.Mie_Anisotropy_G = 0.8;
    
    float3 rayleigh = RayleighCoefficient(atParam, 8501) * RayleighPhase(cos_theta);
    float3 mie = MieCoefficient(atParam, 8501) * MiePhase(atParam, cos_theta);
    // 混合散射
    float3 scattering = lerp(rayleigh * 0.1, mie, 0.7);
    
    // 反平方衰减
    float attenu = 1.0 / (1 + dis * dis);
    
    return scattering * attenu * l.gLightColor.xyz * l.gInsensity;
}

//-------------------------Spot Light-------------------------//
// 计算视线与聚光灯锥体的交点
// 返回交点参数 t_start 和 t_end（沿视线方向）
bool GetLightIntersection(float3 rayOrigin, float3 rayDir, cLight l, float maxMarchingLength, out float t_start, out float t_end)
{
    float3 lightToRay = rayOrigin - l.gLightPosition.xyz;
    float lightCosAngle = cos(l.penumbraAngle);
    
    // 计算光线与锥体的交点（二次方程）
    float3 D = normalize(l.gLightDirection).xyz;
    float3 CO = rayOrigin - l.gLightPosition.xyz;
    
    float a = dot(rayDir, D) * dot(rayDir, D) - lightCosAngle * lightCosAngle;
    float b = 2.0 * (dot(rayDir, D) * dot(CO, D) - dot(rayDir, CO) * lightCosAngle * lightCosAngle);
    float c = dot(CO, D) * dot(CO, D) - dot(CO, CO) * lightCosAngle * lightCosAngle;
    
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return false; // 无交点
    }
    
    if (a > 0)
    {
        t_start = (-b - sqrt(discriminant)) / (2 * a);
        t_end = (-b + sqrt(discriminant)) / (2 * a);
    }
    else if(a < 0)
    {
        t_end = (-b - sqrt(discriminant)) / (2 * a);
        t_start = (-b + sqrt(discriminant)) / (2 * a);
    }
    else
    {
        return false;
    }

    
    // 确保 t_start < t_end 且在视距范围内
    if (t_end < 0 || t_start > maxMarchingLength)
    {
        return false;
    }
    
    t_start = max(t_start, 0);
    t_end = min(t_end, maxMarchingLength);
    
    return true;
}

float4 GetSpotLightIntensity(float3 marchingDir, cLight l, float marchingLength, float2 screenUV)
{
    float t_start = 0.0f;
    float t_end = 0.0f;
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    if (!GetLightIntersection(gCameraPos.xyz, marchingDir.xyz, l, marchingLength, t_start, t_end))
    {
        return color;
    }
    
    // 仅在有效区间内积分
    float _step = max((t_end - t_start) / gMarchingStep, 0.2); // 动态步长
    float3 _intensity = float3(0.0, 0.0, 0.0);
    float3 prevDensity = float3(0.0, 0.0, 0.0);
        
    // 添加抖动减少条带
    float offset = frac(dot(screenUV, float2(12.9898, 78.233))) * _step;
    int it = 0;
    
    for (float t = t_start + offset; t < t_end; t += _step)
    {
        float4 currPos = float4(gCameraPos.xyz + marchingDir * t, 1.0f);
        float3 currentDensity = GetCurrentPositionIntensity(currPos, l);
            
        // 梯形积分法平滑过渡
        _intensity += (prevDensity + currentDensity) * 0.5 * _step;
        prevDensity = currentDensity;
        it++;
        if (it >= gMarchingStep)
        {
            break;
        }
    }
        
    color = float4(_intensity, 1.0f);
    
    return color;
}



//-------------------------Main Calc-------------------------//
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
    
    float4 color = float4(0.0, 0.0, 0.0, 0.0);
    for (int i = 0; i < gLightNum; i++)
    {
        cLight l = gLights[i];
        if(l.isCastShadow == 0 || i >= gLightNum)
        {
            continue;
        }
        
        float4 _intensity = float4(0, 0, 0, 0);
        
        if (l.gLightType == LIGHT_TYPE_OMNI)
        {
            
        }
        else if (l.gLightType == LIGHT_TYPE_SPOT)
        {
            _intensity = GetSpotLightIntensity(marchingDir.xyz, l, marchingLength, PresentIn.texUV);
        }
        else if (l.gLightType == LIGHT_TYPE_INFI)
        {
            
        }
        else if (l.gLightType == LIGHT_TYPE_AREA)
        {
            
        }
        
        color += _intensity;
    }
    
    color += PresentMap.Sample(LinearClamp, PresentIn.texUV);
    
    return color;
}
