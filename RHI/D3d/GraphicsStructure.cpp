#include "GraphicsStructure.h"
#include "MemoryManager.hpp"
#include "StructureSettings.h"
#include "Core/Common/GraphicsCommon.h"

//namespace D3dGraphicsCore {
//	ID3D12Device* g_Device;
//}

namespace D3dGraphicsCore {
	DescriptorHeap* g_BaseDescriptorHeap;
	std::unordered_map<unsigned int, std::unique_ptr<DescriptorHeap>> g_DescriptorHeaps;
	unsigned int g_CurrentHeapIndex = 0;
	unsigned int g_FreeDescriptorsInCurrentHeap = g_DescriptorCountPerHeap;

	RootSignature g_TemplateRootSignature;
    GraphicsPSO g_DefaultPSO(L"Default PSO");

	D3D12_SAMPLER_DESC g_pointWarp;
	D3D12_SAMPLER_DESC g_pointClamp;
	D3D12_SAMPLER_DESC g_linearWarp;
	D3D12_SAMPLER_DESC g_linearClamp;
	D3D12_SAMPLER_DESC g_anisotropicWarp;
	D3D12_SAMPLER_DESC g_anisotropicClamp;

	D3D12_INPUT_ELEMENT_DESC* g_Pos;
	D3D12_INPUT_ELEMENT_DESC* g_PosTex;
	D3D12_INPUT_ELEMENT_DESC* g_PosNorTex;
	D3D12_INPUT_ELEMENT_DESC* g_PosNorTanTex;
}

void D3dGraphicsCore::InitializeBaseDescriptorHeap()
{
	std::unique_ptr<DescriptorHeap> newHeap = std::make_unique<DescriptorHeap>();
	g_CurrentHeapIndex = 0;
	newHeap->Create(L"BaseDescriptorHeap" + std::to_wstring(g_CurrentHeapIndex), g_DescriptorsType, g_DescriptorCountPerHeap);
	g_BaseDescriptorHeap = newHeap.get();
	g_DescriptorHeaps.emplace(g_CurrentHeapIndex, std::move(newHeap));
	g_FreeDescriptorsInCurrentHeap = g_DescriptorCountPerHeap;
}

void D3dGraphicsCore::FinalizeBaseDescriptorHeap()
{
	for (auto it = g_DescriptorHeaps.begin(); it != g_DescriptorHeaps.end(); it++) {
		it->second->Destroy();
		it->second.reset();
	}
	g_DescriptorHeaps.clear();
	g_BaseDescriptorHeap = nullptr;
}

D3dGraphicsCore::DescriptorHandle D3dGraphicsCore::AllocateFromDescriptorHeap(int Count, int& iCurrentHeapIndex)
{
	D3dGraphicsCore::DescriptorHandle Handle;
	if (!Count) {
		iCurrentHeapIndex = -1;
		return Handle;
	}
	if (Count > g_FreeDescriptorsInCurrentHeap) {
		std::unique_ptr<DescriptorHeap> newHeap = std::make_unique<DescriptorHeap>();
		g_CurrentHeapIndex++;
		newHeap->Create(L"DescriptorHeap" + std::to_wstring(g_CurrentHeapIndex), g_DescriptorsType, 
			Count > g_DescriptorCountPerHeap ? Count : g_DescriptorCountPerHeap);
		g_BaseDescriptorHeap = newHeap.get();
		g_DescriptorHeaps.emplace(g_CurrentHeapIndex, std::move(newHeap));
		g_FreeDescriptorsInCurrentHeap = g_DescriptorCountPerHeap;
	}

	Handle = g_BaseDescriptorHeap->Alloc(Count);
	g_FreeDescriptorsInCurrentHeap -= Count;
	iCurrentHeapIndex = g_CurrentHeapIndex;
	return Handle;
}

void D3dGraphicsCore::ReleaseHeapByIndex(int HeapIndex)
{
	ASSERT(HeapIndex > g_CurrentHeapIndex, "Release Index Over Current Heap Index!");
	g_DescriptorHeaps[HeapIndex]->Destroy();
	g_DescriptorHeaps[HeapIndex].reset();
	g_DescriptorHeaps.erase(HeapIndex);
}

