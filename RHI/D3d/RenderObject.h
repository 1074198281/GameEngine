#pragma once
#include "StructureSettings.h"
#include "SceneObject.hpp"
#include "Core/Pipeline/RootSignature.h"
#include "Core/Pipeline/PipelineState.h"
#include "Core/Resource/Texture.h" 

namespace D3dGraphicsCore {
	/*
	* -----basic information-----
	* 顶点/索引缓冲区
	* 纹理存储，在资源堆中的偏移
	* 实例化数量/顶点数量
	* alphamode 透明 模板
	* 当前对象组件使用了几个纹理
	* 
	* ----RootSignature-----
	* 根参数
	* 静态采样器
	* 
	* -----GraphicsPSO-----
	* inputlayoutType
	* 几何图元类型(PrimitiveType)
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