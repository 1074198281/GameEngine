#include "D3dPipeline.h"
#include "ShaderByteCodeHeader.h"
#include "../../Core/Resource/SamplerManager.h"
#include "../../StructureSettings.h"
#include "../../Core/D3dGraphicsCoreManager.h"
#include "../../Modules/ShaderSource/GraphicsStructure.h"

using namespace My;
using namespace D3dGraphicsCore;


CD3dPipeline::CD3dPipeline(bool isCompute)
{
	if (!isCompute) {
		m_pPSO = std::make_shared<GraphicsPSO>(L"Unnamed Graphics PSO");
		m_pComputePSO = nullptr;
	}
	else {
		m_pComputePSO = std::make_shared<ComputePSO>();
		m_pPSO = nullptr;
	}
	
	m_pRootSignature = std::make_shared<RootSignature>();
}

CD3dPipeline::~CD3dPipeline()
{

}

CD3dPipeline::CD3dPipeline(const wchar_t* name, bool isCompute)
{
	if (!isCompute) {
		m_pPSO = std::make_shared<GraphicsPSO>(name);
		m_pComputePSO = nullptr;
	}
	else {
		m_pComputePSO = std::make_shared<ComputePSO>();
		m_pPSO = nullptr;
	}
	m_pRootSignature = std::make_shared<RootSignature>();
}

void CD3dPipeline::SetRootSignature(std::shared_ptr<RootSignature> rootsignature)
{
	m_pRootSignature.reset();
	m_pRootSignature = rootsignature;
}

//----------------------------------------------CD3dPipelineManager----------------------------------------------//

CD3dPipelineManager::CD3dPipelineManager()
{

}

CD3dPipelineManager::~CD3dPipelineManager()
{

}

CD3dPipeline* CD3dPipelineManager::GetGraphicsPSO(std::string PSOName)
{
	if (m_GraphicsPSOMap.find(PSOName) != m_GraphicsPSOMap.end()) {
		return m_GraphicsPSOMap[PSOName].get();
	}
	ASSERT(false, "Error! No Such Graphics PSO!");
	return nullptr;
}

CD3dPipeline* CD3dPipelineManager::GetComputePSO(std::string PSOName)
{
	if (m_ComputePSOMap.find(PSOName) != m_ComputePSOMap.end()) {
		return m_ComputePSOMap[PSOName].get();
	}
	ASSERT(false, "Error! No Such Compute PSO!");
	return nullptr;
}


void CD3dPipelineManager::SetShaderByteCode(GraphicsPSO& PSO, std::string name)
{
#ifndef SET_SHADER_BYTE_CODE
#define SET_SHADER_BYTE_CODE( NAME ) \
	std::string VSName = NAME + "VS"; \
	std::string PSName = NAME + "PS"; \
	ShaderByteCode VS = m_ShaderByteMap[VSName]; \
	ShaderByteCode PS = m_ShaderByteMap[PSName]; \
	PSO.SetVertexShader(VS.pShaderByteCode, VS.size); \
	PSO.SetPixelShader(PS.pShaderByteCode, PS.size);

	SET_SHADER_BYTE_CODE(name);

#undef SET_SHADER_BYTE_CODE
#endif // !SET_SHADER_BYTE_CODE
}