void D3dGraphicsCore::CopyDescriptors(const DescriptorHandle& DesHandle, const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& SrcHandle, const UINT DescriptorsCount)
{
	std::vector<UINT> SourceCounts;
	for (int i = 0; i < DescriptorsCount; i++) {
		SourceCounts.push_back(1);
	}

	g_Device->CopyDescriptors(1, &DesHandle, &DescriptorsCount,
		DescriptorsCount, SrcHandle.data(), SourceCounts.data(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void D3dGraphicsCore::InitializePipelineTemplates()
{
	g_TemplateRootSignature.Reset(RootBindings::kNumRootBindings, 0); 	//暂时不使用采样器
	g_TemplateRootSignature[kMeshConstant].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	g_TemplateRootSignature[kMaterialConstant].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	g_TemplateRootSignature[kMaterialSRVs].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 10, D3D12_SHADER_VISIBILITY_PIXEL);
	g_TemplateRootSignature[kMaterialSamplers].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 0, 10, D3D12_SHADER_VISIBILITY_PIXEL);
	g_TemplateRootSignature[kCommonSRVs].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 10, 10, D3D12_SHADER_VISIBILITY_PIXEL);
	g_TemplateRootSignature[kCommonCBV].InitAsConstantBuffer(1);
	//g_TemplateRootSignature[kSkinMatrices].InitAsBufferSRV(20, D3D12_SHADER_VISIBILITY_VERTEX);
	g_TemplateRootSignature.Finalize(L"TemplateRootSig", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	g_DefaultPSO.SetRootSignature(g_TemplateRootSignature);
	g_DefaultPSO.SetRasterizerState(RasterizerDefault);
	g_DefaultPSO.SetBlendState(BlendDisable);
	g_DefaultPSO.SetDepthStencilState(DepthStateDisabled);
	//g_DefaultPSO.SetInputLayout(4, g_PosNorTanTex);
	//g_DefaultPSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
}

void D3dGraphicsCore::FinalizePipelineTemplates()
{
	g_TemplateRootSignature.DestroyAll();
	g_DefaultPSO.DestroyAll();
}

void D3dGraphicsCore::SetPipelineSettings(D3dGraphicsCore::GraphicsPSO& PSO, int InputLayoutType, My::PrimitiveType PrimitiveType)
{
	//PrimitiveType
	D3D12_PRIMITIVE_TOPOLOGY_TYPE d3dType;
	switch (PrimitiveType) {
	case My::kPrimitiveTypeLineStrip:
	{
		d3dType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	}
	break;
	case My::kPrimitiveTypeTriList:
	{
		d3dType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
	break;
	default:
		ASSERT(false, "ERROR! No Configured Primitive Type!");
		break;
	}

	PSO.SetPrimitiveTopologyType(d3dType);

	//InputLayoutType
	if (InputLayoutType == (1 << kPos)) {
		PSO.SetInputLayout(1, g_Pos);
	}
	else if (InputLayoutType == ((1 << kPos) + (1 << kTexcoord0))) {
		PSO.SetInputLayout(2, g_PosTex);
	}
	else if (InputLayoutType == ((1 << kPos) + (1 << kNormal) + (1 << kTexcoord0))) {
		PSO.SetInputLayout(3, g_PosNorTex);
	}
	else if (InputLayoutType == ((1 << kPos) + (1 << kNormal) + (1 << kTangent) + (1 << kTexcoord0))) {
		PSO.SetInputLayout(4, g_PosNorTanTex);
	}
	else {
		ASSERT(false, "InputLayout Type ERROR!");
	}
}

void D3dGraphicsCore::InitializeSamplers()
{
	g_pointWarp.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	g_pointWarp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_pointWarp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_pointWarp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_pointWarp.BorderColor[0] = 1.0f;
	g_pointWarp.BorderColor[1] = 1.0f;
	g_pointWarp.BorderColor[2] = 1.0f;
	g_pointWarp.BorderColor[3] = 1.0f;
	g_pointWarp.MinLOD = 0.0f;
	g_pointWarp.MaxLOD = D3D12_FLOAT32_MAX;        
	g_pointWarp.MipLODBias = 0.0f;
	g_pointWarp.MaxAnisotropy = 8;
	g_pointWarp.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	g_pointClamp = g_pointWarp;
	g_pointClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	g_pointClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	g_pointClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	g_pointClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	
	g_linearWarp = g_pointWarp;
	g_linearWarp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	g_linearWarp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_linearWarp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_linearWarp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	g_linearClamp = g_pointWarp;
	g_linearClamp.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	g_linearClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	g_linearClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	g_linearClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	g_anisotropicWarp = g_pointWarp;
	g_anisotropicWarp.Filter = D3D12_FILTER_ANISOTROPIC;
	g_anisotropicWarp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_anisotropicWarp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_anisotropicWarp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	g_anisotropicClamp = g_pointWarp;
	g_anisotropicClamp.Filter = D3D12_FILTER_ANISOTROPIC;
	g_anisotropicClamp.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	g_anisotropicClamp.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	g_anisotropicClamp.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
}

void D3dGraphicsCore::InitializeInputLayout()
{
	g_Pos = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(sizeof(D3D12_INPUT_ELEMENT_DESC));
	g_PosTex = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(2 * sizeof(D3D12_INPUT_ELEMENT_DESC));
	g_PosNorTex = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(3 * sizeof(D3D12_INPUT_ELEMENT_DESC));
	g_PosNorTanTex = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(4 * sizeof(D3D12_INPUT_ELEMENT_DESC));

	g_Pos[0] =  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 } ;

	g_PosTex[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosTex[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

	g_PosNorTex[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTex[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTex[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

	g_PosNorTanTex[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTanTex[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTanTex[2] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTanTex[3] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

}