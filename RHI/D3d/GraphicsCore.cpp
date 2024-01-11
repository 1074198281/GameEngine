#include "GraphicsCore.h"
#include "Core/D3dGraphicsCoreManager.h"
#include "MemoryManager.hpp"
#include "GraphicsStructure.h"
#include "ShaderSource.h"
#include "D3dComponents/XMInput/XMInput.h"
#include "Core/Common/SystemTime.h"
#include "D3dComponents/XMImageLoader/XMWICImageLoader.h"
#include "ShaderConstants.h"
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
    D3dGraphicsCore::WICLoader::InitializeWICLoader();
    
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
    D3dGraphicsCore::WICLoader::FinalizeWICLoader();
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
    
    /*
    * 这里minDepth为1.0，maxDepth为0.0，因为深度测试。
    */
    m_MainViewport.MinDepth = 1.0f;
    m_MainViewport.MaxDepth = 0.0f;
    m_MainViewport.TopLeftX = 0.0f;
    m_MainViewport.TopLeftY = 0.0f;

    m_MainScissor.left = 0;
    m_MainScissor.top = 0;
    m_MainScissor.right = g_DisplayWidth;
    m_MainScissor.bottom = g_DisplayHeight;

    m_GlobalLightPosition = XMFLOAT4(0.0f, 100.0f, 0.0f, 1.0f);
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

void D3dGraphicsCore::CD3dGraphicsCore::UpdateGlobalLightPosition(XMFLOAT4 pos)
{
    m_GlobalLightPosition.x += pos.x;
    m_GlobalLightPosition.y += pos.y;
    m_GlobalLightPosition.z += pos.z;
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

    //g_DisplayBuffer[g_CurrentBuffer].SetClearColor(D3dColor::Color(204.0f / 255.0f, 229.0f / 255.0f, 255.0f / 255.0f));
    g_DisplayBuffer[g_CurrentBuffer].SetClearColor(D3dColor::g_ColorBlack);

    gfxContext.TransitionResource(g_DepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
    gfxContext.SetRenderTarget(g_DisplayBuffer[g_CurrentBuffer].GetRTV(), g_DepthBuffer.GetDSV());
    gfxContext.ClearColor(g_DisplayBuffer[g_CurrentBuffer]);
    gfxContext.ClearDepth(g_DepthBuffer);

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
        gfxContext.SetRootSignature(g_TemplateRootSignature);

        gfxContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            g_DescriptorHeaps[(*it)->MaterialResource.DescriptorHeapIndex]->GetHeapPointer());

        // Material Constants
        {
            ConstantMaterial MatCbv;
            MatCbv.EyePos = XMFLOAT4(m_Camera.GetPosition().GetX(), m_Camera.GetPosition().GetY(), m_Camera.GetPosition().GetZ(), 1.0f);
            MatCbv.GlobalInfiniteLightPos = m_GlobalLightPosition;
            MatCbv.LightNum = 0;
            gfxContext.SetDynamicConstantBufferView(kMaterialConstant, sizeof(ConstantMaterial), &MatCbv);
        }

        // Material Shader Resource
        {
            gfxContext.SetDescriptorTable(kMaterialSRVs, (*it)->MaterialResource.FirstHandle);
        }

        // Common Constant Buffer
        {
            ConstantBufferView cbv;
            cbv.ModelMatrix = XMLoadFloat4x4((*it)->transform);
            //cbv.ModelMatrix = XMMatrixIdentity();
            cbv.ViewMatrix = m_Camera.GetViewMatrix();
            cbv.ProjMatrix = m_Camera.GetProjMatrix();

            gfxContext.SetDynamicConstantBufferView(kCommonCBV, sizeof(ConstantBufferView), &cbv);
        }

        gfxContext.SetPipelineState((*it)->MaterialResource.PSO);

        gfxContext.SetVertexBuffer(0, (*it)->VertexBuffer.VertexBufferView());

        gfxContext.SetIndexBuffer((*it)->IndexBuffer.IndexBufferView());

        gfxContext.DrawIndexedInstanced((*it)->indexCountPerInstance, 1, 0, 0, 0);
    }

}