#pragma once
#include "StructureSettings.h"
#include "SceneObject.hpp"
#include "Core/Pipeline/RootSignature.h"
#include "Core/Pipeline/PipelineState.h"
#include "Core/Resource/Texture.h" 

namespace D3dGraphicsCore {
	/*
	* -----basic information-----
	* ����/����������
	* �����洢������Դ���е�ƫ��
	* ʵ��������/��������
	* alphamode ͸�� ģ��
	* ��ǰ�������ʹ���˼�������
	* 
	* ----RootSignature-----
	* ������
	* ��̬������
	* 
	* -----GraphicsPSO-----
	* inputlayoutType
	* ����ͼԪ����(PrimitiveType)
	* shaderByteCode
	
	*/

	typedef struct TextureResource {
		Texture* pTexture;
		DescriptorHandle Handle;
	} TextureResource;

	typedef struct MaterialResource {
		std::vector<TextureResource> TextureResources;
		int TextureCountPerMaterial;
		DescriptorHandle FirstHandle;
		int DescriptorHeapIndex;
		GraphicsPSO PSO;
	} MaterialResource;

	class PrimitiveObject
	{
	public:
		PrimitiveObject() {}
		~PrimitiveObject() {}

	public:
		std::string name;
		
		D3dGraphicsCore::StructuredBuffer VertexBuffer;
		D3dGraphicsCore::ByteAddressBuffer IndexBuffer;
		UINT indexCountPerInstance;
		UINT InstanceCount;
		bool alphamode;

		// TextureResources
		MaterialResource MaterialResource;

		//GraphicsPSO
		int InputLayoutType;
		My::PrimitiveType PrimitiveType;
	};
}