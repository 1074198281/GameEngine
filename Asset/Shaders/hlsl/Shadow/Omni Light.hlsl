

cbuffer Omni_Light_Batch : register(b1)
{
    float4x4 gModelMatrix;
};

cbuffer Omni_Light_Frame : register(b2)
{
    float4x4 gViewMatrix;
    float4x4 gProjMatrix;
    float4 gLightPos;
    
};

void mainVS()
{
    
}