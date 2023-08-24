#pragma once
#include <vector>
#include <math.h>
#include <unordered_map>
#include "d3dx12.h"
#include <DirectXMath.h>
#include <memory>
#include "Core/Resource/GpuBuffer.h"
#include "Core/Resource/Texture.h"
#include "Core/Pipeline/DescriptorHeap.h"


namespace My {
	typedef struct RenderObject {
		D3dGraphicsCore::StructuredBuffer VertexBuffer;
		D3dGraphicsCore::ByteAddressBuffer IndexBuffer;
		unsigned int indexCountPerInstance;
		unsigned int InstanceCount;

		unsigned int TextureCount;
		D3dGraphicsCore::DescriptorHandle FirstHandle;
		std::unordered_map<std::string, std::pair<D3dGraphicsCore::Texture, D3dGraphicsCore::DescriptorHandle> > TextureResource;
	} RenderObject;

	void InitializeSamplers();
	extern D3D12_SAMPLER_DESC g_PointWarpSampler;
	extern D3D12_SAMPLER_DESC g_PointClampSampler;
	extern D3D12_SAMPLER_DESC g_LinearWarpSampler;
	extern D3D12_SAMPLER_DESC g_LinearClampSampler;
	extern D3D12_SAMPLER_DESC g_AnisotropicWarpSampler;
	extern D3D12_SAMPLER_DESC g_AnisotropicClampSampler;
}
