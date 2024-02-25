#define MAX_LIGHT_NUM 16

#define PBR

#ifdef LAMBERT
float4 LambertLighting(float Insensity, float4 LightColor, float3 LightDir, float3 NormalDir)
{
    return float4(Insensity * LightColor.xyz * max(0.0f, dot(LightDir, NormalDir)), 1.0f);
}
#endif

#ifdef PBR

struct SurfaceProperties
{
    float4 PositionWorld;
    float3 Normal_Vec;
    float3 View_Vec;
    float N_dot_V;
    float roughness;
    float roughness_sq;
    float roughness_mul_4;
    
    float3 Albedo;
    float Metallic;
    float Roughness;
    float AmbientOcclusion;
};

// in light properties, position is in the world coordinate
struct LightProperties
{
    float4 LightPosition[MAX_LIGHT_NUM];
    float4 LightColor[MAX_LIGHT_NUM];
    int LightNum;
};

static const float3 F0 = float3(0.04, 0.04, 0.04);     
static const float PI = 3.14159265358979;

float3 LinearTosRGB(float3 input)
{
    input = input / (input + float3(1.0, 1.0, 1.0)); 
    input = pow(abs(input), float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2)); 
    return input;
}

float3 sRGBToLinear(float3 input)
{
    input = log(input) / log(2.2);
    input = input / (float3(1.0, 1.0, 1.0) - input);
    return input;
}

/*
* FresnalSchlick is reflectance on the surface
* FresnelSchlickRoughness used for IBL 
*/

float3 FresnalSchlick(float costheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - costheta, 5.0);
}

float3 FresnelSchlickRoughness(float costheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness), F0) - F0) * pow(1.0 - costheta, 5.0);
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

float Pow5(float x)
{
    float xSquare = x * x;
    return xSquare * xSquare * x;
}

// calculate hhomegeneous coordinates, trans (x, y, z, w) to (x', y', z', 1.0)
float4 HomogeneousCoordinates(float4 position)
{
    return position / position.w;
}

float3 CalculateDirectionalLighting(SurfaceProperties surface, LightProperties light)
{
    float3 irradiance = float3(0.0, 0.0, 0.0);
    // calculate direct and indirect lighting
    for (int lightIdx = 0; lightIdx < light.LightNum; lightIdx++)
    {
        // direct lighting
        float4 Light_Position = HomogeneousCoordinates(light.LightPosition[lightIdx]);
        float4 World_Position = HomogeneousCoordinates(surface.PositionWorld);
        float3 Light_Vec = normalize(Light_Position.xyz - World_Position.xyz);
        float3 Half_Vec = normalize(Light_Vec + surface.View_Vec);
        float N_dot_L = dot(surface.Normal_Vec, Light_Vec);

        float distance = length(World_Position - Light_Position);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = light.LightColor[lightIdx] * attenuation;

        // indirect lighting, based on Cook-Torrence
        
        // calc metal albedo, interpolate by metallic to calculate albedo
        float3 F_Value = lerp(F0, surface.Albedo, surface.Metallic);
        float3 F = FresnalSchlick(surface.N_dot_V, F_Value);
        
        // calc Normal Distribution by GGX
        float3 D = DistributionGGX(surface.Normal_Vec, Half_Vec, surface.Roughness);
        
        // calc Geometry Distribution by Smith
        float3 G = GeometrySmith(surface.Normal_Vec, surface.View_Vec, Light_Vec, surface.Roughness);
        
        // calc specular FDG / 4(n * view)(n * light);
        float3 specular = F * D * G / max(0.001, 4 * saturate(surface.N_dot_V) * saturate(N_dot_L));

        // calc kd, F = kS
        float3 kD = (1 - F) * (1 - surface.Metallic);

        // calc diffuse
        float3 diffuse = kD * surface.Albedo / PI;
        
        irradiance += (diffuse + specular) * radiance * N_dot_L;
    }
    return irradiance;
}

#if 0

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

#endif