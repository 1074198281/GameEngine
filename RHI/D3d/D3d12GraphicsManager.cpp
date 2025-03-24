#include <objbase.h>
#include <d3dcompiler.h>
#include <filesystem>
#include <tuple>
#include "D3d12GraphicsManager.hpp"
#include "D3d12Application.hpp"
#include "D3d12RHI.h"
#include "WindowsApplication.hpp"
#include "PhysicsManager.hpp"
#include "SceneObject.hpp"
#include "StructureSettings.h"
#include "GraphicsStructure.h"
#include "D3d_Helper.hpp"
#include "CommonDefine.h"
#include "Core/Resource/DDSTextureLoader.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

int My::D3d12GraphicsManager::Initialize()
{
    int result = 0;
    GraphicsManager::Initialize();

    result = InitializeD3dImGUI();

    m_pLightManager = std::make_unique<D3d12LightManager>(reinterpret_cast<BaseApplication*>(m_pApp));
    m_IBLResource = std::make_unique<SResourceIBLImage>();
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
    m_VecIndexBuffer.clear();
    m_VecVertexBuffer.clear();

    if (m_IBLResource) {
        for (auto it = m_IBLResource->IBLImages.begin(); it != m_IBLResource->IBLImages.end(); it++) {
            if (it->second->pDiffuse) {
                it->second->pDiffuse->pTexture->Destroy();
                it->second->pDiffuse.reset();
            }
            if (it->second->pSpecular) {
                it->second->pSpecular->pTexture->Destroy();
                it->second->pSpecular.reset();
            }
            
            it->second.reset();
        }
        m_IBLResource->BRDF_LUT_Image->Destroy();
        m_IBLResource->BRDF_LUT_Image.reset();
        m_IBLResource->IBLImages.clear();
    }

    m_BatchTextureResource.clear();
    m_PixelBufferResources.clear();
    m_pLightManager->Clear();

    m_bInitialized = false;
}

void My::D3d12GraphicsManager::Resize(uint32_t width, uint32_t height)
{
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.Resize(width, height);

    ResizeFrameBuffer();
}

