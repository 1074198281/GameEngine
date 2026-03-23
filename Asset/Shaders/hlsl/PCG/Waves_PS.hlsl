struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
};

float4 main(VertexOut pin) : SV_Target
{
    float4 vColor;
    vColor = float4(0, 0, 1, 1);
    
    
    return vColor;
}