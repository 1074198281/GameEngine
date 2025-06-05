#pragma once
#include "Core/Resource/Texture.h"
#include "Core/Pipeline/DescriptorHeap.h"
#include "Core/Resource/ColorBuffer.h"
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
	* kMeshConstant,kMaterialConstant,kCommonCBV是设置常量缓冲区的，
	* 可以通过SetGraphicsRootConstantBufferView接口来设置
	* kMaterialSRVs,kMaterialSamplers,kCommonSRVs设置的是一片区域的数据，
	* 可以通过SetDescriptorTable设置，通常是设置一个描述符表
	* 
	* kMeshConstant 矩阵常量
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
		kShadowSRVs,

        kNumRootBindings
	};

	enum ComputeRootBindings {
		kComputeSRV,
		kComputeUAV,
		kComputeCBV,

		kComputeRootBindings
	};

	enum WaterDropsRootBindings {
		kWaterDropsSRV,
		kWaterDropsCBV,

		kWaterDropsRootBindings
	};

	enum SeaRootBindings {
		kSeaSRV,
		kSeaCBV,

		kSeaRootBindings
	};

	enum VolumetricLightRootBindings {
		kCameraDepthSRV,
		kLightDepthSRV,
		kPresentSRV,
		kLightCBV,
		kVolumnCBV,

		kVolumetricLightRootBindings
	};

	enum ShadowRootSignature {
		kShadowBatchCBV,
		kShadowFrameCBV,
		kShadowSRV,

		kShadowRootBindings
	};

	typedef struct DXSampler
	{
		D3D12_FILTER filter;
		D3D12_TEXTURE_ADDRESS_MODE wrapS;
		D3D12_TEXTURE_ADDRESS_MODE wrapT;
	} DXSampler;

	typedef struct SResourceBase {
		std::string name;
	} SResource;

	typedef struct SResourceDescriptorInfo {
		// descriptor heap and handle info
		int iHeapIndex{ -1 };
		D3dGraphicsCore::DescriptorHandle handle;
	} SResourceDescriptorInfo;

	typedef struct SResourceTextureInfo {
		std::string texture_name;
		uint32_t iSize;
		uint32_t iWidth;
		uint32_t iHeight;
		uint32_t iPitchOrLinearSize;
		uint32_t iDepth;
		uint32_t iMipMapCount;
		std::shared_ptr<D3dGraphicsCore::GpuTexture> pTexture;
	} SResourceTextureInfo;

	typedef struct SResourceBatchInfo
		: public SResourceBase,
		public SResourceDescriptorInfo
	{
		uint32_t batch_index{ 0xFFFFFFFF };
		uint32_t texture_info_flag{ 0 };
		std::vector<std::shared_ptr<SResourceTextureInfo>> texture_info;
	} SResourceBatchInfo;

	typedef struct SResourceIBLImageMap : public SResourceDescriptorInfo
	{
		std::string name;
		std::unique_ptr<SResourceTextureInfo> pSpecular;
		std::unique_ptr<SResourceTextureInfo> pDiffuse;
	} SResourceIBLImageMap;

	typedef struct SResourceIBLImage
	{
		std::unordered_map<int, std::unique_ptr<SResourceIBLImageMap> > IBLImages;
		std::unique_ptr<D3dGraphicsCore::GpuTexture> BRDF_LUT_Image;
		float SpecularIBLRange;
		float SpecularIBLBias;
		int IBLImageCount = 0;
	} SResourceIBLImage;

	typedef struct SResourceColorBufferInfo
		: public SResourceBase,
		public SResourceDescriptorInfo
	{
		std::shared_ptr<D3dGraphicsCore::ColorBuffer> pGpuBuffer;
	} SResourceColorBufferInfo;

}