void My::D3d12GraphicsManager::ResizeFrameBuffer()
{
    if (m_PixelBufferResources.find("OverlaySrc") != m_PixelBufferResources.end()) {
        m_PixelBufferResources["OverlaySrc"]->pGpuBuffer->Destroy();
        m_PixelBufferResources["OverlaySrc"]->pGpuBuffer->Create(L"OverlaySrc", D3dGraphicsCore::g_DisplayWidth, D3dGraphicsCore::g_DisplayHeight, 1, D3dGraphicsCore::g_SceneColorBufferFormat);
    }
    if (m_PixelBufferResources.find("OverlayDes") != m_PixelBufferResources.end()) {
        m_PixelBufferResources["OverlayDes"]->pGpuBuffer->Destroy();
        m_PixelBufferResources["OverlayDes"]->pGpuBuffer->Create(L"OverlayDes", D3dGraphicsCore::g_DisplayWidth, D3dGraphicsCore::g_DisplayHeight, 1, D3dGraphicsCore::g_SceneColorBufferFormat);
    }

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
            std::cout << "[D3d12 Initialize Geometry] Mesh Is Empty!" << std::endl;
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
        float* pVertexData = new float[(uint64_t)elementCount * vertexPerCount];

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
            std::cout << "[D3d12 Initialize Geometry] Convert Index Type ERROR!" << std::endl;
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

            std::unique_ptr<SResourceBatchInfo> pRe = std::make_unique<SResourceBatchInfo>();
            pRe->name = pMaterial->GetName();
            pRe->batch_index = batch_index;
            for (int type = 0; type < My::SceneObjectMaterial::TextureType::kpbrType; type++) {
                auto TextureRef = pMaterial->GetTexture((My::SceneObjectMaterial::TextureType)type);
                // if texture has loaded its image data, then create gpu data
                size_t cpuHandle = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
                std::shared_ptr<D3dGraphicsCore::GpuTexture> pTex = std::make_shared<D3dGraphicsCore::GpuTexture>();
                std::shared_ptr<SResourceTextureInfo> pTexInfo = std::make_shared<SResourceTextureInfo>();
                if (TextureRef) {
                    Image& img = TextureRef->GetTextureImage();
                    DXGI_FORMAT format;
                    GetDXGIFormat(img.format, format);
                    pTex->Create2D(img.pitch, img.Width, img.Height, DXGI_FORMAT_R8G8B8A8_UNORM, img.data);
                    
                    cpuHandle = pTex->GetSRV().ptr;
                    pTexInfo->iWidth = img.Width;
                    pTexInfo->iHeight = img.Height;
                    pTexInfo->iPitchOrLinearSize = img.pitch;
                    pTexInfo->iDepth = 1;
                    pTexInfo->iMipMapCount = 0;
                    pTexInfo->iSize = img.data_size;
                    pTexInfo->texture_name = TextureRef->GetName();
                } else {                  
                    switch (type) {
                    case My::SceneObjectMaterial::kBaseColor:
                    {
                        cpuHandle = D3dGraphicsCore::g_DefaultBaseColorTexture->GetSRV().ptr;
                    }
                    break;
                    case My::SceneObjectMaterial::kMetallicRoughness:
                    {
                        cpuHandle = D3dGraphicsCore::g_DefaultRoughnessMetallicTexture->GetSRV().ptr;
                    }
                    break;
                    case My::SceneObjectMaterial::kOcclusion:
                    {
                        cpuHandle = D3dGraphicsCore::g_DefaultOcclusionTexture->GetSRV().ptr;
                    }
                    break;
                    case My::SceneObjectMaterial::kEmissive:
                    {
                        cpuHandle = D3dGraphicsCore::g_DefaultEmissiveTexture->GetSRV().ptr;
                    }
                    break;
                    case My::SceneObjectMaterial::kNormal:
                    {
                        cpuHandle = D3dGraphicsCore::g_DefaultNormalTexture->GetSRV().ptr;
                    }
                    break;
                    default:
                        ASSERT(false, "NO BLANK DESCRIPTOR TO FILL HEAP,ERROR!");
                        std::cout << "[D3d12 Initialize Geometry] NO BLANK DESCRIPTOR TO FILL HEAP,ERROR!" << std::endl;
                        break;
                    }
                }

                switch (type)
                {
                case My::SceneObjectMaterial::kBaseColor:
                {
                    dbc->Material.DiffuseMap.handle = cpuHandle;
                }
                break;
                case My::SceneObjectMaterial::kMetallicRoughness:
                {
                    dbc->Material.MetallicRoughnessMap.handle = cpuHandle;
                }
                break;
                case My::SceneObjectMaterial::kOcclusion:
                {
                    dbc->Material.AmbientOcclusionMap.handle = cpuHandle;
                }
                break;
                case My::SceneObjectMaterial::kEmissive:
                {
                    dbc->Material.EmissiveMap.handle = cpuHandle;
                }
                break;
                case My::SceneObjectMaterial::kNormal:
                {
                    dbc->Material.NormalMap.handle = cpuHandle;
                }
                break;
                default:
                    ASSERT(false, "Material Type ERROR!");
                    std::cout << "[D3d12 Initialize Geometry] Material Type ERROR!" << std::endl;
                    break;
                }

                if (pTex) {
                    // if pTex exists, then use external texture, save its info
                    pTexInfo->pTexture = std::move(pTex);
                    pRe->texture_info_flag |= (1 << type);
                    pRe->texture_info.push_back(std::move(pTexInfo));
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
                    std::cout << "[D3d12 Initialize Geometry] TEXTURE TRANSFORM ERROR!" << std::endl;
                    break;
                }
            }
            m_BatchTextureResource.emplace(batch_index, std::move(pRe));

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
        else {
            dbc->Material.DiffuseMap.handle = D3dGraphicsCore::g_DefaultBaseColorTexture->GetSRV().ptr;

            dbc->Material.MetallicRoughnessMap.handle = D3dGraphicsCore::g_DefaultRoughnessMetallicTexture->GetSRV().ptr;

            dbc->Material.AmbientOcclusionMap.handle = D3dGraphicsCore::g_DefaultOcclusionTexture->GetSRV().ptr;

            dbc->Material.EmissiveMap.handle = D3dGraphicsCore::g_DefaultEmissiveTexture->GetSRV().ptr;

            dbc->Material.NormalMap.handle = D3dGraphicsCore::g_DefaultNormalTexture->GetSRV().ptr;

            My::TextureTransform trans;
            trans.offset[0] = 0;
            trans.offset[1] = 0;
            trans.scale[0] = 1;
            trans.scale[1] = 1;
            trans.rotation = 0;
            dbc->BaseColorTextureTransform[0] = trans.offset[0];
            dbc->BaseColorTextureTransform[1] = trans.offset[1];
            dbc->BaseColorTextureTransform[2] = trans.scale[0];
            dbc->BaseColorTextureTransform[3] = trans.scale[1];
            dbc->BaseColorRotation = trans.rotation;

            dbc->MetallicRoughnessTextureTransform[0] = trans.offset[0];
            dbc->MetallicRoughnessTextureTransform[1] = trans.offset[1];
            dbc->MetallicRoughnessTextureTransform[2] = trans.scale[0];
            dbc->MetallicRoughnessTextureTransform[3] = trans.scale[1];
            dbc->MetallicRoughnessRotation = trans.rotation;

            dbc->OcclusionTextureTransform[0] = trans.offset[0];
            dbc->OcclusionTextureTransform[1] = trans.offset[1];
            dbc->OcclusionTextureTransform[2] = trans.scale[0];
            dbc->OcclusionTextureTransform[3] = trans.scale[1];
            dbc->OcclusionRotation = trans.rotation;

            dbc->EmissiveTextureTransform[0] = trans.offset[0];
            dbc->EmissiveTextureTransform[1] = trans.offset[1];
            dbc->EmissiveTextureTransform[2] = trans.scale[0];
            dbc->EmissiveTextureTransform[3] = trans.scale[1];
            dbc->EmissiveRotation = trans.rotation;

            dbc->NormalTextureTransform[0] = trans.offset[0];
            dbc->NormalTextureTransform[1] = trans.offset[1];
            dbc->NormalTextureTransform[2] = trans.scale[0];
            dbc->NormalTextureTransform[3] = trans.scale[1];
            dbc->NormalRotation = trans.rotation;

            dbc->BaseColorFactor[0] = 1;
            dbc->BaseColorFactor[1] = 1;
            dbc->BaseColorFactor[2] = 1;
            dbc->BaseColorFactor[3] = 1;

            dbc->EmissiveFactor[0] = 1;
            dbc->EmissiveFactor[1] = 1;
            dbc->EmissiveFactor[2] = 1;

            dbc->MetallicRoughnessFactor[0] = 1;
            dbc->MetallicRoughnessFactor[1] = 1;

            dbc->NormalTextureScale = 1;
        }

        //transform
        //dbc->ModelMatrix = *GeometryNode->GetCalculatedTransform().get();
        dbc->Node = _it.second;

        batch_index++;
        for (int f = 0; f < MAX_FRAME_COUNT; f++) {
            m_Frames[f].BatchContexts.push_back(dbc);
        }
    }
}

