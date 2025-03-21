#include "D3d12RHI.h"
#include "Core/D3dGraphicsCoreManager.h"
#include "Core/Pipeline/RootSignature.h"
#include "Core/Pipeline/PipelineState.h"
#include "Core/Resource/Texture.h"
#include "Core/Pipeline/DescriptorHeap.h"
#include "Core/D3dCommonDef.h"
#include "Core/Resource/GpuBuffer.h"
#include "Core/Command/CommandContext.h"
#include "Core/Common/SystemTime.h"
#include "Core/Resource/DDSTextureLoader.h"
#include "MemoryManager.hpp"
#include "GraphicsStructure.h"
#include "Modules/ShaderSource/ShaderSource.h"
#include "D3dComponents/XMInput/XMInput.h"
#include "cbuffer.h"
#include "WinUtility.h"

#include "imgui_impl_dx12.h"
#include "imgui.h"
#include <array>
#include <filesystem>
#include <algorithm>


using namespace DirectX;

D3dGraphicsCore::D3d12RHI::D3d12RHI()
{
    
}

D3dGraphicsCore::D3d12RHI::~D3d12RHI()
{
#if 0
    // for dxgi debug,to solve memory leak 
    IDXGIDebug1* dxgiDebug;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
    {
        dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
        dxgiDebug->Release();
    }
#endif
}

int D3dGraphicsCore::D3d12RHI::StartUp()
{
    InitializeCoreHWND();
    InitializeInputLayout();
    InitializeSamplers();
    InitializeShaderByteMap();
    InitializeBaseDescriptorHeap();
    InitializePipelineTemplates();

    SystemTime::Initialize();
    XM_Input::Initialize();
    InitializeGraphicsSettings();
    
    InitializeDefaultTexture();

    return 0;
}

void D3dGraphicsCore::D3d12RHI::Finalize()
{
    ShutdownDisplay();
    DestroyCommonState();

    FinalizePipelineTemplates();
    FinalizeBaseDescriptorHeap();
    FinalizeShaderByteMap();
    FinalizeDefaultTexture();

    XM_Input::Shutdown();
    Shutdown();
}

void D3dGraphicsCore::D3d12RHI::Resize(uint32_t width, uint32_t height)
{
    DisplayResize(width, height);
    m_Camera.reset();
    m_CameraController.reset();
    InitializeGraphicsSettings();
}

void D3dGraphicsCore::D3d12RHI::InitializeCoreHWND()
{
    HWND hwnd;
    uint32_t width, height;
    m_fQueryFrameBufferSize(width, height);
    hwnd = m_fGetWindowHandleProc();
    g_hWnd = hwnd;
    g_DisplayWidth = width;
    g_DisplayHeight = height;

    Initialize(false);
    InitializeBuffers();
}

void D3dGraphicsCore::D3d12RHI::InitializeGraphicsSettings()
{
    if (!m_Camera) {
        m_Camera = std::make_unique<XM_Camera::Camera>();
        m_Camera->SetAspectRatio((float)g_DisplayHeight / (float)g_DisplayWidth);
        m_Camera->SetFOV(120.f);
        m_Camera->SetZRange(0.1f, 1000.0f);
        m_Camera->SetPosition(XM_Math::Vector3(0, 5, 150));
    }
    if (!m_CameraController) {
        m_CameraController = std::make_unique<XM_Camera::FlyingFPSCamera>(*m_Camera.get(), XM_Math::Vector3(0.0f, 1.0f, 0.0f));
    }
    
    m_MainViewport.Width = g_DisplayWidth;
    m_MainViewport.Height = g_DisplayHeight;
    
    /*
    * 这里minDepth为0.0，maxDepth为1.0，因为深度测试。
    */
    m_MainViewport.MinDepth = 0.0f;
    m_MainViewport.MaxDepth = 1.0f;
    m_MainViewport.TopLeftX = 0.0f;
    m_MainViewport.TopLeftY = 0.0f;

    m_MainScissor.left = 0;
    m_MainScissor.top = 0;
    m_MainScissor.right = g_DisplayWidth;
    m_MainScissor.bottom = g_DisplayHeight;
}

