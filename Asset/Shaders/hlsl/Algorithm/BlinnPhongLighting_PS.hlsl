#include "CommonMath.hlsl"
/*
* phongģ�Ͱ����˻����⣬���淴���������������
* �����ⲿ����Ϊ�Ǿ��ȹ̶��ġ�
* ���淴�䲿�ֱ������߷���ͷ���ⷽ��Խ�ӽ�������ǿ��Խ��
* �����䲿�ֲ���lambert������ģ��
*/

cbuffer cbPerBatchConstants : register(b1)
{
    float4x4 gModelMatrix;
};

// cb2
cbuffer cbPerFrameConstants : register(b2)
{
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
    float4 gCameraPosition;
    int gLightNum;
    int clip_space_type; //0: opengl ,1: others �ü��ռ�����
    float2 padding2;
};

// cb3
struct cLight
{
    float4x4 gLightViewMatrix;
    float4x4 gLightProjectMatrix;
    float4 gLightPosition;
    float4 gLightColor;
    float4 gLightDirection;
    int gShadowMapIndex;
    int gLightType;
    float gInsensity;
    float padding0;
};

cbuffer LightInfo : register(b3)
{
    cLight glightinfo[MAX_LIGHT_NUM];
}

struct LVertexOut
{
    float4 ProjectedPosition : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TextureUV : TEXCOORD;
};

float4 blinnPhongLighting(LVertexOut pInput) : SV_Target
{
    float4 pixelColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    int i = 0;
    float insensity;
    
    // ambient
    float4 ambientColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 ka = float3(1.0f, 1.0f, 1.0f);
    for (i = 0; i < gLightNum; i++)
    {
        insensity = glightinfo[i].gInsensity / length(HomogeneousCoordinates(glightinfo[i].gLightPosition).xyz - HomogeneousCoordinates(pInput.WorldPosition).xyz);
        ambientColor.rgb += ka * glightinfo[i].gLightColor.rgb * insensity;
    }
    
    // specular
    float4 specularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float3 viewDir = gCameraPosition.xyz - pInput.WorldPosition.xyz;
    float3 lightDir;
    float3 reflectDir;
    for (i = 0; i < gLightNum; i++)
    {
        lightDir = glightinfo[i].gLightPosition.xyz - pInput.WorldPosition.xyz;
        reflectDir = reflect(lightDir, pInput.WorldNormal);
        insensity = glightinfo[i].gInsensity / length(HomogeneousCoordinates(glightinfo[i].gLightPosition).xyz - HomogeneousCoordinates(pInput.WorldPosition).xyz);
        specularColor.rgb += glightinfo[i].gLightColor.rgb * Pow5(max(dot(normalize(lightDir), normalize(reflectDir)), 0)) * insensity;
    }
    
    // diffuse
    float4 diffuseColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    for (i = 0; i < gLightNum; i++)
    {
        lightDir = glightinfo[i].gLightPosition.xyz - pInput.WorldPosition.xyz;
        float N_dot_L = dot(normalize(lightDir), normalize(pInput.WorldNormal));
        insensity = glightinfo[i].gInsensity / length(HomogeneousCoordinates(glightinfo[i].gLightPosition).xyz - HomogeneousCoordinates(pInput.WorldPosition).xyz);
        diffuseColor.rgb += glightinfo[i].gLightColor.rgb * max(0, N_dot_L) * insensity;
    }

    pixelColor = diffuseColor + specularColor + ambientColor;
    return pixelColor;
}