void My::D3d12GraphicsManager::initializeSkybox(const Scene& scene)
{
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
        std::cout << "[D3d12 Load DDS] CREATE DDS FROM FILE FAILED! ERROR!" << std::endl;
    }
}

void My::D3d12GraphicsManager::initializeLight(const Scene& scene)
{
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();

    m_pLightManager->Create();

    uint32_t light_index = 0;
    for (auto _it : scene.LightNodes) {
        auto& LightNode = _it.second;
        auto& pLight = scene.GetLight(LightNode->GetSceneObjectRef());
        Matrix4X4f lightTrans = *LightNode->GetCalculatedTransform().get();

        m_pLightManager->SetPerLightInfo(light_index, LightNode, pLight, LightNode->GetSceneObjectRef());

        light_index++;
    }

    for (auto& frame : m_Frames) {
        frame.LightInfomation = m_pLightManager->GetAllLightInfoPtr();
        frame.FrameContext.LightNum = m_pLightManager->GetLightNum();
    }

    GraphicsRHI.SetLightManager(m_pLightManager.get());
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

    std::unique_ptr<SResourceIBLImageMap> map = std::make_unique<SResourceIBLImageMap>();
    map->name = imageName;
    map->pDiffuse = std::make_unique<SResourceTextureInfo>();
    map->pSpecular = std::make_unique<SResourceTextureInfo>();

    std::shared_ptr<D3dGraphicsCore::GpuTexture> pSpecularTex, pDiffuseTex;
    pSpecularTex = std::make_shared<D3dGraphicsCore::GpuTexture>();
    pDiffuseTex = std::make_shared<D3dGraphicsCore::GpuTexture>();

    uint64_t size = 0;
    std::string specularImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + specular_suffix;
    std::string diffuseImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + diffuse_suffix;
    D3dGraphicsCore::DescriptorHandle Handle;
    int HeapIdx = -1;
    Handle = D3dGraphicsCore::AllocateFromDescriptorHeap(2, HeapIdx);

    map->iHeapIndex = HeapIdx;
    map->handle = Handle;

    HRESULT hr = CreateDDSTextureInfoFromFile(D3dGraphicsCore::g_Device, My::UTF8ToWideString(specularImage).c_str(), size, false,
        pSpecularTex->GetAddressOf(), Handle, map->pDiffuse.get());
    if (FAILED(hr)) {
        ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
        std::cout << "[D3d12 Load DDS] CREATE DDS FROM FILE FAILED! ERROR!" << std::endl;
    }

    D3dGraphicsCore::OffsetDescriptorHandle(Handle);
    hr = CreateDDSTextureInfoFromFile(D3dGraphicsCore::g_Device, My::UTF8ToWideString(diffuseImage).c_str(), size, false,
        pDiffuseTex->GetAddressOf(), Handle, map->pSpecular.get());
    if (FAILED(hr)) {
        ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
        std::cout << "[D3d12 Load DDS] CREATE DDS FROM FILE FAILED! ERROR!" << std::endl;
    }
    
    map->name = imageName;
    map->pSpecular->texture_name = specularImage;
    map->pDiffuse->texture_name = diffuseImage;
    map->pSpecular->pTexture = std::move(pSpecularTex);
    map->pDiffuse->pTexture = std::move(pDiffuseTex);
    m_IBLResource->IBLImages.emplace(m_IBLResource->IBLImages.size(), std::move(map));

    ImageName.emplace(imageName, m_IBLResource->IBLImages.size());
}
void My::D3d12GraphicsManager::initializeFixedHandle()
{
    D3dGraphicsCore::DescriptorHandle Handle;
    int HeapIdx = -1;
    Handle = D3dGraphicsCore::AllocateFromDescriptorHeap(1, HeapIdx);
    std::unique_ptr<SResourceColorBufferInfo> pSceneColorBuffer = std::make_unique<SResourceColorBufferInfo>();
    pSceneColorBuffer->handle = Handle;
    pSceneColorBuffer->iHeapIndex = HeapIdx;
    pSceneColorBuffer->name = "ColorBuffer";
    pSceneColorBuffer->pGpuBuffer = std::make_shared<D3dGraphicsCore::ColorBuffer>(D3dGraphicsCore::g_SceneColorBuffer);
    m_PixelBufferResources.emplace(pSceneColorBuffer->name, std::move(pSceneColorBuffer));


    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> vecHandle;
    int HeapIndex = -1;
    std::unique_ptr<SResourceColorBufferInfo> pOverlaySrcInfo = std::make_unique<SResourceColorBufferInfo>();
    std::unique_ptr<SResourceColorBufferInfo> pOverlayDesInfo = std::make_unique<SResourceColorBufferInfo>();

    std::shared_ptr<D3dGraphicsCore::ColorBuffer> OverlaySrc = std::make_shared<D3dGraphicsCore::ColorBuffer>();
    OverlaySrc->Create(L"OverlaySrc", D3dGraphicsCore::g_DisplayWidth, D3dGraphicsCore::g_DisplayHeight, 1, D3dGraphicsCore::g_SceneColorBufferFormat);
    vecHandle.push_back(OverlaySrc->GetSRV());

    std::shared_ptr<D3dGraphicsCore::ColorBuffer> OverlayDes = std::make_shared<D3dGraphicsCore::ColorBuffer>();
    OverlayDes->Create(L"OverlayDes", D3dGraphicsCore::g_DisplayWidth, D3dGraphicsCore::g_DisplayHeight, 1, D3dGraphicsCore::g_SceneColorBufferFormat);
    vecHandle.push_back(OverlayDes->GetUAV());

    D3dGraphicsCore::DescriptorHandle handle = D3dGraphicsCore::AllocateFromDescriptorHeap(2, HeapIndex);
    D3dGraphicsCore::CopyDescriptors(handle, vecHandle, 2);

    pOverlaySrcInfo->handle = handle;
    pOverlaySrcInfo->iHeapIndex = HeapIdx;
    pOverlaySrcInfo->name = "OverlaySrc";
    pOverlaySrcInfo->pGpuBuffer = std::move(OverlaySrc);
    m_PixelBufferResources.emplace(pOverlaySrcInfo->name, std::move(pOverlaySrcInfo));

    D3dGraphicsCore::OffsetDescriptorHandle(handle);
    pOverlayDesInfo->handle = handle;
    pOverlayDesInfo->iHeapIndex = HeapIdx;
    pOverlayDesInfo->name = "OverlayDes";
    pOverlayDesInfo->pGpuBuffer = std::move(OverlayDes);
    m_PixelBufferResources.emplace(pOverlayDesInfo->name, std::move(pOverlayDesInfo));
}

