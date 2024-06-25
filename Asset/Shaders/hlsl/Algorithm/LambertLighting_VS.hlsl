#include "ShaderInput.hlsl"

#define LAMBERT

/*
* lambert理想漫反射体：受光面表面的光照是均匀的，不受粗糙度的影响
* lambert模型假设理想粗糙表面的反射光是均匀分布在所有方向上的
* 在所有方向上观察的光照反射结果一样
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