void D3dGraphicsCore::D3d12RHI::FinalizeGraphicsSettings()
{
    m_CameraController.reset(nullptr);
}

void D3dGraphicsCore::D3d12RHI::SetLightManager(My::D3d12LightManager* pLightManager)
{
    m_pLightManager = pLightManager;
}

void D3dGraphicsCore::D3d12RHI::UpdateCamera()
{
    float DeltaTime = GetFrameTime();
    XM_Input::Update(DeltaTime);
    m_CameraController->Update(DeltaTime);
    
#ifdef _DEBUG
    if (m_Camera->m_bIsCameraMove) {
        std::cout << "Current Camera Position: X: " << m_Camera->GetPosition().GetX()
            << " Y:" << m_Camera->GetPosition().GetY()
            << " Z:" << m_Camera->GetPosition().GetZ()
            << std::endl;
    }
#endif // _DEBUG
}

void D3dGraphicsCore::D3d12RHI::UpdateCameraParams(int64_t key)
{
    switch (key)
    {
    case VK_PRIOR:
        m_CameraController->ApplyCameraParams(XM_Camera::FlyingFPSCamera::SpeedUp);
        break;
    case VK_NEXT:
        m_CameraController->ApplyCameraParams(XM_Camera::FlyingFPSCamera::SpeedDown);
        break;
    default:
        break;
    }

    switch (key)
    {
    case 0x52:
    {
        //reset cameta position
        XM_Math::Vector3 position = XM_Math::Vector3(0.0f, 0.0f, 5.0f);
        XM_Math::Matrix3 orientation = XM_Math::Matrix3(XM_Math::Vector3(1, 0, 0), XM_Math::Vector3(0, 1, 0), XM_Math::Vector3(0, 0, 1));
        m_Camera->SetTransform(XM_Math::AffineTransform(orientation, position));
        m_Camera->Update();
    }
    break;
    default:
        break;
    }
}

void D3dGraphicsCore::D3d12RHI::UpdatePresent()
{
    g_CommandManager.IdleGPU();
    D3dGraphicsCore::Present();

    g_CommandManager.IdleGPU();
    InitializeBuffers();
}

void D3dGraphicsCore::D3d12RHI::SetPrimitiveType(GraphicsContext& context, My::PrimitiveType Type)
{
    D3D_PRIMITIVE_TOPOLOGY d3dType;
    switch (Type) {
    case My::kPrimitiveTypeLineStrip:
    {
        d3dType = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    }
    break;
    case My::kPrimitiveTypeTriList:
    {
        d3dType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
    break;
    default:
        ASSERT(false, "ERROR! No Configured Primitive Type!");
        std::cout << "[PrimitiveType ERROR] No Configured Primitive Type!" << std::endl;
        break;
    }

    context.SetPrimitiveTopology(d3dType);
}

void D3dGraphicsCore::D3d12RHI::BeginSubPass(const std::string& PassName)
{
    m_pComputeContext = nullptr;
    m_pGraphicsContext = nullptr;
    m_pComputePSO = nullptr;
    m_pGraphicsPSO = nullptr;

    std::wstring wPassName = L"";
#ifdef _DEBUG
    wPassName = My::UTF8ToWideString(PassName);
#endif
    if (PassName.substr(PassName.size() - 3, 3) == "_CS") {
        ComputeContext& csContext = ComputeContext::Begin(wPassName);
        m_pComputeContext = & csContext;
        return;
    }

    GraphicsContext& gfxContext = GraphicsContext::Begin(wPassName);
    m_pGraphicsContext = &gfxContext;
}

void D3dGraphicsCore::D3d12RHI::EndSubPass()
{
    if (m_pComputeContext) {
        m_pComputeContext->Finish();
        return;
    }
    m_pGraphicsContext->TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_PRESENT, true);
    m_pGraphicsContext->Finish();
}

void D3dGraphicsCore::D3d12RHI::BeginOverlayPass(ColorBuffer& result, ColorBuffer& src)
{
    ComputeContext& BeginOverlayContext = ComputeContext::Begin(L"BeginOverlay");
    //BeginOverlayContext.TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
    //BeginOverlayContext.TransitionResource(result, D3D12_RESOURCE_STATE_COPY_DEST);
    BeginOverlayContext.CopyBuffer(result, src);
    BeginOverlayContext.Finish();
}

