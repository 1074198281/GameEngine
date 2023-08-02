#pragma once

#include "Common/D3dCommonDef.h"
#include "Pipeline/RootSignature.h"
#include "Resource/GpuBuffer.h"
#include "Pipeline/PipelineState.h"
#include "Common/Matrix4.h"

namespace D3dGraphicsCore {

	class CD3dGraphicsCore {
	public:
		CD3dGraphicsCore();
		~CD3dGraphicsCore();

	public:
		void setCoreHWND(HWND hwnd, int width, int height);
		void InitializeGraphics();

	public:
		void UpdateStatus();

		//extension features
	private:
		void GenerateMatrix();
		void InitializeInputLayout();

	private:
		RootSignature m_RootSignature;
		GraphicsPSO m_PSO;
		StructuredBuffer m_VertexBuffer;
		ByteAddressBuffer m_IndexBuffer;

	private:
		D3D12_VIEWPORT m_MainViewport;
		D3D12_RECT m_MainScissor;
		Math::Matrix4 m_ProjMatrix;
		Math::Matrix4 m_ViewMatrix;
		Math::Matrix4 m_ModelMatrix;
		Math::Matrix4 m_ModelViewProjMatrix;

		D3D12_INPUT_ELEMENT_DESC* m_InputlayoutPos;
		D3D12_INPUT_ELEMENT_DESC* m_InputlayoutPosUV;
		D3D12_INPUT_ELEMENT_DESC* m_InputlayoutPosNormalUV;
		D3D12_INPUT_ELEMENT_DESC* m_InputlayoutPosNormalTangentUV;
	};

}