void CD3dPipelineManager::InitializeShaderByteMap()
{
#ifndef GENERATE_SHADER_BYTE_CODE
#ifdef TOSTRING
#undef TOSTRING
#endif // TOSTRING
#define TOSTRING( NAME ) #NAME
#define GENERATE_SHADER_BYTE_CODE( NAME ) \
	ShaderByteCode NAME##VS = { g_p##NAME##_VS, sizeof(g_p##NAME##_VS) }; \
	ShaderByteCode NAME##PS = { g_p##NAME##_PS, sizeof(g_p##NAME##_PS) }; \
	m_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##VS), NAME##VS)); \
	m_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##PS), NAME##PS)); \

	//GENERATE_SHADER_BYTE_CODE(Cube);
	//GENERATE_SHADER_BYTE_CODE(ToyCar);
	//GENERATE_SHADER_BYTE_CODE(Fabric);
	GENERATE_SHADER_BYTE_CODE(SkyBox);
	GENERATE_SHADER_BYTE_CODE(Present);
	GENERATE_SHADER_BYTE_CODE(Model);
	GENERATE_SHADER_BYTE_CODE(WaterDropsSub);
	GENERATE_SHADER_BYTE_CODE(SeaSub);

	// lighting
	GENERATE_SHADER_BYTE_CODE(LambertGouraudLighting);
	GENERATE_SHADER_BYTE_CODE(BlinnPhongLighting);

	//shadow
	GENERATE_SHADER_BYTE_CODE(SpotLight);
	GENERATE_SHADER_BYTE_CODE(VolumetricLight);
#undef TOSTRING
#undef GENERATE_SHADER_BYTE_CODE
#endif // !GENERATE_SHADER_BYTE_CODE

#ifndef GENERATE_CS_SHADER_BYTE_CODE
#ifdef TOSTRING
#undef TOSTRING
#endif // TOSTRING
#define TOSTRING( NAME ) #NAME
#define GENERATE_CS_SHADER_BYTE_CODE(NAME)\
	ShaderByteCode cs##NAME##ByteCode = { g_p##NAME##_CS, sizeof(g_p##NAME##_CS)};\
	m_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##CS), cs##NAME##ByteCode)); \

	GENERATE_CS_SHADER_BYTE_CODE(GaussBlur)
	GENERATE_CS_SHADER_BYTE_CODE(MyGaussBlur)

#endif // GENERATE_CS_SHADER_BYTE_CODE

}

void CD3dPipelineManager::FinalizeShaderByteMap()
{
	m_ShaderByteMap.clear();
}

void CD3dPipelineManager::InitializePipelineTemplates()
{
	m_pDefaultRootSignature = std::make_shared<RootSignature>();

	InitializeGeometryPipelines();
	InitializeOverlayPipelines();
	InitializeLightPipelines();
}

