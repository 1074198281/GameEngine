
struct PresentOut
{
    float4 position : SV_POSITION;
    float2 texUV : TEX;
};

PresentOut main(uint VertID : SV_VertexID)
{
    float2 ScreenUV = float2(uint2(VertID, VertID << 1) & 2);
    float2 texUV = lerp(float2(-1, 1), float2(1, -1), ScreenUV);
    float4 ProjectedPos = float4(texUV, 0, 1);
    PresentOut Presentout;
    Presentout.position = ProjectedPos;
    Presentout.texUV = texUV;
    return Presentout;
}