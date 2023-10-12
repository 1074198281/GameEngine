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

		//�������������
		D3dGraphicsCore::StructuredBuffer VertexBuffer;		//���㻺��
		D3dGraphicsCore::ByteAddressBuffer IndexBuffer;		//��������
		unsigned int indexCountPerInstance;					//ÿ��ʵ����������
		unsigned int InstanceCount;							//ʵ��������

		//��Ⱦ����״̬
		D3dGraphicsCore::RootSignature m_RootSignature;
		D3dGraphicsCore::GraphicsPSO m_PSO;
		int m_InputLayoutType = 0;

		//��������
		unsigned int m_TextureCount;
		D3dGraphicsCore::DescriptorHandle m_FirstHandle;
		std::unordered_map<std::string, TextureResource> m_TextureResource;
		bool m_alphaStatus;


	};

}