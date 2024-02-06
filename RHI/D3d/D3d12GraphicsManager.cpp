#include <objbase.h>
#include <d3dcompiler.h>
#include "D3d12GraphicsManager.hpp"
#include "WindowsApplication.hpp"
#include "PhysicsManager.hpp"
#include "SceneObject.hpp"
#include "StructureSettings.h"
#include "GraphicsStructure.h"
#include "ShaderSource.h"
#include "D3dComponents/XMImageLoader/XMWICImageLoader.h"

namespace My {
    extern IApplication* g_pApp;
}

int My::D3d12GraphicsManager::Initialize()
{
    m_pGraphics = std::make_unique<D3dGraphicsCore::CD3dGraphicsCore>();
    m_pGraphics->setCoreHWND(reinterpret_cast<WindowsApplication*>(g_pApp)->GetMainWindow(), g_pApp->GetConfiguration().screenWidth, g_pApp->GetConfiguration().screenHeight);
    m_pGraphics->StartUp();

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

    if (Scene.GeometryNodes.size()) {
        for (auto _it : Scene.GeometryNodes) {
            /*
            * 每个GeometryNode相当Primitive，所以通常来说每个Node中只会存在一种Material
            */

            auto GeometryNode = _it.second;

            if (!GeometryNode->Visible()) {
                continue;
            }
            auto pGeometry = Scene.GetGeometry(GeometryNode->GetSceneObjectRef());

            //-----------------------------------------Mesh-----------------------------------------//
            auto pMesh = pGeometry->GetMesh().lock();
            if (!pMesh) {
                ASSERT(false, "Mesh Is Empty,ERROR!");
                continue;
            }

            ASSERT(pMesh->GetIndexGroupCount() == 1, "Index Group Count more than one!");

            std::unique_ptr<D3dGraphicsCore::PrimitiveObject> _object = std::make_unique<D3dGraphicsCore::PrimitiveObject>();
            _object->PrimitiveType = pMesh->GetPrimitiveType();
            _object->InputLayoutType = 0;

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
                    GenerateInputLayoutType(_object.get(), vtArray.GetAttributeName());
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

            switch (pMesh->GetIndexArray(0).GetIndexType()) {
            case My::kIndexDataTypeInt8:
            {
                uint8_t* pIndexData = new uint8_t[pMesh->GetIndexArray(0).GetIndexCount()];
                uint8_t* _pData = (uint8_t*)pMesh->GetIndexArray(0).GetData();
                memcpy(pIndexData, _pData, sizeof(uint8_t) * pMesh->GetIndexArray(0).GetIndexCount());
                //for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                //    pIndexData[i] = _pData[i];
                //}
                _object->IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint8_t), pIndexData);
            }
            break;
            case My::kIndexDataTypeInt16:
            {
                uint16_t* pIndexData = new uint16_t[pMesh->GetIndexArray(0).GetIndexCount()];
                uint16_t* _pData = (uint16_t*)pMesh->GetIndexArray(0).GetData();
                memcpy(pIndexData, _pData, sizeof(uint16_t) * pMesh->GetIndexArray(0).GetIndexCount());
                //for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                //    pIndexData[i] = _pData[i];
                //}
                _object->IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint16_t), pIndexData);
            }
            break;
            case My::kIndexDataTypeInt32:
            {
                uint32_t* pIndexData = new uint32_t[pMesh->GetIndexArray(0).GetIndexCount()];
                uint32_t* _pData = (uint32_t*)pMesh->GetIndexArray(0).GetData();
                memcpy(pIndexData, _pData, sizeof(uint32_t) * pMesh->GetIndexArray(0).GetIndexCount());
                //for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                //    pIndexData[i] = _pData[i];
                //}
                _object->IndexBuffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint32_t), pIndexData);
            }
            break;
            case My::kIndexDataTypeInt64:
            {
                uint64_t* pIndexData = new uint64_t[pMesh->GetIndexArray(0).GetIndexCount()];
                uint64_t* _pData = (uint64_t*)pMesh->GetIndexArray(0).GetData();
                memcpy(pIndexData, _pData, sizeof(uint64_t) * pMesh->GetIndexArray(0).GetIndexCount());
                //for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
                //    pIndexData[i] = _pData[i];
                //}
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
            ASSERT(GeometryNode->GetMaterialCount() == 1 || GeometryNode->GetMaterialCount() == 0
                , "Primitive Has More Than One Material!");
            if (GeometryNode->GetMaterialCount()) {
                auto pMaterial = Scene.GetMaterial(GeometryNode->GetMaterialRef(0));

                //SRVsHandle用于拷贝默认分配的描述符到设置堆中
                std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> SRVsHandle;

                int TexturesPerMaterial = 0;
                int type = pMaterial->GetTextureTypeFlag();
                for (int i = 0; i < My::SceneObjectMaterial::TextureType::kpbrType; i++) {
                    if (TEST_BIT(type, i)) {
                        auto TextureRef = pMaterial->GetTexture((My::SceneObjectMaterial::TextureType)i);
                        D3dGraphicsCore::Texture* pTexture = new D3dGraphicsCore::Texture();
                        std::wstring TexturePath = Utility::UTF8ToWideString(Scene.m_AssetPath + TextureRef->GetName());
                        auto& pImage = TextureRef->GetTextureImage();
#ifdef USE_DIRECTX_TOOLKIT
                        void* pTexData = D3dGraphicsCore::WICLoader::LoadPNGAndGetImageData(TexturePath.c_str(), pImage.Width, pImage.Height, pImage.pitch, pImage.data_size);
                        //pImage.data = reinterpret_cast<R8G8B8A8Unorm*>(g_pMemoryManager->Allocate(pImage.data_size));
                        //memcpy_s(pImage.data, pImage.data_size, pTexData, pImage.data_size);
                        pTexture->Create2D(pImage.pitch, pImage.Width, pImage.Height, DXGI_FORMAT_R8G8B8A8_UNORM, pTexData);
#else
                        pTexture->Create2D(pImage.pitch, pImage.Width, pImage.Height, DXGI_FORMAT_R8G8B8A8_UNORM, pImage.data);
#endif
                        pTexture->SetName(Utility::UTF8ToWideString(TextureRef->GetName()));
                        SRVsHandle.push_back(pTexture->GetSRV());

                        //DXSampler sampler;
                        //sampler.filter = (D3D12_FILTER)pMaterial->GetTexture((My::SceneObjectMaterial::TextureType)i)->GetSamplerFilter();
                        //sampler.wrapS = (D3D12_TEXTURE_ADDRESS_MODE)pMaterial->GetTexture((My::SceneObjectMaterial::TextureType)i)->GetSamplerWrapS();
                        //sampler.wrapT = (D3D12_TEXTURE_ADDRESS_MODE)pMaterial->GetTexture((My::SceneObjectMaterial::TextureType)i)->GetSamplerWrapT();

                        D3dGraphicsCore::TextureResource re;
                        //re.Handle = Handle;
                        re.pTexture = pTexture;
                        re.pImageData = pTexData;
                        _object->MaterialResource.TextureResources.push_back(re);
                        TexturesPerMaterial++;
                    }
                }
                int HeapIndex = -2;
                D3dGraphicsCore::DescriptorHandle FirstHandle = D3dGraphicsCore::AllocateFromDescriptorHeap(TexturesPerMaterial, HeapIndex);
                ASSERT(HeapIndex > -2, "Descripotr Heap Allocate Failed ERROR!");
                _object->MaterialResource.TextureCountPerMaterial = TexturesPerMaterial;
                _object->MaterialResource.FirstHandle = FirstHandle;
                _object->MaterialResource.DescriptorHeapIndex = HeapIndex;
                //拷贝，且有材质纹理图
                if (!FirstHandle.IsNull()) {
                    D3dGraphicsCore::CopyDescriptors(FirstHandle, SRVsHandle, TexturesPerMaterial);
                }

                _object->name = _it.second->GetSceneObjectRef();
                _object->MaterialName = pMaterial->GetName();
                if (_object->name.size() > 7 && _object->name.substr(_object->name.size() - 7, 7) == "_Shared") {
                    _object->name = pMaterial->GetName();
                }

                //处理每个Primitive的材质对应的PS和VS            
                D3dGraphicsCore::SetPipelineSettings(_object->MaterialResource.PSO
                    , _object->InputLayoutType, _object->PrimitiveType, _object->name);
                if (!FirstHandle.IsNull()) {
                    std::cout << "----------------------------" << std::endl;
                    std::cout << "Mesh " << _object->name << " Add Material " << _object->MaterialName << std::endl;
                    D3dGraphicsCore::SetShaderByteCode(_object->MaterialResource.PSO, _object->MaterialName);
                }
                else {
                    std::cout << "----------------------------" << std::endl;
                    std::cout << "Mesh " << _object->name << " Has None Material Texture" << std::endl;
                    continue;
                }
                _object->MaterialResource.PSO.SetRenderTargetFormats(1,
                    &D3dGraphicsCore::g_SceneColorBuffer.GetFormat(),
                    DSV_FORMAT);
                _object->MaterialResource.PSO.Finalize();
            }
            else {
                //没有材质，可能是透明的，参考alphamode的值
            }

            //transform
            auto mat = GeometryNode->GetCalculatedTransform();
            _object->transform = new DirectX::XMFLOAT4X4();
            memcpy(_object->transform, &*mat, 16 * sizeof(float));

            m_pGraphics->AddPrimitiveObject(std::move(_object));
        }
    }
    

    if (Scene.LightNodes.size()) {
        for (auto _it : Scene.LightNodes) {

        }

        {
            // add global light
            D3dGraphicsCore::LightResource globalLight;
            globalLight.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }


    return true;
}

