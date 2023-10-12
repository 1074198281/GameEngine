#pragma once

#include "GeometryStructure.h"
#include "Core/Common/D3dCommonDef.h"
#include "Core/Pipeline/RootSignature.h"
#include "Core/Resource/GpuBuffer.h"
#include "Core/Pipeline/PipelineState.h"
#include "Core/Common/Matrix4.h"
#include "Core/Resource/Texture.h"
#include "Core/Pipeline/DescriptorHeap.h"
#include "RenderObject.h"
#include <array>
#include <string>
#include <unordered_map>

namespace D3dGraphicsCore {

	class CD3dGraphicsCore {
	public:
		CD3dGraphicsCore();
		~CD3dGraphicsCore();

		int StartUp();
		void Finalize();
	public:
		void setCoreHWND(HWND hwnd, int width, int height);
		void InitializeGraphics();

	public:
		void UpdateStatus();

		void AddRenderObject(std::shared_ptr<My::RenderObject> _object);

	private:
		std::vector<std::shared_ptr<My::RenderObject> > m_RenderObjects;

	private:
		D3D12_VIEWPORT m_MainViewport;
		D3D12_RECT m_MainScissor;
	};

}