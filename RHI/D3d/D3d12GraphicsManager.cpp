#include <objbase.h>
#include <d3dcompiler.h>
#include <filesystem>
#include "D3d12GraphicsManager.hpp"
#include "WindowsApplication.hpp"
#include "D3d12Application.hpp"
#include "PhysicsManager.hpp"
#include "SceneObject.hpp"
#include "StructureSettings.h"
#include "GraphicsStructure.h"
#include "ShaderSource.h"
#include "GraphicsStructure.h"
#include "D3d_Helper.hpp"
#include "Core/Resource/DDSTextureLoader.h"

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
    Clear();
    GraphicsManager::Finalize();
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.Finalize();
}

void My::D3d12GraphicsManager::Clear()
{
    if (m_IBLResource) {
        for (auto it = m_IBLResource->IBLImages.begin(); it != m_IBLResource->IBLImages.end(); it++) {
            it->second->pDiffuse->Destroy();
            it->second->pSpecular->Destroy();
            it->second->pDiffuse.reset();
            it->second->pSpecular.reset();
            it->second.reset();
        }
        m_IBLResource->BRDF_LUT_Image->Destroy();
        m_IBLResource->BRDF_LUT_Image.reset();
        m_IBLResource->IBLImages.clear();
    }
    m_VecIndexBuffer.clear();
    m_VecVertexBuffer.clear();
    m_VecTexture.clear();
    m_BatchHandleStatus.clear();
    m_FixedHandleStatus.clear();
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

        std::shared_ptr<D3dDrawBatchContext> dbc = std::make_shared<D3dDrawBatchContext>();
        dbc->m_PrimitiveType = pMesh->GetPrimitiveType();
        dbc->BatchIndex = batch_index;
        dbc->m_inputlayout = 0;

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
                GenerateInputLayoutType(dbc->m_inputlayout, vtArray.GetAttributeName());
                int elementCountPerArray = vtArray.GetDataSize() / vtArray.GetVertexCount() / (vtArray.GetDataSize() / vtArray.GetElementCount());
                float* _pData = (float*)vtArray.GetData();
                for (int j = 0; j < elementCountPerArray; j++) {
                    pVertexData[i * vertexPerCount + _dataCount] = _pData[i * elementCountPerArray + j];
                    _dataCount++;
                }
            }
            ASSERT(_dataCount == vertexPerCount, "Scene Convert Vertex To Data ERROR!");
        }
        std::unique_ptr<D3dGraphicsCore::StructuredBuffer> vbuffer = std::make_unique<D3dGraphicsCore::StructuredBuffer>();
        vbuffer->Create(L"Vertex Buffer", elementCount, sizeof(float) * vertexPerCount, pVertexData);
        dbc->m_vertex_buffer_index = m_VecVertexBuffer.size();
        m_VecVertexBuffer.push_back(std::move(vbuffer));

        std::unique_ptr<D3dGraphicsCore::ByteAddressBuffer> ibuffer = std::make_unique<D3dGraphicsCore::ByteAddressBuffer>();
        switch (pMesh->GetIndexArray(0).GetIndexType()) {
        case My::kIndexDataTypeInt8:
        {
            uint8_t* pIndexData = new uint8_t[pMesh->GetIndexArray(0).GetIndexCount()];
            uint8_t* _pData = (uint8_t*)pMesh->GetIndexArray(0).GetData();
            memcpy(pIndexData, _pData, sizeof(uint8_t) * pMesh->GetIndexArray(0).GetIndexCount());
            //for (int i = 0; i < pMesh->GetIndexArray(0).GetIndexCount(); i++) {
            //    pIndexData[i] = _pData[i];
            //}
            ibuffer->Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint8_t), pIndexData);
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
            ibuffer->Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint16_t), pIndexData);
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
            ibuffer->Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint32_t), pIndexData);
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
            ibuffer->Create(L"Index Buffer", pMesh->GetIndexArray(0).GetIndexCount(), sizeof(uint64_t), pIndexData);
        }
        break;
        default:
            ASSERT(false, "Convert Index Type ERROR!");
            break;
        }
        dbc->m_indice_buffer_index = m_VecIndexBuffer.size();
        m_VecIndexBuffer.push_back(std::move(ibuffer));
        dbc->m_index_count_per_instance = pMesh->GetIndexArray(0).GetIndexCount();

        //-----------------------------------------Material-----------------------------------------//
        ASSERT(GeometryNode->GetMaterialCount() == 1 || GeometryNode->GetMaterialCount() == 0
            , "Primitive Has More Than One Material!");
        if (GeometryNode->GetMaterialCount()) {
            auto pMaterial = scene.GetMaterial(GeometryNode->GetMaterialRef(0));

            for (int type = 0; type < My::SceneObjectMaterial::TextureType::kpbrType; type++) {
                auto TextureRef = pMaterial->GetTexture((My::SceneObjectMaterial::TextureType)type);
                // if texture has loaded its image data, then create gpu data
                size_t cpuHandle = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
                if (TextureRef) {
                    Image& img = TextureRef->GetTextureImage();
                    std::shared_ptr<D3dGraphicsCore::GpuTexture> pTex = std::make_shared<D3dGraphicsCore::GpuTexture>();
                    DXGI_FORMAT format;
                    GetDXGIFormat(img.format, format);
                    pTex->Create2D(img.pitch, img.Width, img.Height, DXGI_FORMAT_R8G8B8A8_UNORM, img.data);
                    cpuHandle = m_VecTexture.size();
                    m_VecTexture.push_back(std::move(pTex));
                } else {                  
                    switch (type) {
                    case My::SceneObjectMaterial::kBaseColor:
                    {
                        cpuHandle = m_VecTexture.size();
                        std::shared_ptr<D3dGraphicsCore::GpuTexture> pTex(D3dGraphicsCore::g_DefaultBaseColorTexture);
                        m_VecTexture.push_back(std::move(pTex));
                    }
                    break;
                    case My::SceneObjectMaterial::kMetallicRoughness:
                    {
                        cpuHandle = m_VecTexture.size();
                        std::shared_ptr<D3dGraphicsCore::GpuTexture> pTex(D3dGraphicsCore::g_DefaultRoughnessMetallicTexture);
                        m_VecTexture.push_back(std::move(pTex));
                    }
                    break;
                    case My::SceneObjectMaterial::kOcclusion:
                    {
                        cpuHandle = m_VecTexture.size();
                        std::shared_ptr<D3dGraphicsCore::GpuTexture> pTex(D3dGraphicsCore::g_DefaultOcclusionTexture);
                        m_VecTexture.push_back(std::move(pTex));
                    }
                    break;
                    case My::SceneObjectMaterial::kEmissive:
                    {
                        cpuHandle = m_VecTexture.size();
                        std::shared_ptr<D3dGraphicsCore::GpuTexture> pTex(D3dGraphicsCore::g_DefaultEmissiveTexture);
                        m_VecTexture.push_back(std::move(pTex));
                    }
                    break;
                    case My::SceneObjectMaterial::kNormal:
                    {
                        cpuHandle = m_VecTexture.size();
                        std::shared_ptr<D3dGraphicsCore::GpuTexture> pTex(D3dGraphicsCore::g_DefaultNormalTexture);
                        m_VecTexture.push_back(std::move(pTex));
                    }
                    break;
                    default:
                        ASSERT(false, "NO BLANK DESCRIPTOR TO FILL HEAP,ERROR!");
                        break;
                    }
                }

                switch (type)
                {
                case My::SceneObjectMaterial::kBaseColor:
                {
                    dbc->Material.DiffuseMap.Handle = cpuHandle;
                }
                break;
                case My::SceneObjectMaterial::kMetallicRoughness:
                {
                    dbc->Material.MetallicRoughnessMap.Handle = cpuHandle;
                }
                break;
                case My::SceneObjectMaterial::kOcclusion:
                {
                    dbc->Material.AmbientOcclusionMap.Handle = cpuHandle;
                }
                break;
                case My::SceneObjectMaterial::kEmissive:
                {
                    dbc->Material.EmissiveMap.Handle = cpuHandle;
                }
                break;
                case My::SceneObjectMaterial::kNormal:
                {
                    dbc->Material.NormalMap.Handle = cpuHandle;
                }
                break;
                default:
                    ASSERT(false, "Material Type ERROR!");
                    break;
                }

                // texture transform
                My::TextureTransform trans = pMaterial->GetTextureTransform(My::SceneObjectMaterial::TextureType(type));
                switch (type) {
                case My::SceneObjectMaterial::kBaseColor:
                {
                    dbc->BaseColorTextureTransform[0] = trans.offset[0];
                    dbc->BaseColorTextureTransform[1] = trans.offset[1];
                    dbc->BaseColorTextureTransform[2] = trans.scale[0];
                    dbc->BaseColorTextureTransform[3] = trans.scale[1];
                    dbc->BaseColorRotation = trans.rotation;
                }
                break;
                case My::SceneObjectMaterial::kMetallicRoughness:
                {
                    dbc->MetallicRoughnessTextureTransform[0] = trans.offset[0];
                    dbc->MetallicRoughnessTextureTransform[1] = trans.offset[1];
                    dbc->MetallicRoughnessTextureTransform[2] = trans.scale[0];
                    dbc->MetallicRoughnessTextureTransform[3] = trans.scale[1];
                    dbc->MetallicRoughnessRotation = trans.rotation;
                }
                break;
                case My::SceneObjectMaterial::kOcclusion:
                {
                    dbc->OcclusionTextureTransform[0] = trans.offset[0];
                    dbc->OcclusionTextureTransform[1] = trans.offset[1];
                    dbc->OcclusionTextureTransform[2] = trans.scale[0];
                    dbc->OcclusionTextureTransform[3] = trans.scale[1];
                    dbc->OcclusionRotation = trans.rotation;
                }
                break;
                case My::SceneObjectMaterial::kEmissive:
                {
                    dbc->EmissiveTextureTransform[0] = trans.offset[0];
                    dbc->EmissiveTextureTransform[1] = trans.offset[1];
                    dbc->EmissiveTextureTransform[2] = trans.scale[0];
                    dbc->EmissiveTextureTransform[3] = trans.scale[1];
                    dbc->EmissiveRotation = trans.rotation;
                }
                break;
                case My::SceneObjectMaterial::kNormal:
                {
                    dbc->NormalTextureTransform[0] = trans.offset[0];
                    dbc->NormalTextureTransform[1] = trans.offset[1];
                    dbc->NormalTextureTransform[2] = trans.scale[0];
                    dbc->NormalTextureTransform[3] = trans.scale[1];
                    dbc->NormalRotation = trans.rotation;
                }
                break;
                default:
                    ASSERT(false, "TEXTURE TRANSFORM ERROR!");
                    break;
                }
            }

            // factor param / 
            dbc->BaseColorFactor[0] = pMaterial->GetBaseColorFactor()[0];
            dbc->BaseColorFactor[1] = pMaterial->GetBaseColorFactor()[1];
            dbc->BaseColorFactor[2] = pMaterial->GetBaseColorFactor()[2];
            dbc->BaseColorFactor[3] = pMaterial->GetBaseColorFactor()[3];

            dbc->EmissiveFactor[0] = pMaterial->GetEmissivsFactor()[0];
            dbc->EmissiveFactor[1] = pMaterial->GetEmissivsFactor()[1];
            dbc->EmissiveFactor[2] = pMaterial->GetEmissivsFactor()[2];

            dbc->MetallicRoughnessFactor[0] = pMaterial->GetMetallicFactor();
            dbc->MetallicRoughnessFactor[1] = pMaterial->GetRoughnessFactor();

            dbc->NormalTextureScale = pMaterial->GetNormalScaleFactor();
        }

        //transform
        dbc->ModelMatrix = *GeometryNode->GetCalculatedTransform().get();
        dbc->Node = _it.second;
        

        batch_index++;
        for (auto& frame : m_Frames) {
            frame.BatchContexts.push_back(dbc);
        }
    }
}

