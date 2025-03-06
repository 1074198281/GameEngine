#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "LightManager.h"
#include "Core/Resource/DepthBuffer.h"
#include "Core/Resource/ColorBuffer.h"
#include "Core/Pipeline/DescriptorHeap.h"

namespace My {

	typedef struct DepthResource {
		std::shared_ptr<D3dGraphicsCore::DepthBuffer> pDepthBuffer;
		std::shared_ptr<D3dGraphicsCore::ColorBuffer> pColorBuffer;	//colorbuffer for gui show
		D3dGraphicsCore::DescriptorHandle colorBufferSRVHandle;
		D3dGraphicsCore::DescriptorHandle depthBufferSRVHandle;
		int heapIndex;
	} DepthResource;

	class D3d12LightManager : public LightManager 
	{
	public:
		D3d12LightManager();
		D3d12LightManager(BaseApplication* pApp);
		virtual ~D3d12LightManager();

		void Clear() override;
		void InitHandle() override;
		void Create() override;
	public:
		uint64_t GetColorGpuHandle() override;
		uint64_t GetDepthGpuHandle() override;
	public:
		std::shared_ptr<D3dGraphicsCore::DepthBuffer> GetDepthBuffer(uint8_t idx);
		std::shared_ptr<D3dGraphicsCore::ColorBuffer> GetColorBuffer(uint8_t idx);
		D3dGraphicsCore::DescriptorHandle GetDepthSRVDescriptorHandle(uint8_t idx);
		size_t GetShadowMapHandle(uint8_t idx) override;

	private:
		std::shared_ptr<D3dGraphicsCore::DepthBuffer> CreateDepthBuffer(uint8_t idx);


	protected:
		D3dGraphicsCore::DescriptorHandle m_ColorBufferFirstHandle;
		D3dGraphicsCore::DescriptorHandle m_ColorBufferCurrHandle;
		D3dGraphicsCore::DescriptorHandle m_DepthBufferFirstHandle;
		D3dGraphicsCore::DescriptorHandle m_DepthBufferCurrHandle;
		int m_DescriptorOffset;
		int m_iHeapIdx;
		std::unordered_map<uint8_t, DepthResource> m_ShadowMapTexture;
		std::unordered_map<uint8_t, DepthResource> m_GlobalShadowMapTexture;
		std::unordered_map<uint8_t, DepthResource> m_CubeShadowMapTexture;
	};
}