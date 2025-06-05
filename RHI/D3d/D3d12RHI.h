#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <functional>
#include <DirectXMath.h>
#include "GfxConfiguration.h"
#include "BaseApplication.hpp"
#include "StructureSettings.h"
#include "Modules/ShaderSource/ShaderSource.h"
#include "Modules/LightManager/D3d12LightManager.h"
#include "GraphicsStructure.h"

#include "D3dComponents/XMCamera/XMCamera.h"
#include "D3dComponents/XMCamera/XMCameraController.h"


namespace D3dGraphicsCore {
	class StructuredBuffer;
	class ByteAddressBuffer;
	class GraphicsContext;

	class D3d12RHI {
		using QueryFrameBufferSize = std::function<void(uint32_t&, uint32_t&)>;
		using GetWindowHandleProc = std::function<HWND()>;
		using GetGfxconfiguration = std::function<const My::GfxConfiguration&()>;
		using GetApplication = std::function<My::IApplication* ()>;
		using GetTimestamp = std::function<uint32_t()>;

	public:
		D3d12RHI();
		~D3d12RHI();

		int StartUp();
		void Finalize();

		void Resize(uint32_t width, uint32_t height);

		void SetQueryFrameBufferSize(const QueryFrameBufferSize& func) {
			m_fQueryFrameBufferSize = func;
		}

		void SetGetWindowHandleProc(const GetWindowHandleProc& func) {
			m_fGetWindowHandleProc = func;
		}

		void SetGetGfxconfiguration(const GetGfxconfiguration& func) {
			m_fGetGfxconfiguration = func;
		}

		void SetGetApplication(const GetApplication& func)
		{
			m_fGetApplication = func;
		}

		void SetGetTimestamp(const GetTimestamp& func)
		{
			m_fGetTimestamp = func;
		}

	public:
		void InitializeGraphicsSettings();
		void FinalizeGraphicsSettings();
		void SetLightManager(My::D3d12LightManager* pLightManager);
	public:
		void UpdateCamera();
		void UpdateCameraParams(int64_t key);
		void UpdatePresent();

		void UpdateCameraConstants(My::Frame& frame);

	public:
		void DrawBatch(const My::Frame& frame, const My::D3dDrawBatchContext* pdbc, StructuredBuffer* vbuffer, ByteAddressBuffer* ibuffer,
			const int TextureHeapIndex, const DescriptorHandle& TextureHandle, ID3D12DescriptorHeap* IBLHeapPtr, DescriptorHandle IBLHandle, 
			std::unique_ptr<My::D3d12LightManager>& pLightManager, uint8_t lightIdx = -1, bool bShadowCast = false, bool isDrawSkybox = false);
		void DrawSkybox(const My::Frame& frame, ID3D12DescriptorHeap* HeapPtr, DescriptorHandle IBLHandle, GpuTexture* pSpecularTexture, float& SpecularIBLRange, float& SpecularIBLBias);
		void DrawGui(const My::Frame& frame);
		void DrawPresent(const My::Frame& frame, DescriptorHandle ColorBufferHandle, int ColorBufferHeapIndex);
		void DrawGaussBlur(const My::Frame& frame, ColorBuffer& result, ColorBuffer& src, DescriptorHandle ResultBufferHandle, DescriptorHandle ColorBufferHandle, int ColorBufferHeapIndex);
		void DrawWaterDrops(const My::Frame& frame, ColorBuffer& result, ColorBuffer& src, DescriptorHandle ResultBufferHandle, DescriptorHandle ColorBufferHandle, int ColorBufferHeapIndex);
		void DrawSea(const My::Frame& frame, ColorBuffer& result, ColorBuffer& src, DescriptorHandle ResultBufferHandle, DescriptorHandle ColorBufferHandle, int ColorBufferHeapIndex);
		void DrawVolumetricLight(const My::Frame& frame, ColorBuffer& result, ColorBuffer& src, DescriptorHandle& colorBufferHandle, int descriptorHeapIdx, int marchingSteps, float sampleIntensity);

		void BeginSubPass(const std::string& PassName);
		void EndSubPass();
		void BeginOverlayPass(ColorBuffer& result, ColorBuffer& src);
		void EndOverlayPass(ColorBuffer& result, ColorBuffer& src);

		void SetPipelineStatus(const std::string& PSOName);
		void SetBatchResources();
		void SetShadowResources(My::Frame& frame, uint8_t lightIdx, ColorBuffer* colorBuffer, DepthBuffer* depthBuffer, ColorBuffer* volumnBuffer);

		void TransitionResourceState(GpuResource& re, D3D12_RESOURCE_STATES state, bool flush);
	private:
		void InitializeCoreHWND();
	private:
		void SetPrimitiveType(GraphicsContext& context, My::PrimitiveType Type);
	private:
		std::unique_ptr<XM_Camera::Camera> m_Camera;
		std::unique_ptr<XM_Camera::FlyingFPSCamera> m_CameraController;
		D3D12_VIEWPORT m_MainViewport;
		D3D12_RECT m_MainScissor;
		GraphicsContext* m_pGraphicsContext = nullptr;
		GraphicsPSO* m_pGraphicsPSO = nullptr;
		RootSignature* m_pRootSignature = nullptr;

		ComputeContext* m_pComputeContext = nullptr;
		ComputePSO* m_pComputePSO = nullptr;
		My::D3d12LightManager* m_pLightManager;
	private:
		QueryFrameBufferSize m_fQueryFrameBufferSize;
		GetWindowHandleProc m_fGetWindowHandleProc;
		GetGfxconfiguration m_fGetGfxconfiguration;
		GetApplication m_fGetApplication;
		GetTimestamp m_fGetTimestamp;
	};

}