void My::D3d12GraphicsManager::initializeSkybox(const Scene& scene)
{
    m_IBLResource = std::make_unique<IBLImageResource>();
    int HeapIdx = -1;

    std::vector<std::string> IBLFiles;
    std::string IBLLoadDirectory = _IBL_RESOURCE_DIRECTORY;
    std::string specular_suffix = "_specularIBL.dds";
    size_t specular_offset = specular_suffix.size();
    std::string diffuse_suffix = "_diffuseIBL.dds";
    size_t diffuse_offset = diffuse_suffix.size();

    std::unordered_map<std::string, int> ImageName;

    int fileCount = std::count_if(std::filesystem::directory_iterator(IBLLoadDirectory), std::filesystem::directory_iterator{}, (bool (*)(const std::filesystem::path&))std::filesystem::is_regular_file);

    for (auto dir : std::filesystem::recursive_directory_iterator(IBLLoadDirectory)) {
        if (!dir.is_directory()) {
            if (dir.path().extension().string() == ".dds") {
                std::string path = dir.path().string();
                if (path.substr(path.size() - specular_offset, specular_offset) == specular_suffix) {
                    LoadIBLDDSImage(path, specular_suffix, ImageName);
                }
                else if (path.substr(path.size() - diffuse_offset, diffuse_offset) == diffuse_suffix) {
                    LoadIBLDDSImage(path, diffuse_suffix, ImageName);
                }
            }
        }
    }
    m_IBLResource->IBLImageCount = fileCount;

    D3dGraphicsCore::DescriptorHandle Handle;
    Handle = D3dGraphicsCore::AllocateFromDescriptorHeap(1, HeapIdx);
    // Load BRDF_LUT Image
    uint64_t size = 0;
    std::string BRDF_LUT_Name = std::string(_IBL_RESOURCE_DIRECTORY) + "/BRDF/" + "BRDF_LUT.dds";
    m_IBLResource->BRDF_LUT_Image = std::make_unique<D3dGraphicsCore::GpuTexture>();
    HRESULT hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, My::UTF8ToWideString(BRDF_LUT_Name).c_str(), size, false,
        m_IBLResource->BRDF_LUT_Image->GetAddressOf(), Handle);
    if (FAILED(hr)) {
        ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
    }
}


