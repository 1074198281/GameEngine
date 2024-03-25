#include <objbase.h>
#include <d3dcompiler.h>
#include "D3d12GraphicsManager.hpp"
#include "WindowsApplication.hpp"
#include "D3d12Application.hpp"
#include "PhysicsManager.hpp"
#include "SceneObject.hpp"
#include "StructureSettings.h"
#include "GraphicsStructure.h"
#include "ShaderSource.h"
#include "D3dComponents/XMImageLoader/XMWICImageLoader.h"
#include "GraphicsStructure.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

int My::D3d12GraphicsManager::Initialize()
{
    int result = 0;
    GraphicsManager::Initialize();

    result = InitializeD3dImGUI();

    return result;
}

void My::D3d12GraphicsManager::Finalize()
{
    GraphicsManager::Finalize();
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.Finalize();
}

void My::D3d12GraphicsManager::Clear()
{
    
}

void My::D3d12GraphicsManager::Draw()
{
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.UpdateStatus();
}

void My::D3d12GraphicsManager::Resize(uint32_t width, uint32_t height)
{
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.Resize(width, height);
}

void My::D3d12GraphicsManager::Present()
{
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.UpdatePresent();
}

void My::D3d12GraphicsManager::initializeGeometries(const Scene& scene)
{
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    uint32_t batch_index = 0;
    for (auto _it : scene.GeometryNodes) {
        auto& GeometryNode = _it.second;
        if (!GeometryNode->Visible()) {
            continue;
        }
        auto pGeometry = scene.GetGeometry(GeometryNode->GetSceneObjectRef());

        //-----------------------------------------Mesh-----------------------------------------//
        auto pMesh = pGeometry->GetMesh().lock();
        if (!pMesh) {
            ASSERT(false, "Mesh Is Empty,ERROR!");
            continue;
        }

        ASSERT(pMesh->GetIndexGroupCount() == 1, "Index Group Count more than one!");

        D3dDrawBatchContext dbc;
        dbc.m_PrimitiveType = pMesh->GetPrimitiveType();
        dbc.BatchIndex = batch_index;
        dbc.m_inputlayout = 0;

        std::unique_ptr<D3dGraphicsCore::PrimitiveObject> _object = std::make_unique<D3dGraphicsCore::PrimitiveObject>();
        _object->PrimitiveType = pMesh->GetPrimitiveType();

        int elementCount = pMesh->GetVertexCount();
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
                GenerateInputLayoutType(dbc.m_inputlayout, vtArray.GetAttributeName());
                int elementCountPerArray = vtArray.GetDataSize() / vtArray.GetVertexCount() / (vtArray.GetDataSize() / vtArray.GetElementCount());
                float* _pData = (float*)vtArray.GetData();
                for (int j = 0; j < elementCountPerArray; j++) {
                    pVertexData[i * vertexPerCount + _dataCount] = _pData[i * elementCountPerArray + j];
                    _dataCount++;
                }
            }
            ASSERT(_dataCount == vertexPerCount, "Scene Convert Vertex To Data ERROR!");
        }
        dbc.m_vertex_buffer.Create(L"Vertex Buffer", elementCount, sizeof(float) * vertexPerCount, pVertexData);

        switch (pMesh->GetIndexArray(0).GetIndexType()) {
        case My::kIndexDataTypeInt8:
        {
            uint8_t* pIndexData = new uint8_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint8_t* _pData = (uint8_t*)pMesh->GetIndexArray(0).GetData();
            memcpy(pIndexData, _pData, sizeof(uint8_t) * pMesh->GetIndexArray(0).GetIndexCount());
            //for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
            //    pIndexData[i] = _pData[i];
            //}
            dbc.m_index_buffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint8_t), pIndexData);
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
            dbc.m_index_buffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint16_t), pIndexData);
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
            dbc.m_index_buffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint32_t), pIndexData);
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
            dbc.m_index_buffer.Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint64_t), pIndexData);
        }
        break;
        default:
            ASSERT(false, "Convert Index Type ERROR!");
            break;
        }
        dbc.m_index_count_per_instance = pMesh->GetIndexArray(0).GetIndexCount();

        //-----------------------------------------Material-----------------------------------------//
        ASSERT(GeometryNode->GetMaterialCount() == 1 || GeometryNode->GetMaterialCount() == 0
            , "Primitive Has More Than One Material!");
        if (GeometryNode->GetMaterialCount()) {
            auto pMaterial = scene.GetMaterial(GeometryNode->GetMaterialRef(0));

            //SRVsHandle用于拷贝默认分配的描述符到设置堆中
            std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> SRVsHandle;

            int TexturesPerMaterial = 0;
            int type = pMaterial->GetTextureTypeFlag();
            for (int i = 0; i < My::SceneObjectMaterial::TextureType::kpbrType; i++) {
                if (TEST_BIT(type, i)) {
                    auto TextureRef = pMaterial->GetTexture((My::SceneObjectMaterial::TextureType)i);
                    D3dGraphicsCore::Texture* pTexture = new D3dGraphicsCore::Texture();
                    std::string TexPath = std::string(_ASSET_RESOURCE_DIRECTORY) + "/" + TextureRef->GetName();
                    std::wstring TexturePath = Utility::UTF8ToWideString(TexPath);
                    Image img;
                    img.data = TextureRef->GetTextureImage().data;
#ifdef USE_DIRECTX_TOOLKIT
                    DXGI_FORMAT format;
                    void* pTexData = D3dGraphicsCore::WICLoader::LoadPNGAndGetImageData(TexturePath.c_str(), img.Width, img.Height, img.pitch, img.data_size, format);
                    //pImage.data = reinterpret_cast<R8G8B8A8Unorm*>(g_pMemoryManager->Allocate(pImage.data_size));
                    //memcpy_s(pImage.data, pImage.data_size, pTexData, pImage.data_size);
                    pTexture->Create2D(img.pitch, img.Width, img.Height, DXGI_FORMAT_R8G8B8A8_UNORM, pTexData);
#else
                    pTexture->Create2D(pImage.pitch, img.Width, img.Height, DXGI_FORMAT_R8G8B8A8_UNORM, img.data);
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
                else {
                    D3dGraphicsCore::TextureResource re;
                    switch (i) {
                    case My::SceneObjectMaterial::kBaseColor:
                    {
                        SRVsHandle.push_back(D3dGraphicsCore::g_DefaultBaseColorTexture.GetSRV());
                        re.pTexture = &D3dGraphicsCore::g_DefaultBaseColorTexture;
                    }
                    break;
                    case My::SceneObjectMaterial::kMetallicRoughness:
                    {
                        SRVsHandle.push_back(D3dGraphicsCore::g_DefaultRoughnessMetallicTexture.GetSRV());
                        re.pTexture = &D3dGraphicsCore::g_DefaultRoughnessMetallicTexture;
                    }
                    break;
                    case My::SceneObjectMaterial::kOcclusion:
                    {
                        SRVsHandle.push_back(D3dGraphicsCore::g_DefaultOcclusionTexture.GetSRV());
                        re.pTexture = &D3dGraphicsCore::g_DefaultOcclusionTexture;
                    }
                    break;
                    case My::SceneObjectMaterial::kEmissive:
                    {
                        SRVsHandle.push_back(D3dGraphicsCore::g_DefaultEmissiveTexture.GetSRV());
                        re.pTexture = &D3dGraphicsCore::g_DefaultEmissiveTexture;
                    }
                    break;
                    case My::SceneObjectMaterial::kNormal:
                    {
                        SRVsHandle.push_back(D3dGraphicsCore::g_DefaultNormalTexture.GetSRV());
                        re.pTexture = &D3dGraphicsCore::g_DefaultNormalTexture;
                    }
                    break;
                    default:
                        ASSERT(false, "NO BLANK DESCRIPTOR TO FILL HEAP,ERROR!");
                        break;
                    }


                    //re.Handle = Handle;
                    re.pImageData = nullptr;
                    _object->MaterialResource.TextureResources.push_back(re);
                    TexturesPerMaterial++;
                }

                // texture transform
                My::TextureTransform trans = pMaterial->GetTextureTransform(My::SceneObjectMaterial::TextureType(i));
                switch (i) {
                case My::SceneObjectMaterial::kBaseColor:
                {
                    _object->MaterialResource.BaseColorTextureTransform[0] = trans.offset[0];
                    _object->MaterialResource.BaseColorTextureTransform[1] = trans.offset[1];
                    _object->MaterialResource.BaseColorTextureTransform[2] = trans.scale[0];
                    _object->MaterialResource.BaseColorTextureTransform[3] = trans.scale[1];
                    _object->MaterialResource.BaseColorTextureTransform[4] = trans.rotation;
                }
                break;
                case My::SceneObjectMaterial::kMetallicRoughness:
                {
                    _object->MaterialResource.MetallicRoughnessTextureTransform[0] = trans.offset[0];
                    _object->MaterialResource.MetallicRoughnessTextureTransform[1] = trans.offset[1];
                    _object->MaterialResource.MetallicRoughnessTextureTransform[2] = trans.scale[0];
                    _object->MaterialResource.MetallicRoughnessTextureTransform[3] = trans.scale[1];
                    _object->MaterialResource.MetallicRoughnessTextureTransform[4] = trans.rotation;
                }
                break;
                case My::SceneObjectMaterial::kOcclusion:
                {
                    _object->MaterialResource.OcclusionTransform[0] = trans.offset[0];
                    _object->MaterialResource.OcclusionTransform[1] = trans.offset[1];
                    _object->MaterialResource.OcclusionTransform[2] = trans.scale[0];
                    _object->MaterialResource.OcclusionTransform[3] = trans.scale[1];
                    _object->MaterialResource.OcclusionTransform[4] = trans.rotation;
                }
                break;
                case My::SceneObjectMaterial::kEmissive:
                {
                    _object->MaterialResource.EmissiveTextureTransform[0] = trans.offset[0];
                    _object->MaterialResource.EmissiveTextureTransform[1] = trans.offset[1];
                    _object->MaterialResource.EmissiveTextureTransform[2] = trans.scale[0];
                    _object->MaterialResource.EmissiveTextureTransform[3] = trans.scale[1];
                    _object->MaterialResource.EmissiveTextureTransform[4] = trans.rotation;
                }
                break;
                case My::SceneObjectMaterial::kNormal:
                {
                    _object->MaterialResource.NormalTextureTransform[0] = trans.offset[0];
                    _object->MaterialResource.NormalTextureTransform[1] = trans.offset[1];
                    _object->MaterialResource.NormalTextureTransform[2] = trans.scale[0];
                    _object->MaterialResource.NormalTextureTransform[3] = trans.scale[1];
                    _object->MaterialResource.NormalTextureTransform[4] = trans.rotation;
                }
                break;
                default:
                    ASSERT(false, "TEXTURE TRANSFORM ERROR!");
                    break;
                }
            }

            // factor param / 
            _object->MaterialResource.BaseColorFactor[0] = pMaterial->GetBaseColorFactor()[0];
            _object->MaterialResource.BaseColorFactor[1] = pMaterial->GetBaseColorFactor()[1];
            _object->MaterialResource.BaseColorFactor[2] = pMaterial->GetBaseColorFactor()[2];
            _object->MaterialResource.BaseColorFactor[3] = pMaterial->GetBaseColorFactor()[3];

            _object->MaterialResource.EmissiveFactor[0] = pMaterial->GetEmissivsFactor()[0];
            _object->MaterialResource.EmissiveFactor[1] = pMaterial->GetEmissivsFactor()[1];
            _object->MaterialResource.EmissiveFactor[2] = pMaterial->GetEmissivsFactor()[2];

            _object->MaterialResource.MetallicRoughnessFactor[0] = pMaterial->GetMetallicFactor();
            _object->MaterialResource.MetallicRoughnessFactor[1] = pMaterial->GetRoughnessFactor();

            _object->MaterialResource.NormalScaleFactor = pMaterial->GetNormalScaleFactor();

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
            //if (_object->name.size() > 7 && _object->name.substr(_object->name.size() - 7, 7) == "_Shared") {
            //    _object->name = pMaterial->GetName();
            //}

            //处理每个Primitive的材质对应的PS和VS
            D3dGraphicsCore::SetPipelineSettings(_object->MaterialResource.PSO
                , dbc.m_inputlayout, dbc.m_PrimitiveType, _object->name);
            if (!FirstHandle.IsNull()) {
                std::cout << "----------------------------" << std::endl;
                std::cout << "Mesh " << _object->name << " Add Material " << _object->MaterialName << std::endl;
                D3dGraphicsCore::SetShaderByteCode(_object->MaterialResource.PSO, "Model");
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

        GraphicsRHI.AddPrimitiveObject(std::move(_object));
    }
}

void My::D3d12GraphicsManager::initializeSkybox(const Scene& scene)
{

}

void My::D3d12GraphicsManager::UpArrowKeyDown()
{
    
}

void My::D3d12GraphicsManager::DownArrowKeyDown()
{
    
}

void My::D3d12GraphicsManager::LeftArrowKeyDown()
{
    
}

void My::D3d12GraphicsManager::RightArrowKeyDown()
{
    
}

void My::D3d12GraphicsManager::NumPadKeyDown(int64_t key)
{
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    switch (key) {
    case VK_NUMPAD0:
        GraphicsRHI.UpdateCubemapIndex();
        break;
    case VK_NUMPAD1:
        break;
    case VK_NUMPAD2:
        break;
    case VK_NUMPAD3:
        break;
    case VK_NUMPAD4:
        GraphicsRHI.UpdateGlobalLightPosition(XMFLOAT4(0.0f, 0.0f, 100.0f, 1.0f));
        break;
    case VK_NUMPAD5:
        GraphicsRHI.UpdateGlobalLightPosition(XMFLOAT4(0.0f, -100.0f, 0.0f, 1.0f));
        break;
    case VK_NUMPAD6:
        GraphicsRHI.UpdateGlobalLightPosition(XMFLOAT4(0.0f, 0.0f, -100.0f, 1.0f));
        break;
    case VK_NUMPAD7:
        GraphicsRHI.UpdateGlobalLightPosition(XMFLOAT4(-100.0f, 0.0f, 0.0f, 1.0f));
        break;
    case VK_NUMPAD8:
        GraphicsRHI.UpdateGlobalLightPosition(XMFLOAT4(0.0f, 100.0f, 0.0f, 1.0f));
        break;
    case VK_NUMPAD9:
        GraphicsRHI.UpdateGlobalLightPosition(XMFLOAT4(100.0f, 0.0f, 0.0f, 1.0f));
        break;
    default:
        ASSERT(false, "RECIEVE UNKOWN NUMPAD INPUT! ERROR!");
    }
}

void My::D3d12GraphicsManager::FunctionKeyDown(int64_t key)
{
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.UpdateCameraParams(key);
}
void My::D3d12GraphicsManager::FunctionKeyUp(int64_t key)
{

}

void My::D3d12GraphicsManager::NumPadKeyUp(int64_t key)
{

}

bool My::D3d12GraphicsManager::GenerateInputLayoutType(uint32_t& InputLayoutType, const std::string& name)
{
    if (name == "POSITION") {
        InputLayoutType |= (1 << D3dGraphicsCore::kPos);
    }
    else if (name == "NORMAL") {
        InputLayoutType |= (1 << D3dGraphicsCore::kNormal);
    }
    else if (name == "TANGENT") {
        InputLayoutType |= (1 << D3dGraphicsCore::kTangent);
    }
    else if (name == "TEXCOORD0") {
        InputLayoutType |= (1 << D3dGraphicsCore::kTexcoord0);
    }
    else if (name == "TEXCOORD1") {
        InputLayoutType |= (1 << D3dGraphicsCore::kTexcoord1);
    }
    else {
        ASSERT(false, "Set InputLayout Type Error!")
        return false;
    }
    return true;
}

int My::D3d12GraphicsManager::InitializeD3dImGUI()
{
    D3dGraphicsCore::DescriptorHandle ImGUIHandle;
    int HeapIdx = -1;
    ImGUIHandle = D3dGraphicsCore::AllocateFromDescriptorHeap(1, HeapIdx);
    if (HeapIdx == -1) {
        ASSERT(false, "Allocate Descriptor From Heap For ImGUI Failed! ERROR!");
        return -1;
    }
    
    ImGui_ImplDX12_Init(D3dGraphicsCore::g_Device, SWAP_CHAIN_BUFFER_COUNT, D3dGraphicsCore::g_SwapChainFormat,
        D3dGraphicsCore::g_DescriptorHeaps[HeapIdx]->GetHeapPointer(),
        // You'll need to designate a descriptor from your descriptor heap for Dear ImGui to use internally for its font texture's SRV
        ImGUIHandle,
        ImGUIHandle);

    return 0;
}

void My::D3d12GraphicsManager::StartGUIFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(); // Show demo window! :)
}

void My::D3d12GraphicsManager::EndGUIFrame()
{
    ImGui::EndFrame();
    ImGui::Render();
}