// KHR_texture_transform
// input: TextureUV float2 offset float2 scale float rotation output: transformedUV 
// 
#define TEXTURE_TRANSFORM_COUNT 5

float2 getTransformedUV(float2 offset, float2 scale, float rotation, float2 TextureUV)
{
    float3x3 MatTranslation = float3x3(1, 0, 0, 0, 1, 0, offset.x, offset.y, 1);
    float3x3 MatRotation = float3x3(cos(rotation), sin(rotation), 0,
                                -sin(rotation), cos(rotation), 0,
                                0, 0, 1);
    float3x3 MatScale = float3x3(scale.x, 0, 0, 0, scale.y, 0, 0, 0, 1);
    float3x3 mat = mul(mul(MatTranslation, MatRotation), MatScale);
    float2 transformedUV = mul(float3(TextureUV, 1), mat).xy;
    return transformedUV;
}