void My::D3d12GraphicsManager::LoadIBLDDSImage(std::string& ImagePath, std::string& suffix, std::unordered_map<std::string, int>& ImageName)
{
    std::string specular_suffix = "_specularIBL.dds";
    std::string diffuse_suffix = "_diffuseIBL.dds";

    size_t suffix_offset = suffix.size();
    size_t pos = std::max(ImagePath.find_last_of('/'), ImagePath.find_last_of('\\'));
    std::string imageName = ImagePath.substr(pos + 1);
    imageName = imageName.substr(0, imageName.size() - suffix_offset);

    if (ImageName.find(imageName) != ImageName.end()) {
        return;
    }

    std::unique_ptr<D3dGraphicsCore::GpuTexture> pSpecularTex, pDiffuseTex;

    pSpecularTex = std::make_unique<D3dGraphicsCore::GpuTexture>();
    pDiffuseTex = std::make_unique<D3dGraphicsCore::GpuTexture>();

    uint64_t size = 0;
    std::string specularImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + specular_suffix;
    std::string diffuseImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + diffuse_suffix;
    D3dGraphicsCore::DescriptorHandle Handle;
    int HeapIdx = -1;
    Handle = D3dGraphicsCore::AllocateFromDescriptorHeap(1, HeapIdx);
    DescriptorHeapHandleInfo status{ HeapIdx, Handle };
    m_FixedHandleStatus.emplace("Skybox", status);

    HRESULT hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, My::UTF8ToWideString(specularImage).c_str(), size, false,
        pSpecularTex->GetAddressOf(), Handle);
    if (FAILED(hr)) {
        ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
    }

    Handle = D3dGraphicsCore::AllocateFromDescriptorHeap(1, HeapIdx);
    hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, My::UTF8ToWideString(diffuseImage).c_str(), size, false,
        pDiffuseTex->GetAddressOf(), Handle);
    if (FAILED(hr)) {
        ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
    }

    std::unique_ptr<IBLImageMap> map = std::make_unique<IBLImageMap>();
    map->name = imageName;
    map->pSpecular = std::move(pSpecularTex);
    map->pDiffuse = std::move(pDiffuseTex);
    m_IBLResource->IBLImages.emplace(m_IBLResource->IBLImages.size(), std::move(map));

    ImageName.emplace(imageName, m_IBLResource->IBLImages.size());
}
void My::D3d12GraphicsManager::initializeFixedHandle()
{
    D3dGraphicsCore::DescriptorHandle Handle;
    int HeapIdx = -1;
    Handle = D3dGraphicsCore::AllocateFromDescriptorHeap(1, HeapIdx);
    DescriptorHeapHandleInfo status{ HeapIdx, Handle };
    m_FixedHandleStatus.emplace("ColorBuffer", status);
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

    DescriptorHeapHandleInfo status{ HeapIdx, ImGUIHandle };
    m_FixedHandleStatus.emplace("Gui", status);
    return 0;
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
        
        break;
    case VK_NUMPAD1:
        break;
    case VK_NUMPAD2:
        break;
    case VK_NUMPAD3:
        break;
    case VK_NUMPAD4:
        
        break;
    case VK_NUMPAD5:
        
        break;
    case VK_NUMPAD6:
        
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
        InputLayoutType |= (1 << kPos);
    }
    else if (name == "NORMAL") {
        InputLayoutType |= (1 << kNormal);
    }
    else if (name == "TANGENT") {
        InputLayoutType |= (1 << kTangent);
    }
    else if (name == "TEXCOORD0") {
        InputLayoutType |= (1 << kTexcoord0);
    }
    else if (name == "TEXCOORD1") {
        InputLayoutType |= (1 << kTexcoord1);
    }
    else {
        ASSERT(false, "Set InputLayout Type Error!")
        return false;
    }
    return true;
}

