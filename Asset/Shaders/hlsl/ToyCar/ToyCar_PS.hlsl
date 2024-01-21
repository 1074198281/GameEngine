#include "../Algorithm/Lighting.hlsl"

Texture2D<float4> tex0[5] : register(t0);
SamplerState sam : register(s10);

cbuffer cbMaterialConstants : register(b0)
{
    float4 gEyePos;
    float4 gLightDir[MAX_LIGHT_NUM];
    int gLightNum;
};

cbuffer cbCommon : register(b1)
{
    float4x4 gModelMatrix;
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

float4 main(VertexOut pin) : SV_Target
{
    float LumenIns = 1.0;
    float4 LightColor = float4(1.0, 1.0, 1.0, 1.0);

    float4 WorldPos = pin.PosL.xyzw / pin.PosL.w;
    float3 defaultLightPos = gLightDir[0].xyz;
    float3 LightDir = normalize((defaultLightPos - WorldPos.xyz));
    
    float3 NormalDir = normalize(mul(float4(pin.Normal, 0.0f), transpose(gModelMatrix)).xyz);

#ifdef LAMBERT
    return LambertLighting(LumenIns, LightColor, LightDir, NormalDir);
#endif
#ifdef PBR
    MaterialProperties mat;
    mat.Albedo =  tex0[0].Sample(sam, pin.Tex).rgb;
    mat.metallic = tex0[1].Sample(sam, pin.Tex).b;
    mat.roughness = tex0[1].Sample(sam, pin.Tex).g;
    mat.AO = tex0[1].Sample(sam, pin.Tex).r;
    InputLayout input;
    input.CameraPos = gEyePos.xyz;
    input.WorldPos = WorldPos.xyz;
    input.Normal = NormalDir;
    input.TexCoord = pin.Tex;
    return PBROutput(input, mat, gLightDir, gLightNum);
#endif
    
}
