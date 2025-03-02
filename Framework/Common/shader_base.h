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
#include "Image.hpp"

#define __CBREGISTER( i )
#define ATTR_OFFS			offsetof

#define unistruct	struct


namespace My {
	enum LightType { Omni = 0, Spot = 1, Infinity = 2, Area = 3, None = 4 };
#define MAX_LIGHT_NUM 100

#ifdef USING_DX12
	typedef size_t TextureHandle;
#elif USING_OPENGL
	typedef size_t TextureHandle;
#elif USING_METAL

#elif USING_OPENGLES
	typedef size_t TextureHandle;
#elif USING_VULKAN

#endif // GraphicsInterface

	typedef struct TextureBase {
		TextureHandle handle;
		PIXEL_FORMAT pixel_format;

		uint32_t width;
		uint32_t height;
		uint32_t mips;
		uint32_t samplers;
	} TextureBase;

	struct TextureArrayBase : virtual TextureBase {
		uint32_t size = 0;
	};

	struct Texture2D : virtual TextureBase {
	};

	struct TextureCube : virtual TextureBase {
	};

	struct Texture2DArray : Texture2D, TextureArrayBase {
	};

	struct TextureCubeArray : TextureCube, TextureArrayBase {
	};

	typedef struct MaterialTextures {
		Texture2D DiffuseMap;
		Texture2D MetallicRoughnessMap;
		Texture2D AmbientOcclusionMap;
		Texture2D EmissiveMap;
		Texture2D NormalMap;
	} MaterialTextures;

	typedef struct GlobalTextures {
		TextureCube SkyBoxMap;
		TextureCube SkyBoxDiffuseMap;
		Texture2D BRDF_LUT_Map;
	} GlobalTextures;

	typedef struct ShadowTextures {
		Texture2DArray ShadowMap;
		Texture2DArray GlobalShadowMap;
		TextureCubeArray CubeShadowMap;
	} ShadowTextures;
}


#endif

#endif