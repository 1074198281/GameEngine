#include "GraphicsCore.h"
#include "D3dGraphicsCoreManager.h"
#include "GeometryStructure.h"
#include "MemoryManager.hpp"
#include <array>

#include "../Asset/Shaders/CompiledShaders/g_mainPS.h"
#include "../Asset/Shaders/CompiledShaders/g_mainVS.h"
#include "../Asset/Shaders/CompiledShaders/g_main4PS.h"
#include "../Asset/Shaders/CompiledShaders/g_main4VS.h"

#include "../Asset/Shaders/CompiledShaders/g_cubeTexturePS.h"
#include "../Asset/Shaders/CompiledShaders/g_cubeTextureVS.h"


using namespace DirectX;

D3dGraphicsCore::CD3dGraphicsCore::CD3dGraphicsCore()
{
    m_PSO = GraphicsPSO(L"Main PSO");
    InitializeInputLayout();   
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
    My::InitializeSamplers();
    m_TextureHeap.Create(L"Texture Heap", D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 32);
    return 0;
}

void D3dGraphicsCore::CD3dGraphicsCore::Finalize()
{
    Shutdown();
    ShutdownDisplay();
    DestroyCommonState();
    for (auto it = m_RenderObjects.begin(); it != m_RenderObjects.end(); it++) {
        it->VertexBuffer.Destroy();
        it->IndexBuffer.Destroy();
        for (auto tex : it->TextureResource)
        {
            tex.second.first.Destroy();
        }
    }
    m_RenderObjects.clear();
    m_TextureHeap.Destroy();

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

void D3dGraphicsCore::CD3dGraphicsCore::GenerateMatrix()
{
    m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV4, g_DisplayWidth / (float)g_DisplayHeight, 1.0f, 100000.f);
    XMMATRIX model = XMMATRIX(1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f);
    model = DirectX::XMMatrixTranspose(model);

    m_ModelViewProjMatrix = Math::Matrix4(DirectX::XMMatrixTranspose(model * m_ViewMatrix * m_ProjectionMatrix));
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

void D3dGraphicsCore::CD3dGraphicsCore::UpdateView(DirectX::XMMATRIX mat)
{
    m_ViewMatrix = mat;
    GenerateMatrix();
}

void D3dGraphicsCore::CD3dGraphicsCore::InitializeGraphics()
{
    m_RootSignature.Reset(2, 1);
    m_RootSignature[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_ALL);
    m_RootSignature[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    m_RootSignature.InitStaticSampler(0, My::g_AnisotropicWarpSampler, D3D12_SHADER_VISIBILITY_PIXEL);
    m_RootSignature.Finalize(L"Mesh RootSignature", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    m_PSO.SetRootSignature(m_RootSignature);
    m_PSO.SetRasterizerState(RasterizerDefault);
    m_PSO.SetBlendState(BlendDisable);
    m_PSO.SetDepthStencilState(DepthStateDisabled);
    m_PSO.SetInputLayout(4, m_InputlayoutPosNormalTangentUV);
    m_PSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    m_PSO.SetRenderTargetFormat(g_DisplayBuffer[g_CurrentBuffer].GetFormat(), DXGI_FORMAT_UNKNOWN);
    m_PSO.SetVertexShader(g_pcubeTextureVS, sizeof(g_pcubeTextureVS));
    m_PSO.SetPixelShader(g_pcubeTexturePS, sizeof(g_pcubeTexturePS));
    m_PSO.Finalize();
}

void D3dGraphicsCore::CD3dGraphicsCore::AddRenderObject(My::RenderObject _object)
{
    m_RenderObjects.push_back(_object);
}

D3dGraphicsCore::DescriptorHandle D3dGraphicsCore::CD3dGraphicsCore::AllocateTextureDescriptor(int Count)
{
    DescriptorHandle SRV = m_TextureHeap.Alloc(Count);
    return SRV;
}

void D3dGraphicsCore::CD3dGraphicsCore::CopyTextureDescriptors(DescriptorHandle DesHandle, std::vector<unsigned int>& sourceCount, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& sourceHandle)
{
    uint32_t DestCount = sourceCount.size();
    uint32_t* SourceCounts = sourceCount.data();
    D3D12_CPU_DESCRIPTOR_HANDLE* SourceTextures = sourceHandle.data();

    g_Device->CopyDescriptors(1, &DesHandle, &DestCount,
        DestCount, SourceTextures, SourceCounts, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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
    gfxContext.SetPipelineState(m_PSO);
    // RootSignature
    gfxContext.SetRootSignature(m_RootSignature);
    // 
    gfxContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    gfxContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, m_TextureHeap.GetHeapPointer());

    for (auto it = m_RenderObjects.begin(); it != m_RenderObjects.end(); it++) {
        gfxContext.SetVertexBuffer(0, it->VertexBuffer.VertexBufferView());
        gfxContext.SetIndexBuffer(it->IndexBuffer.IndexBufferView());
        gfxContext.SetDynamicConstantBufferView(0, sizeof(m_ModelViewProjMatrix), &m_ModelViewProjMatrix);
        gfxContext.SetDescriptorTable(1, it->FirstHandle);

        gfxContext.DrawIndexedInstanced(it->indexCountPerInstance, it->InstanceCount, 0, 0, 0);
    }

    gfxContext.TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_PRESENT, true);

    gfxContext.Finish();

    D3dGraphicsCore::Present();

    g_CommandManager.IdleGPU();
}

void D3dGraphicsCore::CD3dGraphicsCore::InitializeInputLayout()
{
    m_InputlayoutPos = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(sizeof(D3D12_INPUT_ELEMENT_DESC));
    m_InputlayoutPosUV = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(2 * sizeof(D3D12_INPUT_ELEMENT_DESC));
    m_InputlayoutPosNormalUV = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(3 * sizeof(D3D12_INPUT_ELEMENT_DESC));
    m_InputlayoutPosNormalTangentUV = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(4 * sizeof(D3D12_INPUT_ELEMENT_DESC));

    D3D12_INPUT_ELEMENT_DESC pos = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    D3D12_INPUT_ELEMENT_DESC tex = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    D3D12_INPUT_ELEMENT_DESC nor = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
    D3D12_INPUT_ELEMENT_DESC tan = { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

    m_InputlayoutPos[0] = { pos };
    m_InputlayoutPosUV[0] = { pos }; m_InputlayoutPosUV[1] = { tex };
    m_InputlayoutPosNormalUV[0] = { pos }; m_InputlayoutPosNormalUV[1] = { nor }; m_InputlayoutPosNormalUV[2] = { tex };
    m_InputlayoutPosNormalTangentUV[0] = { pos }; m_InputlayoutPosNormalTangentUV[1] = { nor }; m_InputlayoutPosNormalTangentUV[2] = { tan }; m_InputlayoutPosNormalTangentUV[3] = { tex };

}


//-------------------------------------test-----------------------------------------//
void D3dGraphicsCore::CD3dGraphicsCore::test()
{
    m_RootSignature.Reset(1, 0);
    m_RootSignature[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_ALL);
    m_RootSignature.Finalize(L"Mesh RootSignature", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    struct VertexTemp
    {
        XMFLOAT3 Pos;
        XMFLOAT4 Color;
    };

    std::array<VertexTemp, 8> vertices =
    {
        VertexTemp({ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(Colors::White) }),
        VertexTemp({ XMFLOAT3(-0.5f, +0.5f, -0.5f), XMFLOAT4(Colors::Black) }),
        VertexTemp({ XMFLOAT3(+0.5f, +0.5f, -0.5f), XMFLOAT4(Colors::Red) }),
        VertexTemp({ XMFLOAT3(+0.5f, -0.5f, -0.5f), XMFLOAT4(Colors::Green) }),
        VertexTemp({ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(Colors::Blue) }),
        VertexTemp({ XMFLOAT3(-0.5f, +0.5f, 0.5f), XMFLOAT4(Colors::Yellow) }),
        VertexTemp({ XMFLOAT3(+0.5f, +0.5f, 0.5f), XMFLOAT4(Colors::Cyan) }),
        VertexTemp({ XMFLOAT3(+0.5f, -0.5f, 0.5f), XMFLOAT4(Colors::Magenta) })
    };

    std::array<std::uint16_t, 36> indices =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };

    
    //m_VertexBuffer.Create(L"vertex buff", 8, sizeof(VertexTemp), vertices.data());
    //m_IndexBuffer.Create(L"index buff", 36, sizeof(std::uint16_t), indices.data());

    D3D12_INPUT_ELEMENT_DESC mInputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    //DXGI_FORMAT ColorFormat = g_SceneColorBuffer.GetFormat();
    //DXGI_FORMAT DepthFormat = g_SceneDepthBuffer.GetFormat();

    m_PSO.SetRootSignature(m_RootSignature);
    m_PSO.SetRasterizerState(RasterizerDefault);
    m_PSO.SetBlendState(BlendDisable);
    m_PSO.SetDepthStencilState(DepthStateDisabled);
    m_PSO.SetInputLayout(_countof(mInputLayout), mInputLayout);
    m_PSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    m_PSO.SetRenderTargetFormat(g_DisplayBuffer[g_CurrentBuffer].GetFormat(), DXGI_FORMAT_UNKNOWN);
    m_PSO.SetVertexShader(g_pmainVS, sizeof(g_pmainVS));
    m_PSO.SetPixelShader(g_pmainPS, sizeof(g_pmainPS));
    m_PSO.Finalize();
}