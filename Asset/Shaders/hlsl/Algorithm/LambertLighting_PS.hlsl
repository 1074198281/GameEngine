#include "ShaderInput.hlsl"

#define LAMBERT
#ifdef LAMBERT

struct LVertexOut
{
    float4 ProjectedPosition : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TextureUV : TEXCOORD;
    float4 Color : COLOR;
};

#endif

float4 lambertFlatShadingMain(LVertexOut pInput)
{
    return pInput.Color;
}