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
    m_pGraphics->setCoreHWND(reinterpret_cast<WindowsApplication*>(g_pApp)->GetMainWindow());
    return 0;
}

void My::D3d12GraphicsManager::Finalize()
{

}

void My::D3d12GraphicsManager::Tick()
{

}

void My::D3d12GraphicsManager::Clear()
{
    
}

void My::D3d12GraphicsManager::Draw()
{
    
}

bool My::D3d12GraphicsManager::LoadScene()
{
    auto& Scene = g_pSceneManager->GetSceneForRendering();
    
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

        std::vector<Vertex> VertexFloat;
        std::vector<VertexT> VertexFloat2;
        std::vector<VertexNT> VertexFloat3;
        std::vector<VertexNTT> VertexFloat4;

        switch (pMesh->GetVertexPropertiesCount())
        {
        case 1:
        {
            CreateVertex(pMesh, VertexFloat);
            break;
        }
        case 2:
        {
            CreateVertex(pMesh, VertexFloat2);
            break;
        }
        case 3:
        {
            CreateVertex(pMesh, VertexFloat3);
            break;
        }
        case 4:
        {
            CreateVertex(pMesh, VertexFloat4);
            break;
        }
        default:
            assert(false);
            break;
        }
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