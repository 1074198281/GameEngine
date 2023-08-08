#include <objbase.h>
#include <d3dcompiler.h>
#include "D3d12GraphicsManager.hpp"
#include "WindowsApplication.hpp"

using namespace My;


namespace My {
    extern IApplication* g_pApp;

    template<class T>
    inline void SafeRelease(T** ppInterfaceToRelease)
    {
        if (*ppInterfaceToRelease != nullptr)
        {
            (*ppInterfaceToRelease)->Release();

            (*ppInterfaceToRelease) = nullptr;
        }
    }
}

int My::D3d12GraphicsManager::Initialize()
{
    m_pGraphics = std::make_unique<D3dGraphicsCore::CD3dGraphicsCore>();
    m_pGraphics->StartUp();
    m_pGraphics->setCoreHWND(reinterpret_cast<WindowsApplication*>(g_pApp)->GetMainWindow(), g_pApp->GetConfiguration().screenWidth, g_pApp->GetConfiguration().screenHeight);
    m_pGraphics->InitializeGraphics();

    m_EyePos = DirectX::XMFLOAT3(0.f, 0.f, -400.f);
    DirectX::XMFLOAT3 lookat(0.f, 0.f, 0.f);
    DirectX::XMFLOAT3 updir(0.f, 1.f, 0.f);
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&m_EyePos), XMLoadFloat3(&lookat), XMLoadFloat3(&updir));
    m_pGraphics->UpdateView(view);
    return 0;
}

void My::D3d12GraphicsManager::Finalize()
{
    m_pGraphics->Finalize();
}

void My::D3d12GraphicsManager::Tick()
{
    if (!m_bLoadOK) {
        Draw();
        m_bLoadOK = true;
    }

    m_pGraphics->UpdateStatus();
}

void My::D3d12GraphicsManager::MoveCameraXPositive()
{
    m_EyePos.x += 1.0f;
    DirectX::XMFLOAT3 lookat(0.f, 0.f, 0.f);
    DirectX::XMFLOAT3 updir(0.f, 1.f, 0.f);
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&m_EyePos), XMLoadFloat3(&lookat), XMLoadFloat3(&updir));
    m_pGraphics->UpdateView(view);
}

void My::D3d12GraphicsManager::MoveCameraXNegative()
{
    m_EyePos.x -= 1.0f;
    DirectX::XMFLOAT3 lookat(0.f, 0.f, 0.f);
    DirectX::XMFLOAT3 updir(0.f, 1.f, 0.f);
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&m_EyePos), XMLoadFloat3(&lookat), XMLoadFloat3(&updir));
    m_pGraphics->UpdateView(view);
}

void My::D3d12GraphicsManager::MoveCameraYPositive()
{
    m_EyePos.y += 1.0f;
    DirectX::XMFLOAT3 lookat(0.f, 0.f, 0.f);
    DirectX::XMFLOAT3 updir(0.f, 1.f, 0.f);
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&m_EyePos), XMLoadFloat3(&lookat), XMLoadFloat3(&updir));
    m_pGraphics->UpdateView(view);
}

void My::D3d12GraphicsManager::MoveCameraYNegative()
{
    m_EyePos.y -= 1.0f;
    DirectX::XMFLOAT3 lookat(0.f, 0.f, 0.f);
    DirectX::XMFLOAT3 updir(0.f, 1.f, 0.f);
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(XMLoadFloat3(&m_EyePos), XMLoadFloat3(&lookat), XMLoadFloat3(&updir));
    m_pGraphics->UpdateView(view);
}

void My::D3d12GraphicsManager::Clear()
{
    
}

void My::D3d12GraphicsManager::Draw()
{
    LoadScene();
}

bool My::D3d12GraphicsManager::LoadScene()
{
    auto& Scene = g_pSceneManager->GetSceneForRendering();
    ASSERT(&Scene, "Scene Is Null Error!");

    for (auto _it : Scene.GeometryNodes) {
        auto GeometryNode = _it.second;

        if (!GeometryNode->Visible()) {
            continue;
        }
        auto pGeometry = Scene.GetGeometry(GeometryNode->GetSceneObjectRef());
        auto pMesh = pGeometry->GetMesh().lock();
        if (!pMesh) {
            assert(false);
            continue;
        }

        ASSERT(pMesh->GetIndexGroupCount() == 1, "Index Group Count more than one!");

        My::RenderObject _object;

        int elementCount = pMesh->GetVertexCount();
        int indexPerCount = 1;
        int vertexPerCount = 0;
        
        for (int groupCount = 0; groupCount < pMesh->GetVertexPropertiesCount(); groupCount++) {
            auto& vtArray = pMesh->GetVertexPropertyArray(groupCount);
            ASSERT(vtArray.GetDataSize() / vtArray.GetElementCount() == sizeof(float), "Vertex Type Double, Not Realize! ERROR!");
            vertexPerCount += vtArray.GetDataSize() / vtArray.GetVertexCount() / (vtArray.GetDataSize() / vtArray.GetElementCount());
        }
        float* pVertexData = new float[elementCount * vertexPerCount];

        for (int i = 0; i < elementCount; i++) {
            int _dataCount = 0;
            for (int groupCount = 0; groupCount < pMesh->GetVertexPropertiesCount(); groupCount++) {
                const SceneObjectVertexArray& vtArray = pMesh->GetVertexPropertyArray(groupCount);
                int elementCountPerArray = vtArray.GetDataSize() / vtArray.GetVertexCount() / (vtArray.GetDataSize() / vtArray.GetElementCount());
                float* _pData = (float*)vtArray.GetData();
                for (int j = 0; j < elementCountPerArray; j++) {
                    pVertexData[i * vertexPerCount + _dataCount] = _pData[i * elementCountPerArray + j];
                    _dataCount++;
                }
            }
            ASSERT(_dataCount == vertexPerCount, "Scene Convert Vertex To Data ERROR!");
        }
        _object.VertexBuffer.Create(L"Vertex Buffer", elementCount, sizeof(float) * vertexPerCount, pVertexData);

        switch (pMesh->GetIndexArray(0).GetIndexType()) {
        case My::kIndexDataTypeInt8:
        {
            uint8_t* pIndexData = new uint8_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint8_t* _pData = (uint8_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            _object.IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint8_t), pIndexData);
        }
        break;
        case My::kIndexDataTypeInt16:
        {
            uint16_t* pIndexData = new uint16_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint16_t* _pData = (uint16_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            _object.IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint16_t), pIndexData);
        }
        break;
        case My::kIndexDataTypeInt32:
        {
            uint32_t* pIndexData = new uint32_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint32_t* _pData = (uint32_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            _object.IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint32_t), pIndexData);
        }
        break;
        case My::kIndexDataTypeInt64:
        {
            uint64_t* pIndexData = new uint64_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint64_t* _pData = (uint64_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            _object.IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint64_t), pIndexData);
        }
        break;
        default:
            ASSERT(false, "Convert Index Type ERROR!");
            break;
        }
        _object.indexCountPerInstance = pMesh->GetIndexArray(0).GetIndexCount();
        _object.InstanceCount = 1;

        m_pGraphics->AddRenderObject(_object);
    }

    return true;
}