int My::D3d12GraphicsManager::InitializeD3dImGUI()
{
    D3dGraphicsCore::DescriptorHandle ImGUIHandle;
    int HeapIdx = -1;
    ImGUIHandle = D3dGraphicsCore::AllocateFromDescriptorHeap(1, HeapIdx);
    if (HeapIdx == -1) {
        ASSERT(false, "Allocate Descriptor From Heap For ImGUI Failed! ERROR!");
        std::cout << "[D3d12 Init ImGUI] Allocate Descriptor From Heap For ImGUI Failed! ERROR!" << std::endl;
        return -1;
    }

    ImGui_ImplDX12_Init(D3dGraphicsCore::g_Device, SWAP_CHAIN_BUFFER_COUNT, D3dGraphicsCore::g_SwapChainFormat,
        D3dGraphicsCore::g_DescriptorHeaps[HeapIdx]->GetHeapPointer(),
        // You'll need to designate a descriptor from your descriptor heap for Dear ImGui to use internally for its font texture's SRV
        ImGUIHandle,
        ImGUIHandle);

    std::unique_ptr<SResourceColorBufferInfo> pGuiBuffer = std::make_unique<SResourceColorBufferInfo>();
    pGuiBuffer->name = "Gui";
    pGuiBuffer->handle = ImGUIHandle;
    pGuiBuffer->iHeapIndex = HeapIdx;
    pGuiBuffer->pGpuBuffer = nullptr;

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

}