void My::D3d12GraphicsManager::BeginFrame(Frame& frame)
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    //ImGui::ShowDemoWindow(); // Show demo window! :)

    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.UpdateConstants(frame);
    if (!m_bInitialized) {
        uint32_t NumDest = 5;
        int HeapIndex = -1;
        for (auto& batch : frame.BatchContexts) {
            D3dDrawBatchContext* d3dbatch = reinterpret_cast<D3dDrawBatchContext*>(batch.get());
            D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle[] = {
                m_VecTexture[d3dbatch->Material.DiffuseMap.Handle]->GetSRV(),
                m_VecTexture[d3dbatch->Material.MetallicRoughnessMap.Handle]->GetSRV(),
                m_VecTexture[d3dbatch->Material.AmbientOcclusionMap.Handle]->GetSRV(),
                m_VecTexture[d3dbatch->Material.EmissiveMap.Handle]->GetSRV(),
                m_VecTexture[d3dbatch->Material.NormalMap.Handle]->GetSRV()
            };
            const uint32_t NumSrc = _countof(CpuHandle);
            uint32_t pArray[NumSrc];
            for (int i = 0; i < NumSrc; i++) {
                pArray[i] = 1;
            }
            D3dGraphicsCore::DescriptorHandle GpuHandle = D3dGraphicsCore::AllocateFromDescriptorHeap(NumSrc, HeapIndex);
            D3dGraphicsCore::g_Device->CopyDescriptors(1, &GpuHandle, &NumDest, NumSrc, CpuHandle, pArray, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            DescriptorHeapHandleInfo status{ HeapIndex, GpuHandle };
            m_BatchHandleStatus.emplace(d3dbatch->BatchIndex, status);
        }

        m_bInitialized = true;
    }

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> HandleVec;
    HandleVec.push_back(D3dGraphicsCore::g_SceneColorBuffer.GetSRV());
    CopyDescriptors(m_FixedHandleStatus["ColorBuffer"].Handle, HandleVec, 1);
}

