#include "GraphicsCore.h"
#include "D3dGraphicsCoreManager.h"
#include <array>

#include "../Asset/Shaders/CompiledShaders/g_mainPS.h"
#include "../Asset/Shaders/CompiledShaders/g_mainVS.h"

using namespace DirectX;

D3dGraphicsCore::CD3dGraphicsCore::CD3dGraphicsCore()
{

}

D3dGraphicsCore::CD3dGraphicsCore::~CD3dGraphicsCore()
{
    m_IndexBuffer.Destroy();
    m_VertexBuffer.Destroy();
    Shutdown(); 
    ShutdownDisplay();
}

void D3dGraphicsCore::CD3dGraphicsCore::GenerateMatrix()
{
    XMMATRIX proj = DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV4, g_DisplayWidth / (float)g_DisplayHeight, 1.0f, 1000.f);
    XMMATRIX model = XMMATRIX(1.f, 0.f, 0.f, 0.f,
                              0.f, 1.f, 0.f, 0.f,
                              0.f, 0.f, 1.f, 0.f,
                              0.f, 0.f, 0.f, 1.f);
    model = DirectX::XMMatrixTranspose(model);
    XMFLOAT3 eyepos(0.f, 0.f, -5.f);
    XMFLOAT3 lookat(0.f, 0.f, 0.f);
    XMFLOAT3 updire(0.f, 1.f, 0.f);
    XMMATRIX view = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&eyepos), XMLoadFloat3(&lookat), XMLoadFloat3(&updire));

    m_ModelViewProjMatrix = Math::Matrix4(DirectX::XMMatrixTranspose(model * view * proj));
}

void D3dGraphicsCore::CD3dGraphicsCore::setCoreHWND(HWND hwnd, int width, int height)
{
    g_hWnd = hwnd;
    g_DisplayWidth = width;
    g_DisplayHeight = height;
}

void D3dGraphicsCore::CD3dGraphicsCore::InitializeGraphics()
{
    Initialize(false);
    GenerateMatrix();
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
        VertexTemp({ XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT4(Colors::Blue) }),
        VertexTemp({ XMFLOAT3(-0.5f, +0.5f, +0.5f), XMFLOAT4(Colors::Yellow) }),
        VertexTemp({ XMFLOAT3(+0.5f, +0.5f, +0.5f), XMFLOAT4(Colors::Cyan) }),
        VertexTemp({ XMFLOAT3(+0.5f, -0.5f, +0.5f), XMFLOAT4(Colors::Magenta) })
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

    
    m_VertexBuffer.Create(L"vertex buff", 8, sizeof(VertexTemp), vertices.data());
    m_IndexBuffer.Create(L"index buff", 36, sizeof(std::uint16_t), indices.data());

    D3D12_INPUT_ELEMENT_DESC mInputLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    DXGI_FORMAT ColorFormat = D3dGraphicsCore::g_DisplayBuffer[0].GetFormat();
    //DXGI_FORMAT DepthFormat = g_SceneDepthBuffer.GetFormat();

    m_PSO.SetRootSignature(m_RootSignature);
    m_PSO.SetRasterizerState(RasterizerDefault);
    m_PSO.SetBlendState(BlendDisable);
    m_PSO.SetDepthStencilState(DepthStateDisabled);
    m_PSO.SetInputLayout(_countof(mInputLayout), mInputLayout);
    m_PSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    m_PSO.SetRenderTargetFormat(ColorFormat, DXGI_FORMAT_UNKNOWN);
    m_PSO.SetVertexShader(g_pmainVS, sizeof(g_pmainVS));
    m_PSO.SetPixelShader(g_pmainPS, sizeof(g_pmainPS));
    m_PSO.Finalize();
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
    // 
    gfxContext.SetVertexBuffer(0, m_VertexBuffer.VertexBufferView());
    // 
    gfxContext.SetIndexBuffer(m_IndexBuffer.IndexBufferView());
    // 
    gfxContext.SetDynamicConstantBufferView(0, sizeof(m_ModelViewProjMatrix), &m_ModelViewProjMatrix);
    // 
    gfxContext.DrawIndexedInstanced(36, 1, 0, 0, 0);

    gfxContext.TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_PRESENT, true);

    gfxContext.Finish();

    D3dGraphicsCore::Present();
}