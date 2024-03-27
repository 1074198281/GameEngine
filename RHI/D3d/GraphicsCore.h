#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include <functional>
#include <DirectXMath.h>
#include "GfxConfiguration.h"
#include "StructureSettings.h"
#include "ShaderSource.h"
#include "RenderObject.h"

#include "D3dComponents/XMCamera/XMCamera.h"
#include "D3dComponents/XMCamera/XMCameraController.h"


namespace D3dGraphicsCore {
	class StructuredBuffer;
	class ByteAddressBuffer;
	class GraphicsContext;

	// CD3dGraphicsCore pretends to be D3DRHI
	class CD3dGraphicsCore {
		using QueryFrameBufferSize = std::function<void(uint32_t&, uint32_t&)>;
		using GetWindowHandleProc = std::function<HWND()>;
		using GetGfxconfiguration = std::function<const My::GfxConfiguration&()>;

	public:
		CD3dGraphicsCore();
		~CD3dGraphicsCore();

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

		uint32_t AddVertexBuffer(std::unique_ptr<StructuredBuffer> buffer);
		uint32_t AddIndexBuffer(std::unique_ptr<ByteAddressBuffer> buffer);
		size_t CreateAndAddTexture(const My::Image& img);
		void LoadIBLTextures(size_t& skyboxSpecularHandle, size_t& skyboxDiffuseHandle, size_t& brdfHandle);
	public:
		void InitializeGraphicsSettings();
		void FinalizeGraphicsSettings();
		
	public:
		void UpdateGlobalLightPosition(DirectX::XMFLOAT4 pos);
		void UpdateStatus();
		void UpdateCamera();
		void UpdateCameraParams(int64_t key);
		void UpdateRenderingQueue();
		void UpdateCubemapIndex();
		void UpdatePresent();

		void AddPrimitiveObject(std::unique_ptr<PrimitiveObject> _object);

	public:
		void DrawBatch(Frame& frame);

	private:
		void LoadIBLDDSImage(std::string& ImagePath, std::string& suffix, std::unordered_map<std::string, int>& ImageName);

		void RenderAllObjects();
		void RenderCubeMap();

	private:
		void InitializeCoreHWND();
	private:
		void SetPrimitiveType(GraphicsContext& context, My::PrimitiveType Type);
		void GetDXGIFormat(const My::PIXEL_FORMAT& pixel_format, DXGI_FORMAT& dxgi_format);
	private:
		std::vector<std::unique_ptr<PrimitiveObject> > m_PrimitiveObjects;
		std::vector<std::unique_ptr<StructuredBuffer>> m_VecVertexBuffer;
		std::vector<std::unique_ptr<ByteAddressBuffer>> m_VecIndexBuffer;
		std::vector<Texture> m_VecTexture;

		std::unique_ptr<IBLImageResource> m_IBLResource;
	private:
		std::unique_ptr<XM_Camera::Camera> m_Camera;
		std::unique_ptr<XM_Camera::FlyingFPSCamera> m_CameraController;
		D3D12_VIEWPORT m_MainViewport;
		D3D12_RECT m_MainScissor;

		DirectX::XMFLOAT4 m_GlobalLightPosition;
	private:
		QueryFrameBufferSize m_fQueryFrameBufferSize;
		GetWindowHandleProc m_fGetWindowHandleProc;
		GetGfxconfiguration m_fGetGfxconfiguration;
	};

}