#pragma once

#include <d3d12.h>
#include "d3dx12.h"
#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include "SceneObjectTypeDef.hpp"
#include "Core/Pipeline/RootSignature.h"
#include "Core/Pipeline/PipelineState.h"
#include "Core/Pipeline/DescriptorHeap.h"
#include "Core/D3dGraphicsCoreManager.h"
#include "Core/Resource/Texture.h"

#define DSV_FORMAT DXGI_FORMAT_D32_FLOAT

namespace D3dGraphicsCore {
	//------------------------------------DescriptorHeap------------------------------------//
	extern DescriptorHeap* g_BaseDescriptorHeap;
	extern DescriptorHandle g_BaseDescriptorHandle;
	extern std::unordered_map<unsigned int, std::unique_ptr<DescriptorHeap>> g_DescriptorHeaps;
	extern unsigned int g_CurrentHeapIndex;
	extern unsigned int g_FreeDescriptorsInCurrentHeap;
	const unsigned int g_DescriptorCountPerHeap = 1024;
	extern D3D12_DESCRIPTOR_HEAP_TYPE g_DescriptorsType;
	extern UINT64 g_DescriptorSize;
	void InitializeBaseDescriptorHeap();
	void FinalizeBaseDescriptorHeap();
	DescriptorHandle AllocateFromDescriptorHeap(int Count, int& iCurrentHeapIndex);
	void ReleaseHeapByIndex(int HeapIndex);
	void CopyDescriptors(const DescriptorHandle& DesHandle, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& SrcHandle, const UINT DescriptorsCount);
	void OffsetDescriptorHandle(DescriptorHandle& handle, int offset = 1);

	//-------------------------------------baseTextures-------------------------------------//

	extern std::shared_ptr<GpuTexture> g_DefaultBaseColorTexture;
	extern std::shared_ptr<GpuTexture> g_DefaultRoughnessMetallicTexture;
	extern std::shared_ptr<GpuTexture> g_DefaultOcclusionTexture;
	extern std::shared_ptr<GpuTexture> g_DefaultEmissiveTexture;
	extern std::shared_ptr<GpuTexture> g_DefaultNormalTexture;
	void InitializeDefaultTexture();
	void FinalizeDefaultTexture();


	//---------------------------------------pipeline---------------------------------------//
	extern RootSignature g_TemplateRootSignature;
	extern RootSignature g_PresentRootSignature;
	extern std::unordered_map<std::string, std::unique_ptr<GraphicsPSO>> g_PipelineStatusMap;

	extern RootSignature g_GuassBlurRootSignature;
	extern std::unordered_map<std::string, std::unique_ptr<ComputePSO>> g_ComputePSOMap;

	extern RootSignature g_ShadowOmniRootSignature;
	extern RootSignature g_ShadowAreaRootSignature;
	extern RootSignature g_ShadowSpotRootSignature;
	extern RootSignature g_ShadowInfiRootSignature;

	void InitializePipelineTemplates();

	void FinalizePipelineTemplates();

	void AddLightingShaders();

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