void My::D3d12GraphicsManager::UpArrowKeyDown()
{
    m_pGraphics->UpdateGlobalLightPosition(XMFLOAT4(0.0f, 100.0f, 0.0f, 1.0f));
}

void My::D3d12GraphicsManager::DownArrowKeyDown()
{
    m_pGraphics->UpdateGlobalLightPosition(XMFLOAT4(XMFLOAT4(0.0f, -100.0f, 0.0f, 1.0f)));
}

void My::D3d12GraphicsManager::LeftArrowKeyDown()
{
    m_pGraphics->UpdateGlobalLightPosition(XMFLOAT4(-100.0f, 0.0f, 0.0f, 1.0f));
}

void My::D3d12GraphicsManager::RightArrowKeyDown()
{
    m_pGraphics->UpdateGlobalLightPosition(XMFLOAT4(100.0f, 0.0f, 0.0f, 1.0f));
}

void My::D3d12GraphicsManager::NumPadKeyDown(int64_t key)
{
    switch (key) {
    case VK_NUMPAD0:
        break;
    case VK_NUMPAD1:
        break;
    case VK_NUMPAD2:
        break;
    case VK_NUMPAD3:
        break;
    case VK_NUMPAD4:
        m_pGraphics->UpdateGlobalLightPosition(XMFLOAT4(0.0f, 0.0f, 100.0f, 1.0f));
        break;
    case VK_NUMPAD5:
        break;
    case VK_NUMPAD6:
        m_pGraphics->UpdateGlobalLightPosition(XMFLOAT4(0.0f, 0.0f, -100.0f, 1.0f));
        break;
    case VK_NUMPAD7:
        break;
    case VK_NUMPAD8:
        break;
    case VK_NUMPAD9:
        break;
    default:
        ASSERT(false, "RECIEVE UNKOWN NUMPAD INPUT! ERROR!");
    }
}

void My::D3d12GraphicsManager::FunctionKeyDown(int64_t key)
{
    m_pGraphics->UpdateCameraParams(key);
}
void My::D3d12GraphicsManager::FunctionKeyUp(int64_t key)
{

}

void My::D3d12GraphicsManager::NumPadKeyUp(int64_t key)
{

}

bool My::D3d12GraphicsManager::GenerateInputLayoutType(D3dGraphicsCore::PrimitiveObject* _object, const std::string& name)
{
    if (name == "POSITION") {
        _object->InputLayoutType |= (1 << D3dGraphicsCore::kPos);
    }
    else if (name == "NORMAL") {
        _object->InputLayoutType |= (1 << D3dGraphicsCore::kNormal);
    }
    else if (name == "TANGENT") {
        _object->InputLayoutType |= (1 << D3dGraphicsCore::kTangent);
    }
    else if (name == "TEXCOORD0") {
        _object->InputLayoutType |= (1 << D3dGraphicsCore::kTexcoord0);
    }
    else if (name == "TEXCOORD1") {
        _object->InputLayoutType |= (1 << D3dGraphicsCore::kTexcoord1);
    }
    else {
        ASSERT(false, "Set InputLayout Type Error!")
        return false;
    }
    return true;
}