void CD3dPipelineManager::InitializeGeometryPipelines()
{
	SamplerDesc DefaultSamplerDesc;
	DefaultSamplerDesc.MaxAnisotropy = 8;

	SamplerDesc CubeMapSamplerDesc = DefaultSamplerDesc;

	
	m_pDefaultRootSignature->Reset(My::RootBindings::kNumRootBindings, 2); 	//使用2个采样器
	(*m_pDefaultRootSignature)[My::kMeshConstants].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	(*m_pDefaultRootSignature)[My::kMaterialConstants].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	(*m_pDefaultRootSignature)[My::kMaterialSRVs].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 20, D3D12_SHADER_VISIBILITY_PIXEL);
	(*m_pDefaultRootSignature)[My::kMaterialSamplers].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 0, 10, D3D12_SHADER_VISIBILITY_PIXEL);
	(*m_pDefaultRootSignature)[My::kCommonSRVs].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 20, 15, D3D12_SHADER_VISIBILITY_PIXEL);
	(*m_pDefaultRootSignature)[My::kCommonBatchConstantsCBV].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_ALL);
	(*m_pDefaultRootSignature)[My::kCommonFrameConstantsCBV].InitAsConstantBuffer(2, D3D12_SHADER_VISIBILITY_ALL);
	(*m_pDefaultRootSignature)[My::kCommonLightConstantsCBV].InitAsConstantBuffer(3, D3D12_SHADER_VISIBILITY_ALL);
	(*m_pDefaultRootSignature)[My::kShadowSRVs].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 100, MAX_LIGHT_NUM, D3D12_SHADER_VISIBILITY_PIXEL);
	//(*m_pDefaultRootSignature)[kSkinMatrices].InitAsBufferSRV(20, D3D12_SHADER_VISIBILITY_VERTEX);
	m_pDefaultRootSignature->InitStaticSampler(10, DefaultSamplerDesc);
	//m_pDefaultRootSignature->InitStaticSampler(11, ShadowMapSamplerDesc);
	m_pDefaultRootSignature->InitStaticSampler(12, CubeMapSamplerDesc);
	m_pDefaultRootSignature->Finalize(L"TemplateRootSig", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	std::unique_ptr<CD3dPipeline> pDefaultPSO = std::make_unique<CD3dPipeline>(L"Default PSO");
	pDefaultPSO->SetRootSignature(m_pDefaultRootSignature);
	pDefaultPSO->GetPSO()->SetRootSignature(*m_pDefaultRootSignature);
	pDefaultPSO->GetPSO()->SetRasterizerState(RasterizerDefault);
	pDefaultPSO->GetPSO()->SetBlendState(BlendDisable);
	pDefaultPSO->GetPSO()->SetDepthStencilState(DepthStateReadWriteLess);
	pDefaultPSO->GetPSO()->SetRenderTargetFormat(g_SceneColorBufferFormat, g_SceneDepthBufferFormat);
	pDefaultPSO->GetPSO()->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pDefaultPSO->GetPSO()->SetInputLayout(3, g_PosNorTex);
	SetShaderByteCode(*pDefaultPSO->GetPSO(), "Model");
	pDefaultPSO->GetPSO()->Finalize();

	//SkyBox
	std::unique_ptr<CD3dPipeline> pSkyBoxPSO = std::make_unique<CD3dPipeline>(L"SkyBox PSO");
	auto RasterizerSkyBox = RasterizerDefault;
	RasterizerSkyBox.CullMode = D3D12_CULL_MODE_NONE;
	pSkyBoxPSO->GetPSO()->SetRootSignature(*pDefaultPSO->GetRootSignature());
	pSkyBoxPSO->GetPSO()->SetRasterizerState(RasterizerSkyBox);
	pSkyBoxPSO->GetPSO()->SetDepthStencilState(DepthStateReadWriteLess);
	pSkyBoxPSO->GetPSO()->SetBlendState(BlendDisable);
	pSkyBoxPSO->GetPSO()->SetInputLayout(0, nullptr);
	SetShaderByteCode(*pSkyBoxPSO->GetPSO(), "SkyBox");
	pSkyBoxPSO->GetPSO()->SetRenderTargetFormat(g_SceneColorBufferFormat, DSV_FORMAT);
	pSkyBoxPSO->GetPSO()->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pSkyBoxPSO->GetPSO()->Finalize();

	
	m_GraphicsPSOMap.emplace("Default", std::move(pDefaultPSO));
	m_GraphicsPSOMap.emplace("Skybox", std::move(pSkyBoxPSO));
}

