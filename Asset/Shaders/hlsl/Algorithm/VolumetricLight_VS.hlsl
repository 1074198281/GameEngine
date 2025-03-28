cbuffer VolumetricLightConstants : register(b1)
{
    float4x4 gInvViewProj;
    float4 gCameraPos;
    float gScreenWidth;
    float gScreenHeight;
    float gMarchingStep;
    float gSampleIntensity;
    float gCameraNearZ;
    float gCameraFarZ;
    float2 padding0;
};

struct VolumetricLightVSOut
{
    float4 position : SV_POSITION;
    float2 texUV : TEX;
};

VolumetricLightVSOut main(uint VertID : SV_VertexID)
{
    float2 ScreenUV = float2(uint2(VertID, VertID << 1) & 2);
    float2 texUV = lerp(float2(-1, 1), float2(1, -1), ScreenUV);
    VolumetricLightVSOut volumetricLightVSOut;
    volumetricLightVSOut.position = float4(texUV, 0, 1);
    volumetricLightVSOut.texUV = ScreenUV;
    return volumetricLightVSOut;
}