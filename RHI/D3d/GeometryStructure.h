#pragma once
#include <vector>
#include <math.h>
#include <unordered_map>
#include "d3dx12.h"
#include <DirectXMath.h>
#include "Core/Resource/GpuBuffer.h"
#include "Core/Resource/Texture.h"


namespace My {
	typedef struct RenderObject {
		D3dGraphicsCore::StructuredBuffer VertexBuffer;
		D3dGraphicsCore::ByteAddressBuffer IndexBuffer;
		UINT indexCountPerInstance;
		UINT InstanceCount;

		std::unordered_map<std::string, D3dGraphicsCore::Texture> TextureResource;
	} RenderObject;
}
