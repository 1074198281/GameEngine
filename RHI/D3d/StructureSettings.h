#pragma once
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




}
