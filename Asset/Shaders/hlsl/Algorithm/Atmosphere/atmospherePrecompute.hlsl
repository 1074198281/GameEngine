// 常量定义
static const float PI = 3.14159265359;
static const float MAX = 100000.0;
static const int SAMPLE_COUNT = 16;

// 大气参数
cbuffer AtmosphereBuffer : register(b0)
{
    float3 sunDirection;
    float sunIntensity;
    
    float3 rayleighScattering; // 瑞利散射系数 (RGB)
    float rayleighScaleHeight; // 瑞利散射高度衰减系数
    
    float mieScattering; // 米氏散射系数
    float mieScaleHeight; // 米氏散射高度衰减系数
    float miePreferredDirection; // 米氏散射方向性参数 (通常0.76-0.99)
    
    float earthRadius; // 地球半径 (单位:米)
    float atmosphereRadius; // 大气层半径 (单位:米)
    float3 padding0; // 填充以对齐常量缓冲区
}

cbuffer ConstantBuffer : register(b1)
{
    float3 cameraPos; // 相机位置
};

// 纹理和采样器
Texture2D<float4> transmittanceLUT : register(t0);
SamplerState samplerLinear : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};

// 计算大气密度
void GetDensity(float height, out float rayleighDensity, out float mieDensity)
{
    float h = max(0.0, height - earthRadius);
    rayleighDensity = exp(-h / rayleighScaleHeight);
    mieDensity = exp(-h / mieScaleHeight);
}

// 计算光学深度
float2 ComputeOpticalDepth(float3 startPos, float3 rayDir, float rayLength)
{
    float2 opticalDepth = 0.0;
    float stepSize = rayLength / SAMPLE_COUNT;
    
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        float t = (i + 0.5) * stepSize;
        float3 samplePos = startPos + rayDir * t;
        float height = length(samplePos) - earthRadius;
        
        float rayleighDensity, mieDensity;
        GetDensity(height, rayleighDensity, mieDensity);
        
        opticalDepth.x += rayleighDensity * stepSize;
        opticalDepth.y += mieDensity * stepSize;
    }
    
    return opticalDepth;
}

// 辅助函数: 计算射线与球体的交点
float2 raySphereIntersect(float3 origin, float3 dir, float sphereRadius, float outerRadius)
{
    float3 oc = origin;
    float a = dot(dir, dir);
    float b = 2.0 * dot(oc, dir);
    float c = dot(oc, oc) - outerRadius * outerRadius;
    float disc = b * b - 4 * a * c;
    
    if (disc < 0.0)
        return float2(MAX, -MAX);
    
    float t0 = (-b - sqrt(disc)) / (2.0 * a);
    float t1 = (-b + sqrt(disc)) / (2.0 * a);
    return float2(t0, t1);
}

// 计算从某点沿某方向到达大气层边界的距离
float distanceToAtmosphereBorder(float3 origin, float3 dir, float earthRadius, float atmosphereRadius)
{
    // 计算射线与大气层球体的交点
    float2 intersection = raySphereIntersect(origin, dir, earthRadius, atmosphereRadius);
    
    // 返回正距离的交点（如果没有交点则返回0）
    return max(intersection.y, 0.0);
}

// 计算单次散射
float3 ComputeSingleScattering(float3 startPos, float3 rayDir, float rayLength)
{
    float3 totalRayleigh = 0.0;
    float3 totalMie = 0.0;
    float stepSize = rayLength / SAMPLE_COUNT;
    
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        float t = (i + 0.5) * stepSize;
        float3 samplePos = startPos + rayDir * t;
        float height = length(samplePos) - earthRadius;
        
        // 计算当前点的密度
        float rayleighDensity, mieDensity;
        GetDensity(height, rayleighDensity, mieDensity);
        
        // 计算到太阳的光学深度
        float3 sunRayDir = normalize(sunDirection);
        float sunRayLength = distanceToAtmosphereBorder(samplePos, sunRayDir, earthRadius, atmosphereRadius);
        float2 sunOpticalDepth = ComputeOpticalDepth(samplePos, sunRayDir, sunRayLength);
        
        // 计算到相机的光学深度
        float2 viewOpticalDepth = ComputeOpticalDepth(startPos, rayDir, t);
        
        // 计算透射率
        float3 transmittance = exp(
            -(rayleighScattering * (sunOpticalDepth.x + viewOpticalDepth.x) +
            (mieScattering * (sunOpticalDepth.y + viewOpticalDepth.y)) * 1.1));
        
        // 相位函数
        float cosTheta = dot(rayDir, sunRayDir);
        float rayleighPhase = 3.0 / (16.0 * PI) * (1.0 + cosTheta * cosTheta);
        float miePhase = (3.0 / (8.0 * PI)) *
                        ((1.0 - miePreferredDirection * miePreferredDirection) * (1.0 + cosTheta * cosTheta)) /
                        ((2.0 + miePreferredDirection * miePreferredDirection) * pow(1.0 + miePreferredDirection * miePreferredDirection - 2.0 * miePreferredDirection * cosTheta, 1.5));
        
        // 累加散射
        totalRayleigh += rayleighDensity * transmittance * rayleighPhase * stepSize;
        totalMie += mieDensity * transmittance * miePhase * stepSize;
    }
    
    return sunIntensity * (totalRayleigh * rayleighScattering + totalMie * mieScattering);
}


// 主像素着色器
float4 PSMain(PSInput input) : SV_TARGET
{
    float3 rayOrigin = cameraPos;
    float3 rayDir = normalize(input.worldPos - rayOrigin);
    
    // 计算与大气层的交点
    float2 intersection = raySphereIntersect(rayOrigin, rayDir, earthRadius, atmosphereRadius);
    float rayLength = intersection.y;
    
    if (rayLength <= 0.0)
    {
        // 看向太空
        return float4(0.0, 0.0, 0.0, 1.0);
    }
    
    // 计算散射
    float3 scattering = ComputeSingleScattering(rayOrigin, rayDir, rayLength);
    
    // 添加太阳光晕
    float sunCosTheta = dot(rayDir, sunDirection);
    float sunSize = 0.9999;
    if (sunCosTheta > sunSize)
    {
        float sunGlow = smoothstep(sunSize, 1.0, sunCosTheta);
        scattering += sunIntensity * sunGlow * 10.0;
    }
    
    return float4(scattering, 1.0);
}
