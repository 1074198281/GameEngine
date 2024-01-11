#define MAX_LIGHT_NUM 16

float4 LambertLighting(float Insensity, float4 LightColor, float3 LightDir, float3 NormalDir)
{
    return float4(Insensity * LightColor.xyz * max(0.0f, dot(LightDir, NormalDir)), 1.0f);
}


float4 PBR(float3 LightDir, float3 NormalDir)
{
    float4 ret;
    //calculator half vec
    float3 halfvec= normalize(LightDir + NormalDir);
    
    
}