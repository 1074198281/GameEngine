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
    m_pGraphics->setCoreHWND(reinterpret_cast<WindowsApplication*>(g_pApp)->GetMainWindow(), g_pApp->GetConfiguration().screenWidth, g_pApp->GetConfiguration().screenHeight);
    m_pGraphics->InitializeGraphics();
    return 0;
}

void My::D3d12GraphicsManager::Finalize()
{

}

void My::D3d12GraphicsManager::Tick()
{
    Draw();
    m_pGraphics->UpdateStatus();
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

        int elementCount = pMesh->GetVertexCount();
        int indexPerCount = 1;
        int vertexPerCount = 0;
        //ÿ����ٸ�float����position + tex����3 + 2
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
                const size_t elementCount = vtArray.GetDataSize();
                int elementCountPerArray = vtArray.GetDataSize() / vtArray.GetVertexCount() / (vtArray.GetDataSize() / vtArray.GetElementCount());
                float* _pData = (float*)vtArray.GetData();
                for (int j = 0; j < elementCountPerArray; j++) {
                    pVertexData[i * vertexPerCount + _dataCount] = _pData[i * elementCountPerArray + j];
                    _dataCount++;
                }
            }
            ASSERT(_dataCount == vertexPerCount, "Scene Convert Vertex To Data ERROR!");
        }


        switch (pMesh->GetIndexArray(0).GetIndexType()) {
        case My::kIndexDataTypeInt8:
        {
            uint8_t* pIndexData = new uint8_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint8_t* _pData = (uint8_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            m_pGraphics->SetIndexBuffer(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint8_t), pIndexData);
        }
        break;
        case My::kIndexDataTypeInt16:
        {
            uint16_t* pIndexData = new uint16_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint16_t* _pData = (uint16_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            m_pGraphics->SetIndexBuffer(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint16_t), pIndexData);
        }
        break;
        case My::kIndexDataTypeInt32:
        {
            uint32_t* pIndexData = new uint32_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint32_t* _pData = (uint32_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            m_pGraphics->SetIndexBuffer(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint32_t), pIndexData);
        }
        break;
        case My::kIndexDataTypeInt64:
        {
            uint64_t* pIndexData = new uint64_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint64_t* _pData = (uint64_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            m_pGraphics->SetIndexBuffer(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint64_t), pIndexData);
        }
        break;
        default:
            ASSERT(false, "Convert Index Type ERROR!");
            break;
        }

        m_pGraphics->SetVertexBuffer(L"Vertex Buffer", elementCount * vertexPerCount, elementCount * vertexPerCount * sizeof(float), pVertexData);
    }

    return true;
}

void My::D3d12GraphicsManager::CreateVertex(std::shared_ptr<SceneObjectMesh> pMesh, std::vector<Vertex>& VertexVec)
{
    assert(pMesh->GetVertexPropertiesCount() == 1);
    /*
    const SceneObjectVertexArray& vtArray = pMesh->GetVertexPropertyArray(0);
    const void* pData = vtArray.GetData();
    void* pVecData = VertexVec.data();
    memcpy(pVecData, pData, vtArray.GetDataSize());
    */

    for (int j = 0; j < pMesh->GetVertexCount(); j++) {
        Vertex vt;
        for (int i = 0; i < pMesh->GetVertexPropertiesCount(); i++) {
            const SceneObjectVertexArray& vtArray = pMesh->GetVertexPropertyArray(i);
            const void* pData = vtArray.GetData();
            const char* pcharData = static_cast<const char*>(pData);
            const float* pfloatData = reinterpret_cast<const float*>(pcharData);
            vt.pos[0] = pfloatData[j * 3];
            vt.pos[1] = pfloatData[j * 3 + 1];
            vt.pos[2] = pfloatData[j * 3 + 2];
        }
        VertexVec.push_back(vt);
    }
}

void My::D3d12GraphicsManager::CreateVertex(std::shared_ptr<SceneObjectMesh> pMesh, std::vector<VertexT>& VertexVec)
{
    for (int j = 0; j < pMesh->GetVertexCount(); j++) {
        VertexT vt;
        const float* pVertexPropertyData[2];
        for (int i = 0; i < pMesh->GetVertexPropertiesCount(); i++) {
            const SceneObjectVertexArray& vtArray = pMesh->GetVertexPropertyArray(i);
            const void* pData = vtArray.GetData();
            const char* pcharData = static_cast<const char*>(pData);
            pVertexPropertyData[i] = reinterpret_cast<const float*>(pcharData);
        }
        vt.pos = Vector3f(pVertexPropertyData[0][j * 3], pVertexPropertyData[0][j * 3 + 1], pVertexPropertyData[0][j * 3 + 2]);
        vt.tex = Vector2f(pVertexPropertyData[1][j * 2], pVertexPropertyData[1][j * 2 + 1]);
        VertexVec.push_back(vt);
    }
}

void My::D3d12GraphicsManager::CreateVertex(std::shared_ptr<SceneObjectMesh> pMesh, std::vector<VertexNT>& VertexVec)
{
    for (int j = 0; j < pMesh->GetVertexCount(); j++) {
        VertexNT vt;
        const float* pVertexPropertyData[3];
        for (int i = 0; i < pMesh->GetVertexPropertiesCount(); i++) {
            const SceneObjectVertexArray& vtArray = pMesh->GetVertexPropertyArray(i);
            const void* pData = vtArray.GetData();
            const char* pcharData = static_cast<const char*>(pData);
            pVertexPropertyData[i] = reinterpret_cast<const float*>(pcharData);
        }
        vt.pos = Vector3f(pVertexPropertyData[0][j * 3], pVertexPropertyData[0][j * 3 + 1], pVertexPropertyData[0][j * 3 + 2]);
        vt.normal = Vector3f(pVertexPropertyData[1][j * 3], pVertexPropertyData[1][j * 3 + 1], pVertexPropertyData[1][j * 3 + 2]);
        vt.tex = Vector2f(pVertexPropertyData[2][j * 2], pVertexPropertyData[2][j * 2 + 1]);
        VertexVec.push_back(vt);
    }
}

void My::D3d12GraphicsManager::CreateVertex(std::shared_ptr<SceneObjectMesh> pMesh, std::vector<VertexNTT>& VertexVec)
{
    for (int j = 0; j < pMesh->GetVertexCount(); j++) {
        VertexNTT vt;
        const float* pVertexPropertyData[4];
        for (int i = 0; i < pMesh->GetVertexPropertiesCount(); i++) {
            const SceneObjectVertexArray& vtArray = pMesh->GetVertexPropertyArray(i);
            const void* pData = vtArray.GetData();
            const char* pcharData = static_cast<const char*>(pData);
            pVertexPropertyData[i] = reinterpret_cast<const float*>(pcharData);
        }
        vt.pos = Vector3f(pVertexPropertyData[0][j * 3], pVertexPropertyData[0][j * 3 + 1], pVertexPropertyData[0][j * 3 + 2]);
        vt.normal = Vector3f(pVertexPropertyData[1][j * 3], pVertexPropertyData[1][j * 3 + 1], pVertexPropertyData[1][j * 3 + 2]);
        vt.tangent = Vector3f(pVertexPropertyData[2][j * 3], pVertexPropertyData[2][j * 3 + 1], pVertexPropertyData[2][j * 3 + 2]);
        vt.tex = Vector2f(pVertexPropertyData[3][j * 2], pVertexPropertyData[3][j * 2 + 1]);
        VertexVec.push_back(vt);
    }
}