void D3dGraphicsCore::D3d12RHI::EndOverlayPass(ColorBuffer& result, ColorBuffer& src)
{
    ComputeContext& EndOverlayContext = ComputeContext::Begin(L"EndOverlay");
    //EndOverlayContext.TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
    //EndOverlayContext.TransitionResource(result, D3D12_RESOURCE_STATE_COPY_DEST);
    EndOverlayContext.CopyBuffer(result, src);
    EndOverlayContext.Finish();
}

void D3dGraphicsCore::D3d12RHI::SetPipelineStatus(const std::string& PSOName)
{
    if (PSOName.substr(PSOName.size() - 3, 3) == "_CS") {
        if (g_ComputePSOMap.find(PSOName) != g_ComputePSOMap.end()) {
            m_pComputePSO = g_ComputePSOMap[PSOName].get();
            m_pRootSignature = m_pComputePSO->GetRootSignaturePtr();
        }
        else {
            ASSERT(false, "InValid Compute Pipeline Status Name!");
            std::cout << "[PSO ERROR] InValid Compute Pipeline Status Name!" << std::endl;
            m_pComputePSO = nullptr;
        }
        return;
    }

    if (g_PipelineStatusMap.find(PSOName) != g_PipelineStatusMap.end()) {
        m_pGraphicsPSO = g_PipelineStatusMap[PSOName].get();
        m_pRootSignature = m_pGraphicsPSO->GetRootSignaturePtr();
    }
    else {
        ASSERT(false, "InValid Pipeline Status Name!");
        std::cout << "[PSO ERROR] InValid Pipeline Status Name!" << std::endl;
        m_pGraphicsPSO = g_PipelineStatusMap["Default"].get();
        m_pRootSignature = g_PipelineStatusMap["Default"]->GetRootSignaturePtr();
    }
}

void D3dGraphicsCore::D3d12RHI::UpdateCameraConstants(My::Frame& frame)
{
    UpdateCamera();
    memcpy(&frame.FrameContext.ProjectionMatrix, &m_Camera->GetProjMatrix(), sizeof(float) * 16);
    memcpy(&frame.FrameContext.ViewMatrix, &m_Camera->GetViewMatrix(), sizeof(float) * 16);
    My::Vector4f CameraPos(m_Camera->GetPosition().GetX(), m_Camera->GetPosition().GetY(), m_Camera->GetPosition().GetZ(), 1);
    memcpy(&frame.FrameContext.CameraPosition, &CameraPos, sizeof(float) * 4);
}

