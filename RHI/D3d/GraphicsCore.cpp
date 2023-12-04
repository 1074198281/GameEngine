#include "GraphicsCore.h"
#include "Core/D3dGraphicsCoreManager.h"
#include "MemoryManager.hpp"
#include "GraphicsStructure.h"
#include "ShaderSource.h"
#include "XMInput/XMInput.h"
#include "Core/Common/SystemTime.h"
#include <array>


using namespace DirectX;

D3dGraphicsCore::CD3dGraphicsCore::CD3dGraphicsCore()
{

}

D3dGraphicsCore::CD3dGraphicsCore::~CD3dGraphicsCore()
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

int D3dGraphicsCore::CD3dGraphicsCore::StartUp()
{
    InitializeInputLayout();
    InitializeSamplers();
    InitializeShaderByteMap();
    InitializeBaseDescriptorHeap();
    InitializePipelineTemplates();

    SystemTime::Initialize();
    XM_Input::Initialize();
    InitializeGraphicsSettings();
    
    return 0;
}

void D3dGraphicsCore::CD3dGraphicsCore::Finalize()
{
    Shutdown();
    ShutdownDisplay();
    DestroyCommonState();
    for (auto it = m_PrimitiveObjects.begin(); it != m_PrimitiveObjects.end(); it++) {
        auto p = (*it).release();
        p->VertexBuffer.Destroy();
        p->IndexBuffer.Destroy();
        for (auto itTex = p->MaterialResource.TextureResources.begin(); itTex != p->MaterialResource.TextureResources.end(); itTex++) {
            delete itTex->pTexture;
            itTex->pTexture = nullptr;
        }
    }
    FinalizePipelineTemplates();
    FinalizeBaseDescriptorHeap();
    FinalizeShaderByteMap();

    XM_Input::Shutdown();
}

void D3dGraphicsCore::CD3dGraphicsCore::setCoreHWND(HWND hwnd, int width, int height)
{
    g_hWnd = hwnd;
    g_DisplayWidth = width;
    g_DisplayHeight = height;

    Initialize(false);
}

void D3dGraphicsCore::CD3dGraphicsCore::InitializeGraphicsSettings()
{
    m_Camera.SetAspectRatio((float)g_DisplayHeight / (float)g_DisplayWidth);
    m_Camera.SetFOV(90.f);
    m_Camera.SetZRange(0.1f, 1000.0f);
    m_Camera.SetPosition(XM_Math::Vector3(0, 0, 5));
    m_CameraController = std::make_unique<XM_Camera::FlyingFPSCamera>(m_Camera, XM_Math::Vector3(0.0f, 1.0f, 0.0f));
    
    m_MainViewport.Width = g_DisplayWidth;
    m_MainViewport.Height = g_DisplayHeight;
    m_MainViewport.MinDepth = 0.0f;
    m_MainViewport.MaxDepth = 1.0f;
    m_MainViewport.TopLeftX = 0.0f;
    m_MainViewport.TopLeftY = 0.0f;

    m_MainScissor.left = 0;
    m_MainScissor.top = 0;
    m_MainScissor.right = g_DisplayWidth;
    m_MainScissor.bottom = g_DisplayHeight;
}

void D3dGraphicsCore::CD3dGraphicsCore::FinalizeGraphicsSettings()
{
    m_CameraController.reset(nullptr);
}

void D3dGraphicsCore::CD3dGraphicsCore::AddPrimitiveObject(std::unique_ptr<PrimitiveObject> _object)
{
    m_PrimitiveObjects.push_back(std::move(_object));
}

void D3dGraphicsCore::CD3dGraphicsCore::SetPrimitiveType(GraphicsContext& context, My::PrimitiveType Type)
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
        break;
    }

    context.SetPrimitiveTopology(d3dType);
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdateStatus()
{
    UpdateCamera();
    UpdateRenderingQueue();
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdateCamera()
{
    float DeltaTime = GetFrameTime();
    m_CameraController->Update(DeltaTime);
    XM_Input::Update(DeltaTime);
    m_Camera.Update();
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdateRenderingQueue()
{
    GraphicsContext& gfxContext = GraphicsContext::Begin(L"Scene Render");

    gfxContext.TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_RENDER_TARGET, true);

    gfxContext.SetViewportAndScissor(m_MainViewport, m_MainScissor);

    g_DisplayBuffer[g_CurrentBuffer].SetClearColor(D3dColor::Color(204.0f / 255.0f, 229.0f / 255.0f, 255.0f / 255.0f));
    gfxContext.ClearColor(g_DisplayBuffer[g_CurrentBuffer]);

    gfxContext.SetRenderTarget(g_DisplayBuffer[g_CurrentBuffer].GetRTV());

    RenderAllObjects(gfxContext);

    gfxContext.TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_PRESENT, true);

    gfxContext.Finish();

    D3dGraphicsCore::Present();

    g_CommandManager.IdleGPU();
}

void D3dGraphicsCore::CD3dGraphicsCore::RenderAllObjects(GraphicsContext& gfxContext)
{
    for (auto it = m_PrimitiveObjects.begin(); it != m_PrimitiveObjects.end(); it++) {
        if ((*it)->MaterialResource.FirstHandle.IsNull()) {
            //没有材质，透明
            continue;
        }
        SetPrimitiveType(gfxContext, (*it)->PrimitiveType);
        gfxContext.SetPipelineState((*it)->MaterialResource.PSO);
        gfxContext.SetRootSignature(g_TemplateRootSignature);

        gfxContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            g_DescriptorHeaps[(*it)->MaterialResource.DescriptorHeapIndex]->GetHeapPointer());
        
        XM_Math::Matrix4 ProjMatrix = m_Camera.GetProjMatrix();
        XM_Math::Matrix4 ViewMatrix = m_Camera.GetViewMatrix();
        XM_Math::Matrix4 Model = XM_Math::Matrix4(XM_Math::EIdentityTag::kIdentity);
        XM_Math::Matrix4 mat = ProjMatrix * ViewMatrix * Model;
        DirectX::XMMATRIX Mat = mat.GetMatrix();
        DirectX::XMMATRIX transposeMat = DirectX::XMMatrixTranspose(mat.GetMatrix());
        gfxContext.SetDynamicConstantBufferView(kMeshConstant, sizeof(transposeMat), &transposeMat);

        gfxContext.SetDescriptorTable(kMaterialSRVs, (*it)->MaterialResource.FirstHandle);

        gfxContext.SetVertexBuffer(0, (*it)->VertexBuffer.VertexBufferView());

        gfxContext.SetIndexBuffer((*it)->IndexBuffer.IndexBufferView());

        gfxContext.DrawIndexedInstanced((*it)->indexCountPerInstance, (*it)->InstanceCount, 0, 0, 0);
    }

}