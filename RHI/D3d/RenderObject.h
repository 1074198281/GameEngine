#pragma once
#include "StructureSettings.h"
#include "SceneObject.hpp"
#include "Core/Pipeline/RootSignature.h"
#include "Core/Pipeline/PipelineState.h"
#include "Core/Resource/Texture.h" 
#include "Core/Resource/GpuBuffer.h"
#include <DirectXMath.h>

namespace D3dGraphicsCore {
	/*
	* -----basic information-----
	* ����/����������
	* ����洢������Դ���е�ƫ��
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

	//typedef struct TextureResource {
	//	GpuTexture* pTexture;
	//	DescriptorHandle Handle;
	//	void* pImageData;
	//} TextureResource;

	//typedef struct MaterialResource {
	//	std::vector<TextureResource> TextureResources;
	//	int TextureCountPerMaterial;
	//	DescriptorHandle FirstHandle;
	//	int DescriptorHeapIndex;
	//	float BaseColorFactor[4];
	//	float MetallicRoughnessFactor[2];
	//	float EmissiveFactor[3];
	//	float NormalScaleFactor;
	//	float BaseColorTextureTransform[5];
	//	float MetallicRoughnessTextureTransform[5];
	//	float OcclusionTransform[5];
	//	float EmissiveTextureTransform[5];
	//	float NormalTextureTransform[5];
	//	GraphicsPSO PSO;
	//} MaterialResource;

	//typedef struct LightResource {
	//	DirectX::XMFLOAT4 LightColor;
	//} LightResource;



	//class PrimitiveObject
	//{
	//public:
	//	PrimitiveObject() {}
	//	~PrimitiveObject() {}

	//public:
	//	std::string name;
	//	
	//	D3dGraphicsCore::StructuredBuffer VertexBuffer;
	//	D3dGraphicsCore::ByteAddressBuffer IndexBuffer;
	//	UINT indexCountPerInstance;
	//	UINT InstanceCount;
	//	bool alphamode;
	//	DirectX::XMFLOAT4X4* transform;

	//	// TextureResources
	//	std::string MaterialName;
	//	MaterialResource MaterialResource;

	//	//GraphicsPSO
	//	int InputLayoutType;
	//	My::PrimitiveType PrimitiveType;
	//};
}