void My::D3d12GraphicsManager::NumPadKeyUp(int64_t key)
{

}

void My::D3d12GraphicsManager::FunctionKeyDown(int64_t key)
{
    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.UpdateCameraParams(key);
}
void My::D3d12GraphicsManager::FunctionKeyUp(int64_t key)
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
        ASSERT(false, "Set InputLayout Type Error!");
        std::cout << "[D3d12 Layout Error] Set InputLayout Type Error!" << std::endl;
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

    UpdateD3dFrameConstants(frame);
    m_pLightManager->UpdateLight();
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.UpdateCameraConstants(frame);
    if (!m_bInitialized) {
        uint32_t NumDest = 5;
        int HeapIndex = -1;
        for (auto& batch : frame.BatchContexts) {
            D3dDrawBatchContext* d3dbatch = reinterpret_cast<D3dDrawBatchContext*>(batch.get());
            D3D12_CPU_DESCRIPTOR_HANDLE diffuseHandle, metallicRoughnessHandle,
                ambientOcclusionHandle, emissiveHandle, normalHandle;
            diffuseHandle.ptr = d3dbatch->Material.DiffuseMap.handle;
            metallicRoughnessHandle.ptr = d3dbatch->Material.MetallicRoughnessMap.handle;
            ambientOcclusionHandle.ptr = d3dbatch->Material.AmbientOcclusionMap.handle;
            emissiveHandle.ptr = d3dbatch->Material.EmissiveMap.handle;
            normalHandle.ptr = d3dbatch->Material.NormalMap.handle;
            
            D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle[] = {
                diffuseHandle,
                metallicRoughnessHandle,
                ambientOcclusionHandle,
                emissiveHandle,
                normalHandle
            };

            const uint32_t NumSrc = _countof(CpuHandle);
            uint32_t pArray[NumSrc];
            for (int i = 0; i < NumSrc; i++) {
                pArray[i] = 1;
            }
            D3dGraphicsCore::DescriptorHandle GpuHandle = D3dGraphicsCore::AllocateFromDescriptorHeap(NumSrc, HeapIndex);
            D3dGraphicsCore::g_Device->CopyDescriptors(1, &GpuHandle, &NumDest, NumSrc, CpuHandle, pArray, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
            
            auto pRe = m_BatchTextureResource.find(d3dbatch->BatchIndex);
            if (pRe != m_BatchTextureResource.end()) {
                pRe->second->handle = GpuHandle;
                pRe->second->iHeapIndex = HeapIndex;
            }
        }

        m_bInitialized = true;
    }

    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> HandleVec;
    HandleVec.push_back(D3dGraphicsCore::g_SceneColorBuffer.GetSRV());
    CopyDescriptors(m_PixelBufferResources["ColorBuffer"]->handle, HandleVec, 1);
}

