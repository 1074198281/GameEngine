#define MAX_LIGHT_NUM 16

cbuffer cbCommon : register(b1)
{
    float4 gLightPosition[MAX_LIGHT_NUM];
    float4 gLightColor[MAX_LIGHT_NUM];
    float4x4 gModelMatrix;
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
    float4 gViewerPos;
    float3 SunDirection;
    float3 SunInsensity;
    int gLightNum;
    float IBLRange;
    float IBLBias;
};

struct VertexIn
{
	float3 Position  : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
    float4 ProjectedPosition : SV_POSITION;
    float4 WorldPosition : POSITION;
    float3 WorldNormal : NORMAL;
    float2 TextureUV : TEXCOORD;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    float4x4 ProjectedMat = mul(transpose(gModelMatrix), mul(transpose(gViewMatrix), transpose(gProjMatrix)));
	
    vout.ProjectedPosition = mul(float4(vin.Position, 1.0f), ProjectedMat);
	
    vout.WorldPosition = mul(float4(vin.Position, 1.0f), transpose(gModelMatrix));
	vout.WorldNormal = mul(float4(vin.Normal, 0.0f), transpose(gModelMatrix)).xyz;
	vout.TextureUV = vin.Tex;
	return vout;
}