void D3dGraphicsCore::D3d12RHI::SetBatchResources()
{
    m_pGraphicsContext->TransitionResource(g_DepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
    m_pGraphicsContext->TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
    m_pGraphicsContext->SetViewportAndScissor(m_MainViewport, m_MainScissor);
    m_pGraphicsContext->SetRenderTarget(g_SceneColorBuffer.GetRTV(), g_DepthBuffer.GetDSV());
    m_pGraphicsContext->ClearColor(g_SceneColorBuffer);
    m_pGraphicsContext->ClearDepth(g_DepthBuffer);
}

void D3dGraphicsCore::D3d12RHI::SetShadowResources(My::Frame& frame, uint8_t lightIdx, ColorBuffer* colorBuffer, DepthBuffer* depthBuffer, ColorBuffer* volumnBuffer)
{
    m_pGraphicsContext->SetViewportAndScissor(m_MainViewport, m_MainScissor);

    m_pGraphicsContext->TransitionResource(*depthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
    m_pGraphicsContext->TransitionResource(*colorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);

    if (m_pLightManager->GetCastVolumetric(lightIdx) && volumnBuffer) {
        m_pGraphicsContext->TransitionResource(*volumnBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
        D3D12_CPU_DESCRIPTOR_HANDLE rts[] = {
            colorBuffer->GetRTV(),
            volumnBuffer->GetRTV()
        };
        m_pGraphicsContext->SetRenderTargets(2, rts, depthBuffer->GetDSV());
        m_pGraphicsContext->ClearColor(*volumnBuffer);
    }
    else {
        m_pGraphicsContext->SetRenderTarget(colorBuffer->GetRTV(), depthBuffer->GetDSV());
    }
    
    m_pGraphicsContext->ClearColor(*colorBuffer);
    m_pGraphicsContext->ClearDepth(*depthBuffer);
}

void D3dGraphicsCore::D3d12RHI::TransitionResourceState(GpuResource& re, D3D12_RESOURCE_STATES state, bool flush)
{
    m_pGraphicsContext->TransitionResource(re, state, flush);
}

void D3dGraphicsCore::D3d12RHI::DrawBatch(const My::Frame& frame, const My::D3dDrawBatchContext* pdbc, StructuredBuffer* vbuffer, ByteAddressBuffer* ibuffer,
    const int TextureHeapIndex, const DescriptorHandle& TextureHandle, ID3D12DescriptorHeap* IBLHeapPtr, DescriptorHandle IBLHandle, 
    std::unique_ptr<My::D3d12LightManager>& pLightManager, uint8_t lightIdx, bool bShadowCast, bool isDrawSkybox)
{
    m_pGraphicsContext->SetRootSignature(*m_pRootSignature);
    m_pGraphicsContext->SetPipelineState(*m_pGraphicsPSO);

    SetPrimitiveType(*m_pGraphicsContext, pdbc->m_PrimitiveType);

    m_pGraphicsContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_BaseDescriptorHeap[TextureHeapIndex].GetHeapPointer());

    if(!bShadowCast)
    {
        My::MaterialConstants MatCbv;
        memset(&MatCbv, 0, sizeof(My::MaterialConstants));
        // factor param
        MatCbv.BaseColorFactor[0] = pdbc->BaseColorFactor[0];
        MatCbv.BaseColorFactor[1] = pdbc->BaseColorFactor[1];
        MatCbv.BaseColorFactor[2] = pdbc->BaseColorFactor[2];
        MatCbv.BaseColorFactor[3] = pdbc->BaseColorFactor[3];
        MatCbv.MetallicRoughnessFactor[0] = pdbc->MetallicRoughnessFactor[0];
        MatCbv.MetallicRoughnessFactor[1] = pdbc->MetallicRoughnessFactor[1];
        MatCbv.EmissiveFactor[0] = pdbc->EmissiveFactor[0];
        MatCbv.EmissiveFactor[1] = pdbc->EmissiveFactor[1];
        MatCbv.EmissiveFactor[2] = pdbc->EmissiveFactor[2];
        MatCbv.NormalTextureScale = pdbc->NormalTextureScale;

        // transform param
        MatCbv.BaseColorTextureTransform[0] = pdbc->BaseColorTextureTransform[0];
        MatCbv.BaseColorTextureTransform[1] = pdbc->BaseColorTextureTransform[1];
        MatCbv.BaseColorTextureTransform[2] = pdbc->BaseColorTextureTransform[2];
        MatCbv.BaseColorTextureTransform[3] = pdbc->BaseColorTextureTransform[3];
        MatCbv.BaseColorRotation = pdbc->BaseColorRotation;

        MatCbv.MetallicRoughnessTextureTransform[0] = pdbc->MetallicRoughnessTextureTransform[0];
        MatCbv.MetallicRoughnessTextureTransform[1] = pdbc->MetallicRoughnessTextureTransform[1];
        MatCbv.MetallicRoughnessTextureTransform[2] = pdbc->MetallicRoughnessTextureTransform[2];
        MatCbv.MetallicRoughnessTextureTransform[3] = pdbc->MetallicRoughnessTextureTransform[3];
        MatCbv.MetallicRoughnessRotation = pdbc->MetallicRoughnessRotation;

        MatCbv.OcclusionTextureTransform[0] = pdbc->OcclusionTextureTransform[0];
        MatCbv.OcclusionTextureTransform[1] = pdbc->OcclusionTextureTransform[1];
        MatCbv.OcclusionTextureTransform[2] = pdbc->OcclusionTextureTransform[2];
        MatCbv.OcclusionTextureTransform[3] = pdbc->OcclusionTextureTransform[3];
        MatCbv.OcclusionRotation = pdbc->OcclusionRotation;

        MatCbv.EmissiveTextureTransform[0] = pdbc->EmissiveTextureTransform[0];
        MatCbv.EmissiveTextureTransform[1] = pdbc->EmissiveTextureTransform[1];
        MatCbv.EmissiveTextureTransform[2] = pdbc->EmissiveTextureTransform[2];
        MatCbv.EmissiveTextureTransform[3] = pdbc->EmissiveTextureTransform[3];
        MatCbv.EmissiveRotation = pdbc->EmissiveRotation;

        MatCbv.NormalTextureTransform[0] = pdbc->NormalTextureTransform[0];
        MatCbv.NormalTextureTransform[1] = pdbc->NormalTextureTransform[1];
        MatCbv.NormalTextureTransform[2] = pdbc->NormalTextureTransform[2];
        MatCbv.NormalTextureTransform[3] = pdbc->NormalTextureTransform[3];
        MatCbv.NormalRotation = pdbc->NormalRotation;

        MatCbv.depth_bias = *pLightManager->GetDepthBias();

        m_pGraphicsContext->SetDynamicConstantBufferView(My::kMaterialConstants, sizeof(My::MaterialConstants), &MatCbv);
    }

    if (!bShadowCast)
    {
        m_pGraphicsContext->SetDescriptorTable(My::kMaterialSRVs, TextureHandle);
    }
    
    if (!bShadowCast) {
        My::PerBatchConstants pbc;
        pbc.ModelMatrix = pdbc->ModelMatrix;
        My::PerFrameConstants pfc;
        pfc.ViewMatrix = frame.FrameContext.ViewMatrix;
        pfc.ProjectionMatrix = frame.FrameContext.ProjectionMatrix;
        pfc.CameraPosition = frame.FrameContext.CameraPosition;

        pfc.clip_space_type = 1;
        pfc.LightNum = pLightManager->GetLightNum();
        pfc.screenHeight = g_DisplayHeight;
        pfc.screenWidth = g_DisplayWidth;

        m_pGraphicsContext->SetDynamicConstantBufferView(My::kCommonBatchConstantsCBV, sizeof(My::PerBatchConstants), &pbc);
        m_pGraphicsContext->SetDynamicConstantBufferView(My::kCommonFrameConstantsCBV, sizeof(My::PerFrameConstants), &pfc);
        m_pGraphicsContext->SetDynamicConstantBufferView(My::kCommonLightConstantsCBV, sizeof(My::LightInfo), pLightManager->GetAllLightInfoPtr());
        m_pGraphicsContext->SetDescriptorTable(My::kShadowSRVs, D3D12_GPU_DESCRIPTOR_HANDLE(pLightManager->GetDepthGpuHandle()));
    }
    else {
        auto& lightInfo = pLightManager->GetAllLightInfoPtr()->Lights[lightIdx];

        struct ShadowBatchConstants {
            My::Matrix4X4f modelMatrix;
        } SBC;
        struct ShadowFrameConstants {
            My::Matrix4X4f viewMatrix;
            My::Matrix4X4f projectionMatrix;
            My::Vector4f lightPos;
            float screenWidth;
            float screenHeight;
            float padding0[2];
        } SFC;

        SBC.modelMatrix = pdbc->ModelMatrix;
        SFC.viewMatrix = lightInfo.LightViewMatrix;
        SFC.projectionMatrix = lightInfo.LightProjectionMatrix;
        SFC.lightPos = lightInfo.LightPosition;
        SFC.screenWidth = g_DisplayWidth;
        SFC.screenHeight = g_DisplayHeight;
        SFC.padding0[0] = 0;
        SFC.padding0[1] = 0;
        
        m_pGraphicsContext->SetDynamicConstantBufferView(My::kShadowBatchCBV, sizeof(My::PerBatchConstants), &SBC);
        m_pGraphicsContext->SetDynamicConstantBufferView(My::kShadowFrameCBV, sizeof(My::PerFrameConstants), &SFC);
        m_pGraphicsContext->SetDescriptorTable(My::kShadowSRV, pLightManager->GetDepthSRVDescriptorHandle(lightIdx));
    }

    
    if(isDrawSkybox)
    {
        m_pGraphicsContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, IBLHeapPtr);
        m_pGraphicsContext->SetDescriptorTable(My::kCommonSRVs, IBLHandle);
    }

    m_pGraphicsContext->SetIndexBuffer(ibuffer->IndexBufferView());
    m_pGraphicsContext->SetVertexBuffer(0, vbuffer->VertexBufferView());
    m_pGraphicsContext->DrawIndexedInstanced(pdbc->m_index_count_per_instance, 1, 0, 0, 0);
}

void D3dGraphicsCore::D3d12RHI::DrawSkybox(const My::Frame& frame, ID3D12DescriptorHeap* HeapPtr, DescriptorHandle IBLHandle, GpuTexture* pSpecularTexture
    , float& SpecularIBLRange, float& SpecularIBLBias)
{
    SpecularIBLRange = 0.0f;

    if (pSpecularTexture) {
        ID3D12Resource* texRes = const_cast<ID3D12Resource*>(pSpecularTexture->GetResource());
        const D3D12_RESOURCE_DESC& texDesc = texRes->GetDesc();
        SpecularIBLRange = std::max(0.0f, (float)texDesc.MipLevels - 1);
        SpecularIBLBias = std::min(SpecularIBLBias, SpecularIBLRange);
    }

    __declspec(align(16)) struct SkyboxVSCB
    {
        XM_Math::Matrix4 ProjInverse;
        XM_Math::Matrix3 ViewInverse;
    } skyVSCB;
    skyVSCB.ProjInverse = Invert(m_Camera->GetProjMatrix());
    skyVSCB.ViewInverse = Invert(m_Camera->GetViewMatrix()).Get3x3();

    __declspec(align(16)) struct SkyboxPSCB
    {
        float TextureLevel;
    } skyPSCB;
    skyPSCB.TextureLevel = SpecularIBLBias;

    //Common SRVs for cubemap -- register 10 
    m_pGraphicsContext->SetRootSignature(*m_pRootSignature);
    m_pGraphicsContext->SetPipelineState(*m_pGraphicsPSO);
    m_pGraphicsContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pGraphicsContext->TransitionResource(g_DepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
    m_pGraphicsContext->TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
    m_pGraphicsContext->SetRenderTarget(g_SceneColorBuffer.GetRTV(), g_DepthBuffer.GetDSV_ReadOnly());
    m_pGraphicsContext->SetViewportAndScissor(m_MainViewport, m_MainScissor);

    m_pGraphicsContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, HeapPtr);
    m_pGraphicsContext->SetDynamicConstantBufferView(My::kMeshConstants, sizeof(SkyboxVSCB), &skyVSCB);
    m_pGraphicsContext->SetDynamicConstantBufferView(My::kMaterialConstants, sizeof(SkyboxPSCB), &skyPSCB);
    m_pGraphicsContext->SetDescriptorTable(My::kCommonSRVs, IBLHandle);
    m_pGraphicsContext->Draw(3);
}

void D3dGraphicsCore::D3d12RHI::DrawGui(const My::Frame& frame)
{
    m_pGraphicsContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pGraphicsContext->TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_RENDER_TARGET, true);
    m_pGraphicsContext->SetViewportAndScissor(m_MainViewport, m_MainScissor);
    m_pGraphicsContext->SetRenderTarget(g_DisplayBuffer[g_CurrentBuffer].GetRTV());
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_pGraphicsContext->GetCommandList());
    auto& io = ImGui::GetIO();
    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
    m_pGraphicsContext->TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_PRESENT);
}