void My::D3d12GraphicsManager::EndFrame(Frame& frame)
{
    m_nFrameIndex = (m_nFrameIndex + 1) % MAX_FRAME_COUNT;
    ImGui::EndFrame();
    ImGui::Render();
}

void My::D3d12GraphicsManager::SetPipelineStatus(const std::string& PSOName)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.SetPipelineStatus(PSOName);
}

void My::D3d12GraphicsManager::SetBatchResources(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.SetBatchResources();
}

void My::D3d12GraphicsManager::SetShadowResources(Frame& frame, uint8_t lightIdx)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();

    std::shared_ptr<D3dGraphicsCore::DepthBuffer> depthBuffer = m_pLightManager->GetDepthBuffer(lightIdx);
    std::shared_ptr<D3dGraphicsCore::ColorBuffer> colorBuffer = m_pLightManager->GetColorBuffer(lightIdx);
    std::shared_ptr<D3dGraphicsCore::ColorBuffer> volumnBuffer = m_pLightManager->GetVolumnBuffer(lightIdx);

    if (!colorBuffer) {
        ASSERT(!colorBuffer, "Blank Debug Color Buffer");
        std::cout << "[D3d12 Set Shadow Resource] No Debug Shadow Color Buffer!" << std::endl;
    }
    
    GraphicsRHI.SetShadowResources(frame, lightIdx, colorBuffer.get(), depthBuffer.get(), volumnBuffer.get());
}

void My::D3d12GraphicsManager::SetShadowMapDebugPresent(uint8_t lightIdx)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();

    auto colorBuffer = m_pLightManager->GetColorBuffer(lightIdx);
    GraphicsRHI.TransitionResourceState(*colorBuffer, D3D12_RESOURCE_STATE_PRESENT, true);
}

My::LightManager* My::D3d12GraphicsManager::GetLightManager()
{
    return m_pLightManager.get();
}

std::vector<std::string> My::D3d12GraphicsManager::GetSkyboxInfo()
{
    std::vector<std::string> skyboxInfo;
    for (auto& _it : m_IBLResource->IBLImages) {
        skyboxInfo.push_back(_it.second->name);
    }

    return skyboxInfo;
}

size_t My::D3d12GraphicsManager::GetSkyboxTextureGpuPtr(const std::string skyboxName, uint32_t& width, uint32_t& height)
{
    if (!m_IBLResource->IBLImages.contains(m_iSkyboxIndex)) {
        ASSERT(false, "Skybox Not Exist!");
        std::cout << "[D3d12 Get Skybox Error] Skybox Not Exist!" << std::endl;
        return 0;
    }

    return m_IBLResource->IBLImages[m_iSkyboxIndex]->handle.GetGpuPtr();
}

