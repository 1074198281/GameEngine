#ifndef __SHADERBASE_H__
#define __SHADERBASE_H__

#ifndef __cplusplus

#ifdef __PSSL__
#pragma warning (disable: 5203 5206 8207 5559)
#endif

#define Vector2f float2
#define Vector3f float3
#define Vector4f float4

uint FIRSTBITLOW_SLOW(uint input)
{
	for (int bit = 0; bit<32; ++bit)
		if (input & (1 << bit))
			return bit;
	return 32;
}

uint FIRSTBITHIGH_SLOW(uint input)
{
	for (int bit = 31; bit >= 0; --bit)
		if (input & (1 << bit))
			return bit;
	return 32;
}

#define __CBREGISTER( i )		 : register( b ## i )

#ifdef __PSSL__
#define Matrix4Unaligned column_major matrix
#define unistruct	     ConstantBuffer
#else
#define Matrix4Unaligned matrix
#define unistruct	     cbuffer
#endif 

#else
#include <stddef.h>
#include "geommath.hpp"

#define __CBREGISTER( i )
#define ATTR_OFFS			offsetof

#define unistruct	struct


namespace My {
	enum LightType { Omni = 0, Spot = 1, Infinity = 2, Area = 3 };
#define MAX_LIGHT_NUM 16

	typedef size_t TextureHandle;
	typedef struct Texture {
		TextureHandle Handle;
	} Texture;

	typedef struct MaterialTextures {
		Texture DiffuseMap;
		Texture MetallicRoughnessMap;
		Texture AmbientOcclusionMap;
		Texture EmissiveMap;
		Texture NormalMap;
	} MaterialTextures;

	typedef struct GlobalTextures {
		Texture SkyBoxMap;
		Texture SkyBoxDiffuseMap;
		Texture BRDF_LUT_Map;
	} GlobalTextures;

}


#endif

#endif