#pragma once
#include <map>
#include <unordered_map>
#include "Core/Resource/GpuBuffer.h"
#include "Core/Resource/Texture.h"
#include "Core/Pipeline/PipelineState.h"
#include "Core/Pipeline/RootSignature.h"
#include "Core/Pipeline/DescriptorHeap.h"



namespace My {

	typedef struct TextureResource {
		std::string TextureName;
		D3dGraphicsCore::Texture Texture;
		D3dGraphicsCore::DescriptorHandle Handle;
	} TextureResource;

	class RenderObject {
	public:
		enum { kPosition, kNormal, kTangent, kTexcoord0, kTexcoord1, kInputLayout };
	public:
		RenderObject() {} 
		~RenderObject() {} 

    public:
        void InitializeConstantBuffer();
        void InitializeSRV();
		void Finalize();
		void SetName(std::string name) { m_name = name; }
	public:
		std::string m_name;

		//顶点和索引数据
		D3dGraphicsCore::StructuredBuffer VertexBuffer;		//顶点缓冲
		D3dGraphicsCore::ByteAddressBuffer IndexBuffer;		//索引缓冲
		unsigned int indexCountPerInstance;					//每个实例索引数量
		unsigned int InstanceCount;							//实例化数量

		//渲染管线状态
		D3dGraphicsCore::RootSignature m_RootSignature;
		D3dGraphicsCore::GraphicsPSO m_PSO;
		int m_InputLayoutType = 0;

		//纹理数据
		unsigned int m_TextureCount;
		D3dGraphicsCore::DescriptorHandle m_FirstHandle;
		std::unordered_map<std::string, TextureResource> m_TextureResource;
		bool m_alphaStatus;


	};

}