size_t My::D3d12GraphicsManager::GetTextureGpuPtr(const int& batch_index, int material_index, uint32_t& width, uint32_t& height, uint32_t& size)
{
    if (m_BatchTextureResource.find(batch_index) != m_BatchTextureResource.end()) {
        auto pInfo = m_BatchTextureResource[batch_index].get();
        if (pInfo->texture_info_flag & (1 << material_index)) {
            height = pInfo->texture_info[material_index]->iHeight;
            width = pInfo->texture_info[material_index]->iWidth;
            size = pInfo->texture_info[material_index]->iSize;
            auto handle = pInfo->handle;
            D3dGraphicsCore::OffsetDescriptorHandle(handle, material_index);
            return handle.GetGpuPtr();
        } else {
            ASSERT(false, "Geo %s Has No Texture. Type: %d", pInfo->name, material_index);
            std::cout << "[D3d12 Get Texture Error] Geo " << pInfo->name << "Has No Texture. Type: " << material_index << std::endl;
        }
    } else {
        ASSERT(false, "No Such Texture. Batch: %d", batch_index);
        std::cout << "[D3d12 Get Texture Error] No Such Texture. Batch: " << batch_index << std::endl;
    }
    return 0;
}

void My::D3d12GraphicsManager::UpdateD3dFrameConstants(Frame& frame) {
    auto pPhysicsManager = dynamic_cast<D3d12Application*>(m_pApp)->GetPhysicsManager();
    for (auto& dbc : frame.BatchContexts) {
        Matrix4X4f ModelTrans = *dbc->Node->GetCalculatedTransform().get();

        Matrix4X4f PhysicsTrans;
        BuildIdentityMatrix(PhysicsTrans);
        if (dbc->Node->GetRigidBody()) {
            PhysicsTrans = pPhysicsManager->GetRigidBodyTransform(dbc->Node->GetRigidBody());
            ModelTrans[0][3] = 0;
            ModelTrans[1][3] = 0;
            ModelTrans[2][3] = 0;
        }

        dbc->ModelMatrix = PhysicsTrans * ModelTrans;
    }

    auto& GraphicsRHI = reinterpret_cast<D3d12Application*>(m_pApp)->GetRHI();
}

void My::D3d12GraphicsManager::DrawBatch(Frame& frame, uint8_t lightIdx, bool castShadow, bool isDrawSkybox)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    for (auto& batch : frame.BatchContexts) {
        D3dDrawBatchContext* d3dbatch = reinterpret_cast<D3dDrawBatchContext*>(batch.get());
        std::string skyboxName = m_IBLResource->IBLImages[m_iSkyboxIndex]->name;
        ID3D12DescriptorHeap* pHeap = nullptr;
        D3dGraphicsCore::DescriptorHandle handle;
        if (m_bDrawSkybox) {
            if (m_IBLResource->IBLImages.find(m_iSkyboxIndex) != m_IBLResource->IBLImages.end()) {
                pHeap = D3dGraphicsCore::g_BaseDescriptorHeap[m_IBLResource->IBLImages[m_iSkyboxIndex]->iHeapIndex].GetHeapPointer();
                handle = m_IBLResource->IBLImages[m_iSkyboxIndex]->handle;
            } else {
                ASSERT(false, "No Such Skybox Texture, Name: %s", skyboxName);
                std::cout << "[D3d12 Draw Batch] No Such Skybox Texture, Name: " << skyboxName << std::endl;
                m_bDrawSkybox = false;
            }
        }
        if (lightIdx == -1) {
            for (int i = 0; i < m_pLightManager->GetLightNum(); i++) {
                auto pDepthBuffer = m_pLightManager->GetDepthBuffer(i);
                GraphicsRHI.TransitionResourceState(*pDepthBuffer, D3D12_RESOURCE_STATE_DEPTH_READ, true);
            }
        }
        if (d3dbatch->Node->Visible()) {
            GraphicsRHI.DrawBatch(frame, d3dbatch, m_VecVertexBuffer[d3dbatch->BatchIndex].get(), m_VecIndexBuffer[d3dbatch->BatchIndex].get(),
                m_BatchTextureResource[d3dbatch->BatchIndex]->iHeapIndex,
                m_BatchTextureResource[d3dbatch->BatchIndex]->handle,
                pHeap,
                handle, m_pLightManager, lightIdx, castShadow, m_bDrawSkybox & isDrawSkybox);
        }
    }
}

