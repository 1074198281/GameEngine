#pragma once
#include "Core/Resource/Texture.h"
#include "Core/Pipeline/DescriptorHeap.h"
#include "FrameStructure.hpp"
#include <vector>
#include <math.h>
#include <unordered_map>
#include <memory>
#include "d3dx12.h"
#include <DirectXMath.h>

namespace My {

	struct D3dDrawBatchContext : DrawBatchContext {
		uint32_t m_vertex_buffer_index;
		uint32_t m_indice_buffer_index;
		PrimitiveType m_PrimitiveType;
		uint32_t m_index_count_per_instance;
		uint32_t m_inputlayout;
	};

	enum kInputLayoutType {
		kPos,				// 1
		kNormal,			// 2
		kTangent,			// 4
		kTexcoord0,			// 8
		kTexcoord1,			// 16
		
		kInputLayoutType
	};

	/*
	* kMeshConstant,kMaterialConstant,kCommonCBV�����ó����������ģ�
	* ����ͨ��SetGraphicsRootConstantBufferView�ӿ�������
	* kMaterialSRVs,kMaterialSamplers,kCommonSRVs���õ���һƬ��������ݣ�
	* ����ͨ��SetDescriptorTable���ã�ͨ��������һ����������
	* 
	* kMeshConstant ������
	* 
	*/
	enum RootBindings { 
        kMeshConstants,
        kMaterialConstants,
        kMaterialSRVs,
        kMaterialSamplers,
        kCommonBatchConstantsCBV,
		kCommonFrameConstantsCBV,
		kCommonLightConstantsCBV,
		kCommonSRVs,

        kNumRootBindings
	};

	enum ComputeRootBindings {
		kComputeSRV,
		kComputeUAV,
		kComputeCBV,

		kComputeRootBindings
	};

	typedef struct DXSampler
	{
		D3D12_FILTER filter;
		D3D12_TEXTURE_ADDRESS_MODE wrapS;
		D3D12_TEXTURE_ADDRESS_MODE wrapT;
	} DXSampler;

	typedef struct IBLImageMap
	{
		std::string name;
		std::unique_ptr<D3dGraphicsCore::GpuTexture> pSpecular;
		std::unique_ptr<D3dGraphicsCore::GpuTexture> pDiffuse;
	} IBLImageMap;

	typedef struct IBLImageResource
	{
		std::unordered_map<int, std::unique_ptr<IBLImageMap> > IBLImages;
		std::unique_ptr<D3dGraphicsCore::GpuTexture> BRDF_LUT_Image;
		float SpecularIBLRange;
		float SpecularIBLBias;
		int IBLImageCount = 0;
	} IBLImageResource;

	typedef struct DescriptorHeapHandleInfo {
		int HeapIndex{-1};
		D3dGraphicsCore::DescriptorHandle Handle;
	} DescriptorHeapHandleInfo;
}
