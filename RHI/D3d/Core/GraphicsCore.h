#pragma once

#include "Common/D3dCommonDef.h"
#include "Pipeline/RootSignature.h"
#include "Resource/GpuBuffer.h"
#include "Pipeline/PipelineState.h"
#include "Common/Matrix4.h"
#include "GeometryStructure.h"
#include "Resource/Texture.h"
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
		void UpdateView(DirectX::XMMATRIX mat);
		void InitializeGraphics();

	public:
		void UpdateStatus();

		void AddRenderObject(My::RenderObject _object);
		//extension features
	private:
		void GenerateMatrix();
		void InitializeInputLayout();

	private:
		RootSignature m_RootSignature;
		GraphicsPSO m_PSO;
		std::vector<My::RenderObject> m_RenderObjects;

	private:
		D3D12_VIEWPORT m_MainViewport;
		D3D12_RECT m_MainScissor;
		DirectX::XMMATRIX m_ProjectionMatrix;
		DirectX::XMMATRIX m_ViewMatrix;
		Math::Matrix4 m_ModelViewProjMatrix;

		D3D12_INPUT_ELEMENT_DESC* m_InputlayoutPos;
		D3D12_INPUT_ELEMENT_DESC* m_InputlayoutPosUV;
		D3D12_INPUT_ELEMENT_DESC* m_InputlayoutPosNormalUV;
		D3D12_INPUT_ELEMENT_DESC* m_InputlayoutPosNormalTangentUV;

		//for test
	private:
		void test();
	};

}