void My::D3d12GraphicsManager::DrawSkybox(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    if (m_bDrawSkybox)
    {
        std::string skyboxName = m_IBLResource->IBLImages[m_iSkyboxIndex]->name;
        ID3D12DescriptorHeap* pHeap = D3dGraphicsCore::g_BaseDescriptorHeap[m_IBLResource->IBLImages[m_iSkyboxIndex]->iHeapIndex].GetHeapPointer();
        D3dGraphicsCore::DescriptorHandle handle = m_IBLResource->IBLImages[m_iSkyboxIndex]->handle;
        GraphicsRHI.DrawSkybox(frame, pHeap,
            handle,
            m_IBLResource->IBLImages[0]->pSpecular->pTexture.get(),
            m_IBLResource->SpecularIBLRange, m_IBLResource->SpecularIBLBias);
    }
}

void My::D3d12GraphicsManager::DrawGui(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.DrawGui(frame);
}

void My::D3d12GraphicsManager::DrawPresent(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.DrawPresent(frame, m_PixelBufferResources["ColorBuffer"]->handle, m_PixelBufferResources["ColorBuffer"]->iHeapIndex);
}

void My::D3d12GraphicsManager::DrawGuassBlur(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    auto& config = dynamic_cast<D3d12Application*>(m_pApp)->GetConfiguration();

    auto& srcBuffer = m_PixelBufferResources["OverlaySrc"];
    auto& desBuffer = m_PixelBufferResources["OverlayDes"];
    ASSERT(m_PixelBufferResources["OverlayDes"]->iHeapIndex == m_PixelBufferResources["OverlaySrc"]->iHeapIndex, "Overlay Descriptors Not In Same Heap!");

    GraphicsRHI.DrawGuassBlur(frame, *desBuffer->pGpuBuffer, *srcBuffer->pGpuBuffer,
        m_PixelBufferResources["OverlayDes"]->handle, m_PixelBufferResources["OverlaySrc"]->handle, m_PixelBufferResources["OverlayDes"]->iHeapIndex);
}

void My::D3d12GraphicsManager::DrawOverlay(Frame& frame)
{
    if (m_bDrawOverlay) {
        auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();

        auto& srcBuffer = m_PixelBufferResources["OverlaySrc"];
        auto& desBuffer = m_PixelBufferResources["OverlayDes"];
        ASSERT(m_PixelBufferResources["OverlayDes"]->iHeapIndex == m_PixelBufferResources["OverlaySrc"]->iHeapIndex, "Overlay Descriptors Not In Same Heap!");


        GraphicsRHI.DrawOverlay(frame, *desBuffer->pGpuBuffer, *srcBuffer->pGpuBuffer,
            m_PixelBufferResources["OverlayDes"]->handle, m_PixelBufferResources["OverlaySrc"]->handle, m_PixelBufferResources["OverlayDes"]->iHeapIndex);
    }
}

void My::D3d12GraphicsManager::DrawVolumetricLight(Frame& frame)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
}

void My::D3d12GraphicsManager::BeginSubPass(const std::string& PassName)
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.BeginSubPass(PassName);
}

void My::D3d12GraphicsManager::EndSubPass()
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.EndSubPass();
}

void My::D3d12GraphicsManager::BeginOverlayPass()
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.BeginOverlayPass(*m_PixelBufferResources["OverlaySrc"]->pGpuBuffer, D3dGraphicsCore::g_SceneColorBuffer);
}

void My::D3d12GraphicsManager::EndOverlayPass()
{
    auto& GraphicsRHI = dynamic_cast<D3d12Application*>(m_pApp)->GetRHI();
    GraphicsRHI.EndOverlayPass(D3dGraphicsCore::g_SceneColorBuffer, *m_PixelBufferResources["OverlaySrc"]->pGpuBuffer);
}