void D3dGraphicsCore::D3d12RHI::DrawPresent(const My::Frame& frame, DescriptorHandle ColorBufferHandle, int ColorBufferHeapIndex)
{
    m_pGraphicsContext->SetRootSignature(*m_pRootSignature);
    m_pGraphicsContext->SetPipelineState(*m_pGraphicsPSO);
    m_pGraphicsContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pGraphicsContext->TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    m_pGraphicsContext->TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_RENDER_TARGET, true);
    m_pGraphicsContext->SetViewportAndScissor(m_MainViewport, m_MainScissor);
    m_pGraphicsContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_DescriptorHeaps[ColorBufferHeapIndex]->GetHeapPointer());
    m_pGraphicsContext->SetDescriptorTable(0, ColorBufferHandle);
    m_pGraphicsContext->SetRenderTarget(g_DisplayBuffer[g_CurrentBuffer].GetRTV());
    m_pGraphicsContext->Draw(3);
    m_pGraphicsContext->TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_PRESENT);
}

void D3dGraphicsCore::D3d12RHI::DrawGuassBlur(const My::Frame& frame, ColorBuffer& result, ColorBuffer& src, DescriptorHandle ResultBufferHandle, DescriptorHandle ColorBufferHandle, int ColorBufferHeapIndex)
{
    if (!m_pComputePSO) {
        return;
    }

    m_pComputeContext->TransitionResource(result, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, true);
    
    m_pComputeContext->SetRootSignature(*m_pRootSignature);
    m_pComputeContext->SetPipelineState(*m_pComputePSO);
    m_pComputeContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_DescriptorHeaps[ColorBufferHeapIndex]->GetHeapPointer());
    m_pComputeContext->SetDescriptorTable(My::kComputeSRV, ColorBufferHandle);
    m_pComputeContext->SetDescriptorTable(My::kComputeUAV, ResultBufferHandle);
    m_pComputeContext->Dispatch(g_DisplayWidth, g_DisplayHeight, 1);

    m_pComputeContext->TransitionResource(result, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, true);

    m_pComputeContext->TransitionResource(src, D3D12_RESOURCE_STATE_COPY_DEST);
    m_pComputeContext->TransitionResource(result, D3D12_RESOURCE_STATE_COPY_SOURCE);
    m_pComputeContext->CopyBuffer(src, result);
}

