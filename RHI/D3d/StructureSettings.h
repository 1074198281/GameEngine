#pragma once
#include "Core/Resource/Texture.h"
#include "Core/Pipeline/DescriptorHeap.h"
#include <vector>
#include <math.h>
#include <unordered_map>
#include <memory>
#include "d3dx12.h"
#include <DirectXMath.h>


namespace D3dGraphicsCore {
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
        kMeshConstant,
        kMaterialConstant,
        kMaterialSRVs,
        kMaterialSamplers,
        kCommonSRVs,
        kCommonCBV,
		//kSkinMatrices,

        kNumRootBindings
	};

	typedef struct DXSampler
	{
		D3D12_FILTER filter;
		D3D12_TEXTURE_ADDRESS_MODE wrapS;
		D3D12_TEXTURE_ADDRESS_MODE wrapT;
	} DXSampler;

	typedef struct IBLImageMap
	{
		std::unique_ptr<Texture> pSpecular;
		std::unique_ptr<Texture> pDiffuse;
	} IBLImageMap;

	typedef struct IBLImageResource
	{
		DescriptorHandle FirstHandle;
		float SpecularIBLRange;
		float SpecularIBLBias;
		int IBLImageCount = 0;
		int HeapIndex = -1;
		std::unordered_map<std::string, std::unique_ptr<IBLImageMap> > IBLImages;

		DescriptorHandle BRDF_LUT_Handle;
		std::unique_ptr<Texture> BRDF_LUT_Image;
		int BRDFHeapIndex;
		int BRDF_offset;
	} IBLImageResource;
}
