#include "GraphicsCore.h"
#include "D3dGraphicsCoreManager.h"
#include "GeometryStructure.h"
#include "MemoryManager.hpp"
#include <array>

using namespace DirectX;

D3dGraphicsCore::CD3dGraphicsCore::CD3dGraphicsCore()
{

}

D3dGraphicsCore::CD3dGraphicsCore::~CD3dGraphicsCore()
{
#if 1
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
    return 0;
}

void D3dGraphicsCore::CD3dGraphicsCore::Finalize()
{
    Shutdown();
    ShutdownDisplay();
    DestroyCommonState();
    for (auto it = m_RenderObjects.begin(); it != m_RenderObjects.end(); it++) {
        (*it)->VertexBuffer.Destroy();
        (*it)->IndexBuffer.Destroy();
        for (auto tex : (*it)->m_TextureResource)
        {
            tex.second.Texture.Destroy();
        }
    }
    m_RenderObjects.clear();

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


void D3dGraphicsCore::CD3dGraphicsCore::setCoreHWND(HWND hwnd, int width, int height)
{
    g_hWnd = hwnd;
    g_DisplayWidth = width;
    g_DisplayHeight = height;

    Initialize(false);

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

void D3dGraphicsCore::CD3dGraphicsCore::InitializeGraphics()
{

}

void D3dGraphicsCore::CD3dGraphicsCore::AddRenderObject(std::shared_ptr<My::RenderObject> _object)
{
    m_RenderObjects.push_back(_object);
}


void D3dGraphicsCore::CD3dGraphicsCore::UpdateStatus()
{
    GraphicsContext& gfxContext = GraphicsContext::Begin(L"Scene Render");

    gfxContext.TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_RENDER_TARGET, true);

    gfxContext.SetViewportAndScissor(m_MainViewport, m_MainScissor);

    g_DisplayBuffer[g_CurrentBuffer].SetClearColor(D3dColor::Color(0.2f, 0.4f, 0.8f));
    gfxContext.ClearColor(g_DisplayBuffer[g_CurrentBuffer]);

    gfxContext.SetRenderTarget(g_DisplayBuffer[g_CurrentBuffer].GetRTV());

    // PipelineState
    //gfxContext.SetPipelineState(m_PSO);
    // RootSignature
    //gfxContext.SetRootSignature(m_RootSignature);
    // 
    gfxContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    //gfxContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_TextureHeap.GetHeapPointer());

    for (auto it = m_RenderObjects.begin(); it != m_RenderObjects.end(); it++) {
        gfxContext.SetVertexBuffer(0, (*it)->VertexBuffer.VertexBufferView());
        gfxContext.SetIndexBuffer((*it)->IndexBuffer.IndexBufferView());
        //gfxContext.SetDynamicConstantBufferView(0, sizeof(m_ModelViewProjMatrix), &m_ModelViewProjMatrix);
        if (!(*it)->m_alphaStatus) {
            gfxContext.SetDescriptorTable(1, (*it)->m_FirstHandle);
        }

        gfxContext.DrawIndexedInstanced((*it)->indexCountPerInstance, (*it)->InstanceCount, 0, 0, 0);
    }

    gfxContext.TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_PRESENT, true);

    gfxContext.Finish();

    D3dGraphicsCore::Present();

    g_CommandManager.IdleGPU();
}