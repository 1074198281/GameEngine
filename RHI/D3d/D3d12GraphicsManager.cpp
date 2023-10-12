#include <objbase.h>
#include <d3dcompiler.h>
#include "D3d12GraphicsManager.hpp"
#include "WindowsApplication.hpp"
#include "PhysicsManager.hpp"
#include "SceneObject.hpp"
#include "GeometryStructure.h"
#include "RenderObject.h"

using namespace My;


namespace My {
    extern IApplication* g_pApp;
}

int My::D3d12GraphicsManager::Initialize()
{
    My::InitializeSamplers();
    My::InitializeShaderByteCodeMap();
    My::InitializeInputLayout();

    m_pGraphics = std::make_unique<D3dGraphicsCore::CD3dGraphicsCore>();
    m_pGraphics->setCoreHWND(reinterpret_cast<WindowsApplication*>(g_pApp)->GetMainWindow(), g_pApp->GetConfiguration().screenWidth, g_pApp->GetConfiguration().screenHeight);
    m_pGraphics->StartUp();
    m_pGraphics->InitializeGraphics();

    return 0;
}

void My::D3d12GraphicsManager::Finalize()
{
    m_pGraphics->Finalize();
    m_pGraphics.reset(nullptr);
}

void My::D3d12GraphicsManager::Tick()
{
    if (g_pSceneManager->IsSceneChanged()) {
        LoadScene();
        g_pSceneManager->NotifySceneIsRenderingQueued();
    }
    Draw();
}

void My::D3d12GraphicsManager::MoveCameraXPositive()
{

}

void My::D3d12GraphicsManager::MoveCameraXNegative()
{

}

void My::D3d12GraphicsManager::MoveCameraYPositive()
{

}

void My::D3d12GraphicsManager::MoveCameraYNegative()
{

}

void My::D3d12GraphicsManager::Clear()
{
    
}

void My::D3d12GraphicsManager::Draw()
{
    m_pGraphics->UpdateStatus();
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

        //-----------------------------------------Mesh-----------------------------------------//
        auto pMesh = pGeometry->GetMesh().lock();
        if (!pMesh) {
            assert(false);
            continue;
        }

        ASSERT(pMesh->GetIndexGroupCount() == 1, "Index Group Count more than one!");

        std::shared_ptr<My::RenderObject> _object = std::make_shared<My::RenderObject>();
        _object->SetName(_it.first);

        int elementCount = pMesh->GetVertexCount();
        int indexPerCount = 1;
        int vertexPerCount = 0;
        
        // Create Vertex Buffer Data
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
                SetPrimitiveType(_object->m_InputLayoutType, vtArray.GetAttributeName());
                int elementCountPerArray = vtArray.GetDataSize() / vtArray.GetVertexCount() / (vtArray.GetDataSize() / vtArray.GetElementCount());
                float* _pData = (float*)vtArray.GetData();
                for (int j = 0; j < elementCountPerArray; j++) {
                    pVertexData[i * vertexPerCount + _dataCount] = _pData[i * elementCountPerArray + j];
                    _dataCount++;
                }
            }
            ASSERT(_dataCount == vertexPerCount, "Scene Convert Vertex To Data ERROR!");
        }
        _object->VertexBuffer.Create(L"Vertex Buffer", elementCount, sizeof(float) * vertexPerCount, pVertexData);

        //Create Index Buffer Data
        switch (pMesh->GetIndexArray(0).GetIndexType()) {
        case My::kIndexDataTypeInt8:
        {
            uint8_t* pIndexData = new uint8_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint8_t* _pData = (uint8_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            _object->IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint8_t), pIndexData);
        }
        break;
        case My::kIndexDataTypeInt16:
        {
            uint16_t* pIndexData = new uint16_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint16_t* _pData = (uint16_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            _object->IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint16_t), pIndexData);
        }
        break;
        case My::kIndexDataTypeInt32:
        {
            uint32_t* pIndexData = new uint32_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint32_t* _pData = (uint32_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            _object->IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint32_t), pIndexData);
        }
        break;
        case My::kIndexDataTypeInt64:
        {
            uint64_t* pIndexData = new uint64_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint64_t* _pData = (uint64_t*)pMesh->GetIndexArray(0).GetData();
            for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                pIndexData[i] = _pData[i];
            }
            _object->IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint64_t), pIndexData);
        }
        break;
        default:
            ASSERT(false, "Convert Index Type ERROR!");
            break;
        }
        _object->indexCountPerInstance = pMesh->GetIndexArray(0).GetIndexCount();
        _object->InstanceCount = 1;
        
        //-----------------------------------------Material-----------------------------------------//
        _object->alphaStatus = false;
        for (int MaterialIndex = 0; MaterialIndex < GeometryNode->GetMaterialCount(); MaterialIndex++) {
            auto pMaterial = Scene.GetMaterial(GeometryNode->GetMaterialRef(MaterialIndex));

            if (!pMaterial->m_alpha_mode) {
                _object->alphaStatus = true;
                continue;
            }

            int type = pMaterial->GetTextureTypeFlag();
            D3dGraphicsCore::DescriptorHandle FirstHandle;
            std::vector<unsigned int> sourceCount;
            std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> sourceHandle;
            for (int i = 0; i < My::SceneObjectMaterial::TextureType::kpbrType; i++) {
                if (TEST_BIT(type, i)) {
                    auto TextureRef = pMaterial->GetTexture((My::SceneObjectMaterial::TextureType)i);
                    D3dGraphicsCore::Texture pTexture;
                    auto pImage = TextureRef->GetTextureImage();
                    pTexture.Create2D(pImage.pitch, pImage.Width, pImage.Height, DXGI_FORMAT_R8G8B8A8_UNORM, pImage.data);
                    D3dGraphicsCore::DescriptorHandle handle;
                    //handle = m_pGraphics->AllocateTextureDescriptor();
                    if (FirstHandle.IsNull()) {
                        FirstHandle = handle;
                    }
                    sourceHandle.push_back(pTexture.GetSRV());
                    sourceCount.push_back(1);
                    _object->TextureResource.insert(std::make_pair(TextureRef->GetName(), std::make_pair(pTexture, handle)));
                }
            }

            if (FirstHandle.IsNull()) {
                //没有纹理，不需要拷贝
                //会不会有常量缓冲区还需要以后再看，先把理论这一块补全
                continue;
            }
            _object->FirstHandle = FirstHandle;
            //m_pGraphics->CopyTextureDescriptors(FirstHandle, sourceCount, sourceHandle);
        }


        m_pGraphics->AddRenderObject(_object);
    }

    return true;
}

void My::D3d12GraphicsManager::SetPrimitiveType(int& type, std::string attrib)
{
    if (attrib == "POSITION") {
        type |= RenderObject::kPosition;
    }
    if (attrib == "NORMAL") {
        type |= RenderObject::kNormal;
    }
    if (attrib == "TANGENT") {
        type |= RenderObject::kTangent;
    }
    if (attrib == "TEXCOORD0") {
        type |= RenderObject::kTexcoord0;
    }
    if (attrib == "TEXCOORD1") {
        type |= RenderObject::kTexcoord1;
    }
}