#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include "Core/Pipeline/RootSignature.h"
#include "Core/Pipeline/PipelineState.h"
#include "Core/Pipeline/DescriptorHeap.h"
#include "Core/D3dGraphicsCoreManager.h"

#define DSV_FORMAT DXGI_FORMAT_D32_FLOAT

namespace D3dGraphicsCore {
	//------------------------------------DescriptorHeap------------------------------------//
	extern DescriptorHeap* g_BaseDescriptorHeap;
	extern DescriptorHandle g_BaseDescriptorHandle;
	extern std::unordered_map<unsigned int, std::unique_ptr<DescriptorHeap>> g_DescriptorHeaps;
	extern unsigned int g_CurrentHeapIndex;
	extern unsigned int g_FreeDescriptorsInCurrentHeap;
	const unsigned int g_DescriptorCountPerHeap = 1024;
	const D3D12_DESCRIPTOR_HEAP_TYPE g_DescriptorsType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	extern UINT64 g_DescriptorSize;
	void InitializeBaseDescriptorHeap();
	void FinalizeBaseDescriptorHeap();
	DescriptorHandle AllocateFromDescriptorHeap(int Count, int& iCurrentHeapIndex);
	void ReleaseHeapByIndex(int HeapIndex);
	void CopyDescriptors(const DescriptorHandle& DesHandle, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& SrcHandle, const UINT DescriptorsCount);
	void OffsetDescriptorHandle(DescriptorHandle& handle, int offset = 1);

	//---------------------------------------pipeline---------------------------------------//
	extern RootSignature g_TemplateRootSignature;
	extern RootSignature g_PresentRootSignature;
    extern GraphicsPSO g_DefaultPSO;
	extern GraphicsPSO g_SkyBoxPSO;
	extern GraphicsPSO g_PresentPSO;

	void InitializePipelineTemplates();

	void FinalizePipelineTemplates();

	void SetPipelineSettings(GraphicsPSO& PSO, const int& InputLayoutType, const My::PrimitiveType& PrimitiveType, const std::string& Name);
    
	//---------------------------------------samplers---------------------------------------//
	extern D3D12_SAMPLER_DESC g_pointWarp;
	extern D3D12_SAMPLER_DESC g_pointClamp;
	extern D3D12_SAMPLER_DESC g_linearWarp;
	extern D3D12_SAMPLER_DESC g_linearClamp;
	extern D3D12_SAMPLER_DESC g_anisotropicWarp;
	extern D3D12_SAMPLER_DESC g_anisotropicClamp;

	void InitializeSamplers();

	//---------------------------------------inputlayout------------------------------------//
	extern D3D12_INPUT_ELEMENT_DESC* g_Pos;
	extern D3D12_INPUT_ELEMENT_DESC* g_PosTex;
	extern D3D12_INPUT_ELEMENT_DESC* g_PosNorTex;
	extern D3D12_INPUT_ELEMENT_DESC* g_PosNorTanTex;

	void InitializeInputLayout();
}