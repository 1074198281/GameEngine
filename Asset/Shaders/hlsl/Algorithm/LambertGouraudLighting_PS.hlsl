
struct LVertexOut
{
    float4 ProjectedPosition : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TextureUV : TEXCOORD;
    float4 Color : COLOR;
};

float4 lambertGouraudShadingMain(LVertexOut pInput) : SV_Target
{
    return pInput.Color;
}