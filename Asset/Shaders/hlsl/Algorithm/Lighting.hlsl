#define MAX_LIGHT_NUM 16

#define PBR

#ifdef LAMBERT
float4 LambertLighting(float Insensity, float4 LightColor, float3 LightDir, float3 NormalDir)
{
    return float4(Insensity * LightColor.xyz * max(0.0f, dot(LightDir, NormalDir)), 1.0f);
}
#endif

#ifdef PBR

/*
pbr relative
[out] float4 pixelColor
[in]  float2 TexCoord
[in]  float3 WorldPos
[in]  float3 Normal

float3 CameraPos        ���λ��

//Material
float3 Albedo           �����ʣ��������ʱ������ɫ��
float metallic          ������
float roughness         �ֲڶ�
float AO                �������ڱ�

float attenuation       ��Դ˥��ϵ��
*/

struct MaterialProperties
{
    float3 Albedo;
    float metallic;
    float roughness;
    float AO;
};

struct InputLayout
{
    float3 WorldPos;
    float3 CameraPos;
    float3 Normal;
    float2 TexCoord;
};

static const float3 F0 = float3(0.04, 0.04, 0.04);     //�ǵ���ʽ�������Ϊ��һ������0.04
static const float PI = 3.14159265358979;

float3 LinearTosRGB(float3 input)
{
    input = input / (input + float3(1.0, 1.0, 1.0)); // ɫ��ӳ��
    input = pow(abs(input), float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)); // ٤��У��
    return input;
}

float3 sRGBToLinear(float3 input)
{
    input = log(input) / log(2.2);
    input = input / (float3(1.0, 1.0, 1.0) - input);
    return input;
}

float3 FresnalSchlick(float costheta, float3 F0)
{
    //����߹���������ı��������ݷ���������
    return F0 + (1.0 - F0) * pow(1.0 - costheta, 5.0);
}

float DistributionGGX(float3 Normal, float3 HalfVec, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(Normal, HalfVec), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1 - k) + k;
    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
 
    return ggx1 * ggx2;
}


float3 DirectLighting(InputLayout input, MaterialProperties mat, float4 LightPosition[MAX_LIGHT_NUM], int LightNum)
{
    float3 N_vec = normalize(input.Normal);
    float3 View_vec = normalize(input.CameraPos - input.WorldPos);
    
    float3 LightColor = float3(1.0, 1.0, 1.0);
    float3 LightOut = float3(0.0, 0.0, 0.0);
    for (int light = 0; light < LightNum; light++)
    {
        //�������˥������ƽ��˥��
        float3 CurrentLightPosition = LightPosition[light].xyz;
        float3 L_vec = normalize(CurrentLightPosition - input.WorldPos);
        float3 Half_vec = normalize(L_vec + View_vec);

        float distance = length(CurrentLightPosition - input.WorldPos);
        float attenuation = 1.0f / (distance * distance);
        float3 radiance = LightColor * 1;

        //����DFG�е�F
        float3 F = lerp(F0, mat.Albedo, mat.metallic);
        F = FresnalSchlick(max(dot(Half_vec, View_vec), 0.0f), F);
        
        float NDF = DistributionGGX(N_vec, Half_vec, mat.roughness);
        float G = GeometrySmith(N_vec, View_vec, L_vec, mat.roughness);
        
        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N_vec, View_vec), 0.0) * max(dot(N_vec, L_vec), 0.0);
        float3 specular = numerator / max(denominator, 0.001);
        
        float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
        kD *= 1.0 - mat.metallic;

        float NdotL = max(dot(N_vec, L_vec), 0.0);
        LightOut += (kD * mat.Albedo / PI + specular) * radiance * NdotL;
    }
    
    return LightOut;
}


float4 PBROutput(InputLayout input, MaterialProperties mat, float4 LightPosition[MAX_LIGHT_NUM], int LightNum)
{
    float4 pixelColor = float4(1.0, 1.0, 1.0, 1.0);
    
    float3 N = normalize(input.Normal);
    float3 V = normalize(input.CameraPos - input.WorldPos);
    
    float3 directLighting = DirectLighting(input, mat, LightPosition, LightNum);
    
    float3 ambient = float3(0.03, 0.03, 0.03) * mat.Albedo * mat.AO;
    pixelColor.rgb = ambient + directLighting;

    pixelColor.rgb = LinearTosRGB(pixelColor.rgb);

    return pixelColor;
}

#endif