void CD3dPipelineManager::InitializeOverlayPipelines()
{
	SamplerDesc DefaultSamplerDesc;
	DefaultSamplerDesc.MaxAnisotropy = 8;


	// Present
	std::unique_ptr<CD3dPipeline> pPresentPSO = std::make_unique<CD3dPipeline>(L"Present PSO");

	pPresentPSO->GetRootSignature()->Reset(1, 2);
	(*pPresentPSO->GetRootSignature())[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	pPresentPSO->GetRootSignature()->InitStaticSampler(16, g_linearWarp);
	pPresentPSO->GetRootSignature()->InitStaticSampler(17, g_linearClamp);
	pPresentPSO->GetRootSignature()->Finalize(L"PresentRootSig");

	pPresentPSO->GetPSO()->SetRootSignature(*pPresentPSO->GetRootSignature());
	pPresentPSO->GetPSO()->SetRasterizerState(RasterizerDefault);
	pPresentPSO->GetPSO()->SetBlendState(BlendDisable);
	pPresentPSO->GetPSO()->SetDepthStencilState(DepthStateDisabled);
	pPresentPSO->GetPSO()->SetSampleMask(0xFFFFFFFF);
	pPresentPSO->GetPSO()->SetInputLayout(0, nullptr);
	SetShaderByteCode(*pPresentPSO->GetPSO(), "Present");
	pPresentPSO->GetPSO()->SetRenderTargetFormat(DXGI_FORMAT_R10G10B10A2_UNORM, DXGI_FORMAT_UNKNOWN);		// RT是DisplayBuffer
	pPresentPSO->GetPSO()->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pPresentPSO->GetPSO()->Finalize();


	// WaterDrops Sub
	std::unique_ptr<CD3dPipeline> pWaterDropsSubPSO = std::make_unique<CD3dPipeline>(L"WaterDropsSub PSO");
	pWaterDropsSubPSO->GetRootSignature()->Reset(My::kWaterDropsRootBindings, 6);
	(*pWaterDropsSubPSO->GetRootSignature())[My::kWaterDropsSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*pWaterDropsSubPSO->GetRootSignature())[My::kWaterDropsCBV].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	pWaterDropsSubPSO->GetRootSignature()->InitStaticSampler(16, g_linearWarp);
	pWaterDropsSubPSO->GetRootSignature()->InitStaticSampler(17, g_linearClamp);
	pWaterDropsSubPSO->GetRootSignature()->InitStaticSampler(18, g_pointWarp);
	pWaterDropsSubPSO->GetRootSignature()->InitStaticSampler(19, g_pointClamp);
	pWaterDropsSubPSO->GetRootSignature()->InitStaticSampler(20, g_anisotropicWarp);
	pWaterDropsSubPSO->GetRootSignature()->InitStaticSampler(21, g_anisotropicClamp);
	pWaterDropsSubPSO->GetRootSignature()->Finalize(L"WaterDropsSubRootSig");

	pWaterDropsSubPSO->GetPSO()->SetRootSignature(*pWaterDropsSubPSO->GetRootSignature());
	pWaterDropsSubPSO->GetPSO()->SetRasterizerState(RasterizerDefault);
	pWaterDropsSubPSO->GetPSO()->SetBlendState(BlendDisable);
	pWaterDropsSubPSO->GetPSO()->SetDepthStencilState(DepthStateDisabled);
	pWaterDropsSubPSO->GetPSO()->SetSampleMask(0xFFFFFFFF);
	pWaterDropsSubPSO->GetPSO()->SetInputLayout(0, nullptr);
	SetShaderByteCode(*pWaterDropsSubPSO->GetPSO(), "WaterDropsSub");
	pWaterDropsSubPSO->GetPSO()->SetRenderTargetFormat(g_SceneColorBufferFormat, DXGI_FORMAT_UNKNOWN);	// RT是ColorBuffer
	pWaterDropsSubPSO->GetPSO()->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pWaterDropsSubPSO->GetPSO()->Finalize();


	// Sea Sub
	std::unique_ptr<CD3dPipeline> pSeaSubPSO = std::make_unique<CD3dPipeline>(L"SeaSub PSO");
	pSeaSubPSO->GetRootSignature()->Reset(My::kSeaRootBindings);
	(*pSeaSubPSO->GetRootSignature())[My::kSeaSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*pSeaSubPSO->GetRootSignature())[My::kSeaCBV].InitAsConstantBuffer(1);
	pSeaSubPSO->GetRootSignature()->Finalize(L"SeaSubRootSig");

	pSeaSubPSO->GetPSO()->SetRootSignature(*pSeaSubPSO->GetRootSignature());
	pSeaSubPSO->GetPSO()->SetRasterizerState(RasterizerDefault);
	pSeaSubPSO->GetPSO()->SetBlendState(BlendDisable);
	pSeaSubPSO->GetPSO()->SetDepthStencilState(DepthStateDisabled);
	pSeaSubPSO->GetPSO()->SetSampleMask(0xFFFFFFFF);
	pSeaSubPSO->GetPSO()->SetInputLayout(0, nullptr);
	SetShaderByteCode(*pSeaSubPSO->GetPSO(), "SeaSub");
	pSeaSubPSO->GetPSO()->SetRenderTargetFormat(g_SceneColorBufferFormat, DXGI_FORMAT_UNKNOWN);	// RT是ColorBuffer
	pSeaSubPSO->GetPSO()->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pSeaSubPSO->GetPSO()->Finalize();


	// Volumetric Light Sub
	std::unique_ptr<CD3dPipeline> pVolumetricLightPSO = std::make_unique<CD3dPipeline>(L"VolumetricLight PSO");
	pVolumetricLightPSO->GetRootSignature()->Reset(My::kVolumetricLightRootBindings, 2);
	(*pVolumetricLightPSO->GetRootSignature())[My::kCameraDepthSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	(*pVolumetricLightPSO->GetRootSignature())[My::kLightDepthSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, MAX_LIGHT_NUM, D3D12_SHADER_VISIBILITY_PIXEL);
	(*pVolumetricLightPSO->GetRootSignature())[My::kPresentSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 101, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	(*pVolumetricLightPSO->GetRootSignature())[My::kLightCBV].InitAsConstantBuffer(0);
	(*pVolumetricLightPSO->GetRootSignature())[My::kVolumnCBV].InitAsConstantBuffer(1);
	pVolumetricLightPSO->GetRootSignature()->InitStaticSampler(16, g_linearWarp);
	pVolumetricLightPSO->GetRootSignature()->InitStaticSampler(17, g_linearClamp);
	pVolumetricLightPSO->GetRootSignature()->Finalize(L"VolumetricLightSubRootSig");

	pVolumetricLightPSO->GetPSO()->SetRootSignature(*pVolumetricLightPSO->GetRootSignature());
	pVolumetricLightPSO->GetPSO()->SetRasterizerState(RasterizerDefault);
	pVolumetricLightPSO->GetPSO()->SetBlendState(BlendDisable);
	pVolumetricLightPSO->GetPSO()->SetDepthStencilState(DepthStateDisabled);
	pVolumetricLightPSO->GetPSO()->SetSampleMask(0xFFFFFFFF);
	pVolumetricLightPSO->GetPSO()->SetInputLayout(0, nullptr);
	SetShaderByteCode(*pVolumetricLightPSO->GetPSO(), "VolumetricLight");
	pVolumetricLightPSO->GetPSO()->SetRenderTargetFormat(g_SceneColorBufferFormat, DXGI_FORMAT_UNKNOWN);	// RT是ColorBuffer
	pVolumetricLightPSO->GetPSO()->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pVolumetricLightPSO->GetPSO()->Finalize();


	m_GraphicsPSOMap.emplace("Present", std::move(pPresentPSO));
	m_GraphicsPSOMap.emplace("WaterDrops", std::move(pWaterDropsSubPSO));
	m_GraphicsPSOMap.emplace("Sea", std::move(pSeaSubPSO));
	m_GraphicsPSOMap.emplace("VolumetricLight", std::move(pVolumetricLightPSO));


	// Compute RootSignature and PSOs
	std::unique_ptr<CD3dPipeline> GaussBlurPSO = std::make_unique<CD3dPipeline>(L"GaussBlur PSO", true);
	GaussBlurPSO->GetRootSignature()->Reset(My::kComputeRootBindings - 1);
	(*GaussBlurPSO->GetRootSignature())[My::kComputeSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1);
	(*GaussBlurPSO->GetRootSignature())[My::kComputeUAV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1);
	GaussBlurPSO->GetRootSignature()->Finalize(L"Gauss Blur");

	GaussBlurPSO->GetComputePSO()->SetRootSignature(*GaussBlurPSO->GetRootSignature());
	D3D12_SHADER_BYTECODE GaussBlurShader = CD3DX12_SHADER_BYTECODE();
	GaussBlurShader.pShaderBytecode = m_ShaderByteMap["MyGaussBlurCS"].pShaderByteCode;
	GaussBlurShader.BytecodeLength = m_ShaderByteMap["MyGaussBlurCS"].size;
	GaussBlurPSO->GetComputePSO()->SetComputeShader(GaussBlurShader);
	GaussBlurPSO->GetComputePSO()->Finalize();
	m_ComputePSOMap.emplace("GaussBlur_CS", std::move(GaussBlurPSO));
}

void CD3dPipelineManager::InitializeLightPipelines()
{
	SamplerDesc DefaultSamplerDesc;
	DefaultSamplerDesc.MaxAnisotropy = 8;

	//Spot light
	std::unique_ptr<CD3dPipeline> pSpotLightPSO = std::make_unique<CD3dPipeline>(L"SpotLight PSO");
	pSpotLightPSO->GetRootSignature()->Reset(My::kShadowRootBindings, 1);
	(*pSpotLightPSO->GetRootSignature())[My::kShadowBatchCBV].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_ALL);
	(*pSpotLightPSO->GetRootSignature())[My::kShadowFrameCBV].InitAsConstantBuffer(2, D3D12_SHADER_VISIBILITY_ALL);
	(*pSpotLightPSO->GetRootSignature())[My::kShadowSRV].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	pSpotLightPSO->GetRootSignature()->InitStaticSampler(10, DefaultSamplerDesc);
	pSpotLightPSO->GetRootSignature()->Finalize(L"SpotLightRootSig", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	pSpotLightPSO->GetPSO()->SetRootSignature(*pSpotLightPSO->GetRootSignature());
	pSpotLightPSO->GetPSO()->SetRasterizerState(RasterizerDefault);
	pSpotLightPSO->GetPSO()->SetBlendState(BlendDisable);
	pSpotLightPSO->GetPSO()->SetDepthStencilState(DepthStateReadWriteLess);
	pSpotLightPSO->GetPSO()->SetSampleMask(0xFFFFFFFF);
	pSpotLightPSO->GetPSO()->SetInputLayout(3, g_PosNorTex);
	SetShaderByteCode(*pSpotLightPSO->GetPSO(), "SpotLight");
	pSpotLightPSO->GetPSO()->SetRenderTargetFormat(g_SceneColorBufferFormat, DSV_FORMAT);	// RT是ColorBuffer
	pSpotLightPSO->GetPSO()->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pSpotLightPSO->GetPSO()->Finalize();

	m_GraphicsPSOMap.emplace("SpotLight", std::move(pSpotLightPSO));

	//lighting shading model
	AddLightingShaders();
}

void CD3dPipelineManager::FinalizePipelineTemplates()
{
	for (auto& _it : m_GraphicsPSOMap) {
		_it.second->GetPSO()->DestroyAll();
		_it.second.reset();
	}
	for (auto& _it : m_ComputePSOMap) {
		_it.second->GetComputePSO()->DestroyAll();
		_it.second.reset();
	}
}

//void CD3dPipelineManager::SetPipelineSettings(D3dGraphicsCore::GraphicsPSO& PSO, const int& InputLayoutType, const My::PrimitiveType& PrimitiveType, const std::string& Name)
//{
//	return;
//	std::wstring wName = My::UTF8ToWideString(Name);
//	size_t size = wName.size() + 1;
//	const wchar_t* LName = wName.c_str();
//	wchar_t* newname = new wchar_t[wName.size()];
//	wcscpy(newname, LName);
//	PSO = GraphicsPSO(newname);
//	PSO.SetRootSignature(g_TemplateRootSignature);
//	PSO.SetRasterizerState(RasterizerDefault);
//	PSO.SetBlendState(BlendDisable);
//	PSO.SetDepthStencilState(DepthStateReadWriteLess);
//
//	//PrimitiveType
//	D3D12_PRIMITIVE_TOPOLOGY_TYPE d3dType;
//	switch (PrimitiveType) {
//	case My::kPrimitiveTypeLineStrip:
//	{
//		d3dType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
//	}
//	break;
//	case My::kPrimitiveTypeTriList:
//	{
//		d3dType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//	}
//	break;
//	default:
//		ASSERT(false, "ERROR! No Configured Primitive Type!");
//		break;
//	}
//
//	PSO.SetPrimitiveTopologyType(d3dType);
//
//	//InputLayoutType
//	if (InputLayoutType == (1 << My::kPos)) {
//		PSO.SetInputLayout(1, g_Pos);
//	}
//	else if (InputLayoutType == ((1 << My::kPos) + (1 << My::kTexcoord0))) {
//		PSO.SetInputLayout(2, g_PosTex);
//	}
//	else if (InputLayoutType == ((1 << My::kPos) + (1 << My::kNormal) + (1 << My::kTexcoord0))) {
//		PSO.SetInputLayout(3, g_PosNorTex);
//	}
//	else if (InputLayoutType == ((1 << My::kPos) + (1 << My::kNormal) + (1 << My::kTangent) + (1 << My::kTexcoord0))) {
//		PSO.SetInputLayout(4, g_PosNorTanTex);
//	}
//	else if (InputLayoutType == ((1 << My::kPos) + (1 << My::kColor0))) {
//		PSO.SetInputLayout(2, g_PosColor);
//	}
//	else {
//		ASSERT(false, "InputLayout Type ERROR!");
//	}
//}

void CD3dPipelineManager::AddLightingShaders()
{
	std::unique_ptr<CD3dPipeline> pLambertLighting = std::make_unique<CD3dPipeline>(L"LambertGouraudLighting");
	pLambertLighting->GetPSO()->SetRootSignature(*m_pDefaultRootSignature);
	pLambertLighting->GetPSO()->SetRasterizerState(RasterizerDefault);
	pLambertLighting->GetPSO()->SetBlendState(BlendDisable);
	pLambertLighting->GetPSO()->SetDepthStencilState(DepthStateReadWriteLess);
	pLambertLighting->GetPSO()->SetRenderTargetFormat(g_SceneColorBufferFormat, g_SceneDepthBufferFormat);
	pLambertLighting->GetPSO()->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pLambertLighting->GetPSO()->SetInputLayout(3, g_PosNorTex);
	SetShaderByteCode(*pLambertLighting->GetPSO(), "LambertGouraudLighting");
	pLambertLighting->GetPSO()->Finalize();
	m_GraphicsPSOMap.emplace("LambertGouraudLighting", std::move(pLambertLighting));

	std::unique_ptr<CD3dPipeline> pBlinnPhongLighting = std::make_unique<CD3dPipeline>(L"BlinnPhongLighting");
	pBlinnPhongLighting->GetPSO()->SetRootSignature(*m_pDefaultRootSignature);
	pBlinnPhongLighting->GetPSO()->SetRasterizerState(RasterizerDefault);
	pBlinnPhongLighting->GetPSO()->SetBlendState(BlendDisable);
	pBlinnPhongLighting->GetPSO()->SetDepthStencilState(DepthStateReadWriteLess);
	pBlinnPhongLighting->GetPSO()->SetRenderTargetFormat(g_SceneColorBufferFormat, g_SceneDepthBufferFormat);
	pBlinnPhongLighting->GetPSO()->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pBlinnPhongLighting->GetPSO()->SetInputLayout(3, g_PosNorTex);
	SetShaderByteCode(*pBlinnPhongLighting->GetPSO(), "BlinnPhongLighting");
	pBlinnPhongLighting->GetPSO()->Finalize();
	m_GraphicsPSOMap.emplace("BlinnPhongLighting", std::move(pBlinnPhongLighting));
}

