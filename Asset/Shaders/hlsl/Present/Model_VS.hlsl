#include "../Algorithm/ShaderInput.hlsl"

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
