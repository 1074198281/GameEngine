#include "GraphicsStructure.h"
#include "MemoryManager.hpp"
#include "StructureSettings.h"
#include "Core/Common/GraphicsCommon.h"
#include "Core/D3dGraphicsCoreManager.h"
#include "Modules/ShaderSource/ShaderSource.h"
#include "../Windows/WinUtility.h"


namespace D3dGraphicsCore {
	DescriptorHeap* g_BaseDescriptorHeap;
	std::unordered_map<unsigned int, std::unique_ptr<DescriptorHeap>> g_DescriptorHeaps;
	unsigned int g_CurrentHeapIndex = 0;
	unsigned int g_FreeDescriptorsInCurrentHeap = g_DescriptorCountPerHeap;
	D3D12_DESCRIPTOR_HEAP_TYPE g_DescriptorsType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	UINT64 g_DescriptorSize = 0;

	std::shared_ptr<GpuTexture> g_DefaultBaseColorTexture;
	std::shared_ptr<GpuTexture> g_DefaultRoughnessMetallicTexture;
	std::shared_ptr<GpuTexture> g_DefaultOcclusionTexture;
	std::shared_ptr<GpuTexture> g_DefaultEmissiveTexture;
	std::shared_ptr<GpuTexture> g_DefaultNormalTexture;

	RootSignature g_TemplateRootSignature;
	RootSignature g_PresentRootSignature;
	RootSignature g_WaterDropsSubRootSignature;
	RootSignature g_VolumetricLightSubRootSignature;
	RootSignature g_ShadowRootSignature;
	std::unordered_map<std::string, std::unique_ptr<GraphicsPSO>> g_PipelineStatusMap;

	RootSignature g_GaussBlurRootSignature;
	std::unordered_map<std::string, std::unique_ptr<ComputePSO>> g_ComputePSOMap;

