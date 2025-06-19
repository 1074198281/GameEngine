// ��������
static const float PI = 3.14159265359;
static const float MAX = 100000.0;
static const int SAMPLE_COUNT = 16;

// ��������
cbuffer AtmosphereBuffer : register(b0)
{
    float3 sunDirection;
    float sunIntensity;
    
    float3 rayleighScattering; // ����ɢ��ϵ�� (RGB)
    float rayleighScaleHeight; // ����ɢ��߶�˥��ϵ��
    
    float mieScattering; // ����ɢ��ϵ��
    float mieScaleHeight; // ����ɢ��߶�˥��ϵ��
    float miePreferredDirection; // ����ɢ�䷽���Բ��� (ͨ��0.76-0.99)
    
    float earthRadius; // ����뾶 (��λ:��)
    float atmosphereRadius; // ������뾶 (��λ:��)
    float3 padding0; // ����Զ��볣��������
}

cbuffer ConstantBuffer : register(b1)
{
    float3 cameraPos; // ���λ��
};

// ����Ͳ�����
Texture2D<float4> transmittanceLUT : register(t0);
SamplerState samplerLinear : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};

// ��������ܶ�
void GetDensity(float height, out float rayleighDensity, out float mieDensity)
{
    float h = max(0.0, height - earthRadius);
    rayleighDensity = exp(-h / rayleighScaleHeight);
    mieDensity = exp(-h / mieScaleHeight);
}

// �����ѧ���
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

// ��������: ��������������Ľ���
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

// �����ĳ����ĳ���򵽴������߽�ľ���
float distanceToAtmosphereBorder(float3 origin, float3 dir, float earthRadius, float atmosphereRadius)
{
    // �������������������Ľ���
    float2 intersection = raySphereIntersect(origin, dir, earthRadius, atmosphereRadius);
    
    // ����������Ľ��㣨���û�н����򷵻�0��
    return max(intersection.y, 0.0);
}

// ���㵥��ɢ��
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
        
        // ���㵱ǰ����ܶ�
        float rayleighDensity, mieDensity;
        GetDensity(height, rayleighDensity, mieDensity);
        
        // ���㵽̫���Ĺ�ѧ���
        float3 sunRayDir = normalize(sunDirection);
        float sunRayLength = distanceToAtmosphereBorder(samplePos, sunRayDir, earthRadius, atmosphereRadius);
        float2 sunOpticalDepth = ComputeOpticalDepth(samplePos, sunRayDir, sunRayLength);
        
        // ���㵽����Ĺ�ѧ���
        float2 viewOpticalDepth = ComputeOpticalDepth(startPos, rayDir, t);
        
        // ����͸����
        float3 transmittance = exp(
            -(rayleighScattering * (sunOpticalDepth.x + viewOpticalDepth.x) +
            (mieScattering * (sunOpticalDepth.y + viewOpticalDepth.y)) * 1.1));
        
        // ��λ����
        float cosTheta = dot(rayDir, sunRayDir);
        float rayleighPhase = 3.0 / (16.0 * PI) * (1.0 + cosTheta * cosTheta);
        float miePhase = (3.0 / (8.0 * PI)) *
                        ((1.0 - miePreferredDirection * miePreferredDirection) * (1.0 + cosTheta * cosTheta)) /
                        ((2.0 + miePreferredDirection * miePreferredDirection) * pow(1.0 + miePreferredDirection * miePreferredDirection - 2.0 * miePreferredDirection * cosTheta, 1.5));
        
        // �ۼ�ɢ��
        totalRayleigh += rayleighDensity * transmittance * rayleighPhase * stepSize;
        totalMie += mieDensity * transmittance * miePhase * stepSize;
    }
    
    return sunIntensity * (totalRayleigh * rayleighScattering + totalMie * mieScattering);
}


// ��������ɫ��
float4 PSMain(PSInput input) : SV_TARGET
{
    float3 rayOrigin = cameraPos;
    float3 rayDir = normalize(input.worldPos - rayOrigin);
    
    // �����������Ľ���
    float2 intersection = raySphereIntersect(rayOrigin, rayDir, earthRadius, atmosphereRadius);
    float rayLength = intersection.y;
    
    if (rayLength <= 0.0)
    {
        // ����̫��
        return float4(0.0, 0.0, 0.0, 1.0);
    }
    
    // ����ɢ��
    float3 scattering = ComputeSingleScattering(rayOrigin, rayDir, rayLength);
    
    // ���̫������
    float sunCosTheta = dot(rayDir, sunDirection);
    float sunSize = 0.9999;
    if (sunCosTheta > sunSize)
    {
        float sunGlow = smoothstep(sunSize, 1.0, sunCosTheta);
        scattering += sunIntensity * sunGlow * 10.0;
    }
    
    return float4(scattering, 1.0);
}
