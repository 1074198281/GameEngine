Texture2D<float3> InputBuf : register(t0);
RWTexture2D<float3> Result : register(u0);

static const float Weights[3] = { 1.0f / 16.0f, 1.0f / 4.0f, 3.0f / 8.0f };

groupshared float3 gCache[9];

[numthreads(1, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID, uint3 DTid : SV_DispatchThreadID)
{
    for (int i = 0; i < 3; i++)
    {
        gCache[0 + i * 3] = InputBuf[uint2(DTid.x - 1, DTid.y + i - 1)];
        gCache[1 + i * 3] = InputBuf[uint2(DTid.x, DTid.y + i - 1)];
        gCache[2 + i * 3] = InputBuf[uint2(DTid.x + 1, DTid.y + i - 1)];
    }
    
    
    Result[uint2(DTid.x, DTid.y)] = (gCache[0] + gCache[2] + gCache[6] + gCache[8]) * Weights[0] +
                            (gCache[1] + gCache[3] + gCache[5] + gCache[7]) * Weights[1] +
                            (gCache[4]) * Weights[2];

}