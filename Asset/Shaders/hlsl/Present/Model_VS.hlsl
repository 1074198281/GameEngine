#define MAX_LIGHT_NUM 16

cbuffer PerBatchConstants : register(b1)
{
    float4x4 gModelMatrix;
};

// cb2
cbuffer PerFrameConstants : register(b2)
{
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
    float4 gCameraPosition;
    int gLightNum;
    int clip_space_type; //0: opengl ,1: others ≤√ºÙø’º‰¿‡–Õ
    float2 padding2;
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
    float4x4 ProjectedMat = mul(transpose(gProjMatrix), transpose(gViewMatrix));
	
    vout.ProjectedPosition = mul(mul(mul(float4(vin.Position, 1.0f), gModelMatrix), transpose(gViewMatrix)), transpose(gProjMatrix));
	
    vout.WorldPosition = mul(float4(vin.Position, 1.0f), gModelMatrix);
    vout.WorldNormal = mul(float4(vin.Normal, 0.0f), gModelMatrix).xyz;
	vout.TextureUV = vin.Tex;
	return vout;
}