void My::D3d12GraphicsManager::EndFrame(Frame& frame)
{
    m_nFrameIndex = (m_nFrameIndex + 1) % MAX_FRAME_COUNT;
    ImGui::EndFrame();
    ImGui::Render();
}

void My::D3d12GraphicsManager::DrawBatch(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.PrepareBatch();
    for (auto& batch : frame.BatchContexts) {
        D3dDrawBatchContext* d3dbatch = reinterpret_cast<D3dDrawBatchContext*>(batch.get());
        GraphicsRHI.DrawBatch(frame, d3dbatch, m_VecVertexBuffer[d3dbatch->BatchIndex].get(), m_VecIndexBuffer[d3dbatch->BatchIndex].get(),
            m_BatchHandleStatus,
            D3dGraphicsCore::g_BaseDescriptorHeap[m_FixedHandleStatus["Skybox"].HeapIndex].GetHeapPointer(),
            m_FixedHandleStatus["Skybox"].Handle);
    }
}

void My::D3d12GraphicsManager::DrawSkybox(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.DrawSkybox(frame, D3dGraphicsCore::g_BaseDescriptorHeap[m_FixedHandleStatus["Skybox"].HeapIndex].GetHeapPointer(),
        m_FixedHandleStatus["Skybox"].Handle,
        m_IBLResource->IBLImages[0]->pSpecular.get(),
        m_IBLResource->SpecularIBLRange, m_IBLResource->SpecularIBLBias);
}

void My::D3d12GraphicsManager::DrawGui(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.DrawGui(frame);
}

void My::D3d12GraphicsManager::DrawPresent(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.DrawPresent(frame, m_FixedHandleStatus["ColorBuffer"].Handle, m_FixedHandleStatus["ColorBuffer"].HeapIndex);
}

void My::D3d12GraphicsManager::BeginSubPass(std::string PassName)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.BeginSubPass(PassName);
}

void My::D3d12GraphicsManager::EndSubPass()
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.EndSubPass();
}