#pragma once

#include <array>
#include <string>
#include <unordered_map>
#include "Core/D3dCommonDef.h"
#include "Core/Pipeline/RootSignature.h"
#include "Core/Resource/GpuBuffer.h"
#include "Core/Pipeline/PipelineState.h"
#include "Core/Resource/Texture.h"
#include "Core/Pipeline/DescriptorHeap.h"
#include "Core/Command/CommandContext.h"
#include "StructureSettings.h"
#include "ShaderSource.h"
#include "RenderObject.h"

#include "../Component/XMCamera/XMCamera.h"
#include "../Component/XMCamera/XMCameraController.h"


namespace D3dGraphicsCore {

	class CD3dGraphicsCore {
	public:
		CD3dGraphicsCore();
		~CD3dGraphicsCore();

		int StartUp();
		void Finalize();
	public:
		void setCoreHWND(HWND hwnd, int width, int height);
		void InitializeGraphicsSettings();

	public:
		void UpdateStatus();
		void UpdateCamera();
		void UpdateRenderingQueue();

		void AddPrimitiveObject(std::unique_ptr<PrimitiveObject> _object);
		void SetPrimitiveType(GraphicsContext& context, My::PrimitiveType Type);
	private:
		void RenderAllObjects(GraphicsContext& gfxContext);
	private:
		std::vector<std::unique_ptr<PrimitiveObject> > m_PrimitiveObjects;

	private:
		XM_Camera::Camera m_Camera;
		std::unique_ptr<XM_Camera::FlyingFPSCamera> m_CameraController;
		D3D12_VIEWPORT m_MainViewport;
		D3D12_RECT m_MainScissor;
	};

}