void D3dGraphicsCore::D3d12RHI::DrawOverlay(const My::Frame& frame, ColorBuffer& result, ColorBuffer& src, DescriptorHandle ResultBufferHandle, DescriptorHandle ColorBufferHandle, int ColorBufferHeapIndex)
{
    m_pGraphicsContext->SetRootSignature(*m_pRootSignature);
    m_pGraphicsContext->SetPipelineState(*m_pGraphicsPSO);
    m_pGraphicsContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pGraphicsContext->TransitionResource(src, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    m_pGraphicsContext->TransitionResource(result, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_pGraphicsContext->SetViewportAndScissor(m_MainViewport, m_MainScissor);
    m_pGraphicsContext->SetRenderTarget(result.GetRTV());

    m_pGraphicsContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_DescriptorHeaps[ColorBufferHeapIndex]->GetHeapPointer());
    m_pGraphicsContext->SetDescriptorTable(My::kOverlaySRV, ColorBufferHandle);

    __declspec(align(16)) struct OverlayPSCB
    {
        float ScreenSize[2];
        float Time;
        float padding0;
    } overlayPSCB;

    memset(&overlayPSCB, 0, sizeof(OverlayPSCB));

    overlayPSCB.ScreenSize[0] = g_DisplayWidth;
    overlayPSCB.ScreenSize[1] = g_DisplayHeight;
    overlayPSCB.Time = m_fGetTimestamp();

    m_pGraphicsContext->SetDynamicConstantBufferView(My::kOverlayCBV, sizeof(OverlayPSCB), &overlayPSCB);

    m_pGraphicsContext->Draw(3);

    m_pGraphicsContext->TransitionResource(result, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, true);

    m_pGraphicsContext->TransitionResource(src, D3D12_RESOURCE_STATE_COPY_DEST);
    m_pGraphicsContext->TransitionResource(result, D3D12_RESOURCE_STATE_COPY_SOURCE);
    m_pGraphicsContext->CopyBuffer(src, result);
}

void D3dGraphicsCore::D3d12RHI::DrawVolumetricLight(const My::Frame& frame)
{


    //m_pGraphicsContext->SetDescriptorTable();
}