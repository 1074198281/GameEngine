#include "ShaderInput.hlsl"

#define LAMBERT

/*
* lambert�����������壺�ܹ������Ĺ����Ǿ��ȵģ����ֲܴڶȵ�Ӱ��
* lambertģ�ͼ�������ֲڱ���ķ�����Ǿ��ȷֲ������з����ϵ�
* �����з����Ϲ۲�Ĺ��շ�����һ��
*/

#ifdef LAMBERT

struct LVertexIn
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

struct LVertexOut
{
    float4 ProjectedPosition : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TextureUV : TEXCOORD;
    float4 Color : COLOR;
};

float4 LambertLighting(float Insensity, float4 LightColor, float3 LightDir, float3 NormalDir)
{
    return float4(Insensity * LightColor.xyz * max(0.0f, dot(LightDir, NormalDir)), 1.0f);
}
#endif


//------------------------------ flat shading ------------------------------//
LVertexOut lambertFlatShadingMain(LVertexIn vInput)
{
    LVertexOut vOutput;
    
    vOutput.ProjectedPosition = mul(mul(mul(float4(vInput.Position, 1.0f), gModelMatrix), transpose(gViewMatrix)), transpose(gProjMatrix));
    vOutput.WorldPosition = mul(float4(vInput.Position, 1.0f), gModelMatrix);
    vOutput.TextureUV = vInput.Tex;
    vOutput.WorldNormal = mul(float4(vInput.Normal, 1.0f), gModelMatrix);
    
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int idx = 0; idx < gLightNum; idx++)
    {
        color += LambertLighting(glightinfo[idx].gInsensity, glightinfo[idx].gLightColor, 
        (glightinfo[idx].gLightPosition - HomogeneousCoordinates(vOutput.WorldPosition)).xyz, vOutput.WorldNormal);
    }
    
    vOutput.Color = color;
    
    return vOutput;
}