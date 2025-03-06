static const float3 F0 = float3(0.04, 0.04, 0.04);
static const float PI = 3.14159265358979;
#define MAX_LIGHT_NUM 16

float Pow5(float x)
{
    float xSquare = x * x;
    return xSquare * xSquare * x;
}

// calculate hhomegeneous coordinates, trans (x, y, z, w) to (x', y', z', 1.0)
float4 HomogeneousCoordinates(float4 position)
{
    return float4(position.xyz / position.w, 1.0f);
}

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