	RootSignature g_ShadowOmniRootSignature;
	RootSignature g_ShadowAreaRootSignature;
	RootSignature g_ShadowSpotRootSignature;
	RootSignature g_ShadowInfiRootSignature;

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
	g_DescriptorSize = g_Device->GetDescriptorHandleIncrementSize(g_DescriptorsType);
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

void D3dGraphicsCore::CopyDescriptors(const DescriptorHandle& DesHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* SrcHandle, const UINT DescriptorsCount)
{
	std::vector<UINT> SourceCounts;
	for (int i = 0; i < DescriptorsCount; i++) {
		SourceCounts.push_back(1);
	}

	g_Device->CopyDescriptors(1, &DesHandle, &DescriptorsCount,
		DescriptorsCount, SrcHandle, SourceCounts.data(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void D3dGraphicsCore::OffsetDescriptorHandle(DescriptorHandle& handle, int offset)
{
	handle += offset * g_DescriptorSize;
}

void D3dGraphicsCore::InitializeDefaultTexture()
{
	const UINT paddingSize = 16;
	const UINT textureWidth = 1;
	const UINT textureHeight = 1;
	const UINT pixelSize = 4; // RGBA8，每像素4字节

	UINT8 WhiteTextureData[paddingSize];
	UINT8 BlackTextureData[paddingSize];
	UINT8 NormalTextureData[paddingSize];
	memset(WhiteTextureData, 0, paddingSize * sizeof(UINT8));
	memset(BlackTextureData, 0, paddingSize * sizeof(UINT8));
	memset(NormalTextureData, 0, paddingSize * sizeof(UINT8));

	WhiteTextureData[0] = 255;
	WhiteTextureData[1] = 255;
	WhiteTextureData[2] = 255;
	WhiteTextureData[3] = 255;

	BlackTextureData[0] = 0;
	BlackTextureData[1] = 0;
	BlackTextureData[2] = 0;
	BlackTextureData[3] = 255;

	NormalTextureData[0] = 128;
	NormalTextureData[1] = 128;
	NormalTextureData[2] = 255;
	NormalTextureData[3] = 255;

	g_DefaultBaseColorTexture = std::make_shared<GpuTexture>();
	g_DefaultRoughnessMetallicTexture = std::make_shared<GpuTexture>();
	g_DefaultOcclusionTexture = std::make_shared<GpuTexture>();
	g_DefaultEmissiveTexture = std::make_shared<GpuTexture>();
	g_DefaultNormalTexture = std::make_shared<GpuTexture>();

	g_DefaultBaseColorTexture->Create2D(4, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, WhiteTextureData);
	g_DefaultRoughnessMetallicTexture->Create2D(4, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, WhiteTextureData);
	g_DefaultOcclusionTexture->Create2D(4, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, WhiteTextureData);
	g_DefaultEmissiveTexture->Create2D(4, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, BlackTextureData);
	g_DefaultNormalTexture->Create2D(4, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, NormalTextureData);
}

void D3dGraphicsCore::FinalizeDefaultTexture()
{
	g_DefaultBaseColorTexture->Destroy();
	g_DefaultRoughnessMetallicTexture->Destroy();
	g_DefaultOcclusionTexture->Destroy();
	g_DefaultEmissiveTexture->Destroy();
	g_DefaultNormalTexture->Destroy();
}

void D3dGraphicsCore::InitializePipelineTemplates()
{
	InitializeGeometryPipelines();
	InitializeOverlayPipelines();
	InitializeLightPipelines();
}

void D3dGraphicsCore::InitializeGeometryPipelines()
{
	SamplerDesc DefaultSamplerDesc;
	DefaultSamplerDesc.MaxAnisotropy = 8;

	SamplerDesc CubeMapSamplerDesc = DefaultSamplerDesc;

	g_TemplateRootSignature.Reset(My::RootBindings::kNumRootBindings, 2); 	//使用2个采样器
	g_TemplateRootSignature[My::kMeshConstants].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	g_TemplateRootSignature[My::kMaterialConstants].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	g_TemplateRootSignature[My::kMaterialSRVs].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 20, D3D12_SHADER_VISIBILITY_PIXEL);
	g_TemplateRootSignature[My::kMaterialSamplers].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 0, 10, D3D12_SHADER_VISIBILITY_PIXEL);
	g_TemplateRootSignature[My::kCommonSRVs].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 20, 15, D3D12_SHADER_VISIBILITY_PIXEL);
	g_TemplateRootSignature[My::kCommonBatchConstantsCBV].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_ALL);
	g_TemplateRootSignature[My::kCommonFrameConstantsCBV].InitAsConstantBuffer(2, D3D12_SHADER_VISIBILITY_ALL);
	g_TemplateRootSignature[My::kCommonLightConstantsCBV].InitAsConstantBuffer(3, D3D12_SHADER_VISIBILITY_ALL);
	g_TemplateRootSignature[My::kShadowSRVs].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 100, MAX_LIGHT_NUM, D3D12_SHADER_VISIBILITY_PIXEL);
	//g_TemplateRootSignature[kSkinMatrices].InitAsBufferSRV(20, D3D12_SHADER_VISIBILITY_VERTEX);
	g_TemplateRootSignature.InitStaticSampler(10, DefaultSamplerDesc);
	//g_TemplateRootSignature.InitStaticSampler(11, ShadowMapSamplerDesc);
	g_TemplateRootSignature.InitStaticSampler(12, CubeMapSamplerDesc);
	g_TemplateRootSignature.Finalize(L"TemplateRootSig", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


	// Default
	std::unique_ptr<GraphicsPSO> pDefaultPSO = std::make_unique<GraphicsPSO>(L"Default PSO");
	pDefaultPSO->SetRootSignature(g_TemplateRootSignature);
	pDefaultPSO->SetRasterizerState(RasterizerDefault);
	pDefaultPSO->SetBlendState(BlendDisable);
	pDefaultPSO->SetDepthStencilState(DepthStateReadWriteLess);
	pDefaultPSO->SetRenderTargetFormat(g_SceneColorBufferFormat, g_SceneDepthBufferFormat);
	pDefaultPSO->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pDefaultPSO->SetInputLayout(3, g_PosNorTex);
	SetShaderByteCode(*pDefaultPSO.get(), "Model");
	pDefaultPSO->Finalize();

	//SkyBox
	std::unique_ptr<GraphicsPSO> pSkyBoxPSO = std::make_unique<GraphicsPSO>(L"SkyBox PSO");
	auto RasterizerSkyBox = RasterizerDefault;
	RasterizerSkyBox.CullMode = D3D12_CULL_MODE_NONE;
	pSkyBoxPSO->SetRootSignature(g_TemplateRootSignature);
	pSkyBoxPSO->SetRasterizerState(RasterizerSkyBox);
	pSkyBoxPSO->SetDepthStencilState(DepthStateReadWriteLess);
	pSkyBoxPSO->SetBlendState(BlendDisable);
	pSkyBoxPSO->SetInputLayout(0, nullptr);
	SetShaderByteCode(*pSkyBoxPSO.get(), "SkyBox");
	pSkyBoxPSO->SetRenderTargetFormat(g_SceneColorBufferFormat, DSV_FORMAT);
	pSkyBoxPSO->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pSkyBoxPSO->Finalize();

	g_PipelineStatusMap.emplace("Default", std::move(pDefaultPSO));
	g_PipelineStatusMap.emplace("Skybox", std::move(pSkyBoxPSO));
}

