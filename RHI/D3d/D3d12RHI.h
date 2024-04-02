#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <functional>
#include <DirectXMath.h>
#include "GfxConfiguration.h"
#include "StructureSettings.h"
#include "ShaderSource.h"
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


	public:
		void InitializeGraphicsSettings();
		void FinalizeGraphicsSettings();
		
	public:
		void UpdateStatus();
		void UpdateCamera();
		void UpdateCameraParams(int64_t key);
		void UpdatePresent();

		

	public:
		void UpdateConstants(My::Frame& frame);
		void PrepareBatch();
		void DrawBatch(My::Frame frame, const My::D3dDrawBatchContext* pdbc, StructuredBuffer* vbuffer, ByteAddressBuffer* ibuffer,
			std::unordered_map<uint32_t, My::DescriptorHeapHandleInfo>& batch_map, ID3D12DescriptorHeap* IBLHeapPtr, DescriptorHandle IBLHandle);
		void DrawSkybox(My::Frame frame, ID3D12DescriptorHeap* HeapPtr, DescriptorHandle IBLHandle, GpuTexture* pSpecularTexture, float& SpecularIBLRange, float& SpecularIBLBias);
		void DrawGui(My::Frame frame);
		void DrawPresent(My::Frame frame, DescriptorHandle ColorBufferHandle, int ColorBufferHeapIndex);


		void BeginSubPass(std::string PassName);
		void EndSubPass();

	private:
		void InitializeCoreHWND();
	private:
		void SetPrimitiveType(GraphicsContext& context, My::PrimitiveType Type);
	private:
		std::unique_ptr<XM_Camera::Camera> m_Camera;
		std::unique_ptr<XM_Camera::FlyingFPSCamera> m_CameraController;
		D3D12_VIEWPORT m_MainViewport;
		D3D12_RECT m_MainScissor;
		GraphicsContext* m_pGraphicsContext;

	private:
		QueryFrameBufferSize m_fQueryFrameBufferSize;
		GetWindowHandleProc m_fGetWindowHandleProc;
		GetGfxconfiguration m_fGetGfxconfiguration;
	};

}