void D3dGraphicsCore::InitializeOverlayPipelines()
{
	SamplerDesc DefaultSamplerDesc;
	DefaultSamplerDesc.MaxAnisotropy = 8;

	g_PresentRootSignature.Reset(1, 2);
	g_PresentRootSignature[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	g_PresentRootSignature.InitStaticSampler(16, g_linearWarp);
	g_PresentRootSignature.InitStaticSampler(17, g_linearClamp);
	g_PresentRootSignature.Finalize(L"PresentRootSig");

	g_WaterDropsSubRootSignature.Reset(My::kWaterDropsRootBindings, 6);
	g_WaterDropsSubRootSignature[My::kWaterDropsSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	g_WaterDropsSubRootSignature[My::kWaterDropsCBV].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	g_WaterDropsSubRootSignature.InitStaticSampler(16, g_linearWarp);
	g_WaterDropsSubRootSignature.InitStaticSampler(17, g_linearClamp);
	g_WaterDropsSubRootSignature.InitStaticSampler(18, g_pointWarp);
	g_WaterDropsSubRootSignature.InitStaticSampler(19, g_pointClamp);
	g_WaterDropsSubRootSignature.InitStaticSampler(20, g_anisotropicWarp);
	g_WaterDropsSubRootSignature.InitStaticSampler(21, g_anisotropicClamp);
	g_WaterDropsSubRootSignature.Finalize(L"WaterDropsSubRootSig");

	g_VolumetricLightSubRootSignature.Reset(My::kVolumetricLightRootBindings, 2);
	g_VolumetricLightSubRootSignature[My::kCameraDepthSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	g_VolumetricLightSubRootSignature[My::kLightDepthSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, MAX_LIGHT_NUM, D3D12_SHADER_VISIBILITY_PIXEL);
	g_VolumetricLightSubRootSignature[My::kPresentSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 101, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	g_VolumetricLightSubRootSignature[My::kLightCBV].InitAsConstantBuffer(0);
	g_VolumetricLightSubRootSignature[My::kVolumnCBV].InitAsConstantBuffer(1);
	g_VolumetricLightSubRootSignature.InitStaticSampler(16, g_linearWarp);
	g_VolumetricLightSubRootSignature.InitStaticSampler(17, g_linearClamp);
	g_VolumetricLightSubRootSignature.Finalize(L"VolumetricLightSubRootSig");

	// Present
	std::unique_ptr<GraphicsPSO> pPresentPSO = std::make_unique<GraphicsPSO>(L"Present PSO");
	pPresentPSO->SetRootSignature(g_PresentRootSignature);
	pPresentPSO->SetRasterizerState(RasterizerDefault);
	pPresentPSO->SetBlendState(BlendDisable);
	pPresentPSO->SetDepthStencilState(DepthStateDisabled);
	pPresentPSO->SetSampleMask(0xFFFFFFFF);
	pPresentPSO->SetInputLayout(0, nullptr);
	SetShaderByteCode(*pPresentPSO.get(), "Present");
	pPresentPSO->SetRenderTargetFormat(DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_UNKNOWN);		// RT是DisplayBuffer
	pPresentPSO->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pPresentPSO->Finalize();

	// WaterDrops Sub
	std::unique_ptr<GraphicsPSO> pWaterDropsSubPSO = std::make_unique<GraphicsPSO>(L"WaterDropsSub PSO");
	pWaterDropsSubPSO->SetRootSignature(g_WaterDropsSubRootSignature);
	pWaterDropsSubPSO->SetRasterizerState(RasterizerDefault);
	pWaterDropsSubPSO->SetBlendState(BlendDisable);
	pWaterDropsSubPSO->SetDepthStencilState(DepthStateDisabled);
	pWaterDropsSubPSO->SetSampleMask(0xFFFFFFFF);
	pWaterDropsSubPSO->SetInputLayout(0, nullptr);
	SetShaderByteCode(*pWaterDropsSubPSO.get(), "WaterDropsSub");
	pWaterDropsSubPSO->SetRenderTargetFormat(g_SceneColorBufferFormat, DXGI_FORMAT_UNKNOWN);	// RT是ColorBuffer
	pWaterDropsSubPSO->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pWaterDropsSubPSO->Finalize();

	// Volumetric Light Sub
	std::unique_ptr<GraphicsPSO> pVolumetricLightPSO = std::make_unique<GraphicsPSO>(L"VolumetricLight PSO");
	pVolumetricLightPSO->SetRootSignature(g_VolumetricLightSubRootSignature);
	pVolumetricLightPSO->SetRasterizerState(RasterizerDefault);
	pVolumetricLightPSO->SetBlendState(BlendDisable);
	pVolumetricLightPSO->SetDepthStencilState(DepthStateDisabled);
	pVolumetricLightPSO->SetSampleMask(0xFFFFFFFF);
	pVolumetricLightPSO->SetInputLayout(0, nullptr);
	SetShaderByteCode(*pVolumetricLightPSO.get(), "VolumetricLight");
	pVolumetricLightPSO->SetRenderTargetFormat(g_SceneColorBufferFormat, DXGI_FORMAT_UNKNOWN);	// RT是ColorBuffer
	pVolumetricLightPSO->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pVolumetricLightPSO->Finalize();

	g_PipelineStatusMap.emplace("Present", std::move(pPresentPSO));
	g_PipelineStatusMap.emplace("WaterDrops", std::move(pWaterDropsSubPSO));
	g_PipelineStatusMap.emplace("VolumetricLight", std::move(pVolumetricLightPSO));


	// Compute RootSignature and PSOs
	g_GaussBlurRootSignature.Reset(My::kComputeRootBindings - 1);
	g_GaussBlurRootSignature[My::kComputeSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	g_GaussBlurRootSignature[My::kComputeUAV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	g_GaussBlurRootSignature.Finalize(L"Gauss Blur");

	std::unique_ptr<ComputePSO> GaussBlurPSO = std::make_unique<ComputePSO>(L"GaussBlur PSO");
	GaussBlurPSO->SetRootSignature(g_GaussBlurRootSignature);
	D3D12_SHADER_BYTECODE GaussBlurShader = CD3DX12_SHADER_BYTECODE();
	GaussBlurShader.pShaderBytecode = g_ShaderByteMap["MyGaussBlurCS"].pShaderByteCode;
	GaussBlurShader.BytecodeLength = g_ShaderByteMap["MyGaussBlurCS"].size;
	GaussBlurPSO->SetComputeShader(GaussBlurShader);
	GaussBlurPSO->Finalize();
	g_ComputePSOMap.emplace("GaussBlur_CS", std::move(GaussBlurPSO));
}

void D3dGraphicsCore::InitializeLightPipelines()
{
	SamplerDesc DefaultSamplerDesc;
	DefaultSamplerDesc.MaxAnisotropy = 8;

	g_ShadowSpotRootSignature.Reset(My::kShadowRootBindings, 1);
	g_ShadowSpotRootSignature[My::kShadowBatchCBV].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_ALL);
	g_ShadowSpotRootSignature[My::kShadowFrameCBV].InitAsConstantBuffer(2, D3D12_SHADER_VISIBILITY_ALL);
	g_ShadowSpotRootSignature[My::kShadowSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	g_ShadowSpotRootSignature.InitStaticSampler(10, DefaultSamplerDesc);
	g_ShadowSpotRootSignature.Finalize(L"SpotLightRootSig", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//Spot light
	std::unique_ptr<GraphicsPSO> pSpotLightPSO = std::make_unique<GraphicsPSO>(L"SpotLight PSO");
	pSpotLightPSO->SetRootSignature(g_ShadowSpotRootSignature);
	pSpotLightPSO->SetRasterizerState(RasterizerDefault);
	pSpotLightPSO->SetBlendState(BlendDisable);
	pSpotLightPSO->SetDepthStencilState(DepthStateReadWriteLess);
	pSpotLightPSO->SetSampleMask(0xFFFFFFFF);
	pSpotLightPSO->SetInputLayout(3, g_PosNorTex);
	SetShaderByteCode(*pSpotLightPSO.get(), "SpotLight");
	pSpotLightPSO->SetRenderTargetFormat(g_SceneColorBufferFormat, DSV_FORMAT);	// RT是ColorBuffer
	pSpotLightPSO->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pSpotLightPSO->Finalize();

	g_PipelineStatusMap.emplace("SpotLight", std::move(pSpotLightPSO));

	//lighting shading model
	AddLightingShaders();
}

void D3dGraphicsCore::FinalizePipelineTemplates()
{
	g_TemplateRootSignature.DestroyAll();
	g_PresentRootSignature.DestroyAll();
	for (auto& _it : g_PipelineStatusMap) {
		_it.second->DestroyAll();
		_it.second.reset();
	}
	for (auto& _it : g_ComputePSOMap) {
		_it.second->DestroyAll();
		_it.second.reset();
	}
}

void D3dGraphicsCore::SetPipelineSettings(D3dGraphicsCore::GraphicsPSO& PSO, const int& InputLayoutType, const My::PrimitiveType& PrimitiveType, const std::string& Name)
{
	std::wstring wName = My::UTF8ToWideString(Name);
	size_t size = wName.size() + 1;
	const wchar_t* LName = wName.c_str();
	wchar_t* newname = new wchar_t[wName.size()];
	wcscpy(newname, LName);
	PSO = GraphicsPSO(newname);
	PSO.SetRootSignature(g_TemplateRootSignature);
	PSO.SetRasterizerState(RasterizerDefault);
	PSO.SetBlendState(BlendDisable);
	PSO.SetDepthStencilState(DepthStateReadWriteLess);

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
	if (InputLayoutType == (1 << My::kPos)) {
		PSO.SetInputLayout(1, g_Pos);
	}
	else if (InputLayoutType == ((1 << My::kPos) + (1 << My::kTexcoord0))) {
		PSO.SetInputLayout(2, g_PosTex);
	}
	else if (InputLayoutType == ((1 << My::kPos) + (1 << My::kNormal) + (1 << My::kTexcoord0))) {
		PSO.SetInputLayout(3, g_PosNorTex);
	}
	else if (InputLayoutType == ((1 << My::kPos) + (1 << My::kNormal) + (1 << My::kTangent) + (1 << My::kTexcoord0))) {
		PSO.SetInputLayout(4, g_PosNorTanTex);
	}
	else {
		ASSERT(false, "InputLayout Type ERROR!");
	}
}

void D3dGraphicsCore::AddLightingShaders()
{
	std::unique_ptr<GraphicsPSO> pLambertLighting = std::make_unique<GraphicsPSO>(L"LambertGouraudLighting"); \
	pLambertLighting->SetRootSignature(g_TemplateRootSignature);
	pLambertLighting->SetRasterizerState(RasterizerDefault);
	pLambertLighting->SetBlendState(BlendDisable);
	pLambertLighting->SetDepthStencilState(DepthStateReadWriteLess);
	pLambertLighting->SetRenderTargetFormat(g_SceneColorBufferFormat, g_SceneDepthBufferFormat);
	pLambertLighting->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pLambertLighting->SetInputLayout(3, g_PosNorTex);
	SetShaderByteCode(*pLambertLighting.get(), "LambertGouraudLighting");
	pLambertLighting->Finalize();
	g_PipelineStatusMap.emplace("LambertGouraudLighting", std::move(pLambertLighting));

	std::unique_ptr<GraphicsPSO> pBlinnPhongLighting = std::make_unique<GraphicsPSO>(L"BlinnPhongLighting"); \
	pBlinnPhongLighting->SetRootSignature(g_TemplateRootSignature);
	pBlinnPhongLighting->SetRasterizerState(RasterizerDefault);
	pBlinnPhongLighting->SetBlendState(BlendDisable);
	pBlinnPhongLighting->SetDepthStencilState(DepthStateReadWriteLess);
	pBlinnPhongLighting->SetRenderTargetFormat(g_SceneColorBufferFormat, g_SceneDepthBufferFormat);
	pBlinnPhongLighting->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pBlinnPhongLighting->SetInputLayout(3, g_PosNorTex);
	SetShaderByteCode(*pBlinnPhongLighting.get(), "BlinnPhongLighting");
	pBlinnPhongLighting->Finalize();
	g_PipelineStatusMap.emplace("BlinnPhongLighting", std::move(pBlinnPhongLighting));
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
	g_Pos = (D3D12_INPUT_ELEMENT_DESC*)new D3D12_INPUT_ELEMENT_DESC;
	g_PosTex = (D3D12_INPUT_ELEMENT_DESC*)new D3D12_INPUT_ELEMENT_DESC[2];
	g_PosNorTex = (D3D12_INPUT_ELEMENT_DESC*)new D3D12_INPUT_ELEMENT_DESC[3];
	g_PosNorTanTex = (D3D12_INPUT_ELEMENT_DESC*)new D3D12_INPUT_ELEMENT_DESC[4];

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