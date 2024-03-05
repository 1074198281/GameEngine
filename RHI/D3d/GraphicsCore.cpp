#include "GraphicsCore.h"
#include "Core/D3dGraphicsCoreManager.h"
#include "MemoryManager.hpp"
#include "GraphicsStructure.h"
#include "ShaderSource.h"
#include "D3dComponents/XMInput/XMInput.h"
#include "Core/Common/SystemTime.h"
#include "D3dComponents/XMImageLoader/XMWICImageLoader.h"
#include "Core/Resource/DDSTextureLoader.h"
#include "ShaderConstants.h"

#include "imgui_impl_dx12.h"
#include <array>
#include <filesystem>
#include <algorithm>


using namespace DirectX;

D3dGraphicsCore::CD3dGraphicsCore::CD3dGraphicsCore()
{

}

D3dGraphicsCore::CD3dGraphicsCore::~CD3dGraphicsCore()
{
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

int D3dGraphicsCore::CD3dGraphicsCore::StartUp()
{
    InitializeInputLayout();
    InitializeSamplers();
    InitializeShaderByteMap();
    InitializeBaseDescriptorHeap();
    InitializePipelineTemplates();

    SystemTime::Initialize();
    XM_Input::Initialize();
    InitializeGraphicsSettings();
    D3dGraphicsCore::WICLoader::InitializeWICLoader();
    
    LoadIBLTextures();
    InitializeDefaultTexture();

    return 0;
}

void D3dGraphicsCore::CD3dGraphicsCore::Finalize()
{
    Shutdown();
    ShutdownDisplay();
    DestroyCommonState();
    for (auto it = m_PrimitiveObjects.begin(); it != m_PrimitiveObjects.end(); it++) {
        auto p = (*it).release();
        p->VertexBuffer.Destroy();
        p->IndexBuffer.Destroy();
        for (auto itTex = p->MaterialResource.TextureResources.begin(); itTex != p->MaterialResource.TextureResources.end(); itTex++) {
            delete itTex->pTexture;
            itTex->pTexture = nullptr;
        }
    }
    if (m_IBLResource) {
        for (auto it = m_IBLResource->IBLImages.begin(); it != m_IBLResource->IBLImages.end(); it++) {
            it->second->pDiffuse->Destroy();
            it->second->pSpecular->Destroy();
            it->second->pDiffuse.reset();
            it->second->pSpecular.reset();
            it->second.reset();
        }
    }
    D3dGraphicsCore::WICLoader::FinalizeWICLoader();
    FinalizePipelineTemplates();
    FinalizeBaseDescriptorHeap();
    FinalizeShaderByteMap();
    FinalizeDefaultTexture();

    XM_Input::Shutdown();
}

void D3dGraphicsCore::CD3dGraphicsCore::Resize(uint32_t width, uint32_t height)
{
    DisplayResize(width, height);
    m_Camera.reset();
    m_CameraController.reset();
    InitializeGraphicsSettings();
}

void D3dGraphicsCore::CD3dGraphicsCore::setCoreHWND(HWND hwnd, int width, int height)
{
    g_hWnd = hwnd;
    g_DisplayWidth = width;
    g_DisplayHeight = height;

    Initialize(false);
    InitializeBuffers();
}

void D3dGraphicsCore::CD3dGraphicsCore::InitializeGraphicsSettings()
{
    if (!m_Camera) {
        m_Camera = std::make_unique<XM_Camera::Camera>();
        m_Camera->SetAspectRatio((float)g_DisplayHeight / (float)g_DisplayWidth);
        m_Camera->SetFOV(120.f);
        m_Camera->SetZRange(0.01f, 1000.0f);
        m_Camera->SetPosition(XM_Math::Vector3(0, 0, 5));
    }
    if (!m_CameraController) {
        m_CameraController = std::make_unique<XM_Camera::FlyingFPSCamera>(*m_Camera.get(), XM_Math::Vector3(0.0f, 1.0f, 0.0f));
    }
    
    m_MainViewport.Width = g_DisplayWidth;
    m_MainViewport.Height = g_DisplayHeight;
    
    /*
    * 这里minDepth为0.0，maxDepth为1.0，因为深度测试。
    */
    m_MainViewport.MinDepth = 0.0f;
    m_MainViewport.MaxDepth = 1.0f;
    m_MainViewport.TopLeftX = 0.0f;
    m_MainViewport.TopLeftY = 0.0f;

    m_MainScissor.left = 0;
    m_MainScissor.top = 0;
    m_MainScissor.right = g_DisplayWidth;
    m_MainScissor.bottom = g_DisplayHeight;

    m_GlobalLightPosition = XMFLOAT4(0.0f, 100.0f, 0.0f, 1.0f);
}

void D3dGraphicsCore::CD3dGraphicsCore::FinalizeGraphicsSettings()
{
    m_CameraController.reset(nullptr);
}

void D3dGraphicsCore::CD3dGraphicsCore::AddPrimitiveObject(std::unique_ptr<PrimitiveObject> _object)
{
    m_PrimitiveObjects.push_back(std::move(_object));
}

void D3dGraphicsCore::CD3dGraphicsCore::SetPrimitiveType(GraphicsContext& context, My::PrimitiveType Type)
{
    D3D_PRIMITIVE_TOPOLOGY d3dType;
    switch (Type) {
    case My::kPrimitiveTypeLineStrip:
    {
        d3dType = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
    }
    break;
    case My::kPrimitiveTypeTriList:
    {
        d3dType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
    break;
    default:
        ASSERT(false, "ERROR! No Configured Primitive Type!");
        break;
    }

    context.SetPrimitiveTopology(d3dType);
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdateGlobalLightPosition(XMFLOAT4 pos)
{
    m_GlobalLightPosition.x += pos.x;
    m_GlobalLightPosition.y += pos.y;
    m_GlobalLightPosition.z += pos.z;
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdateStatus()
{
    UpdateCamera();
    UpdateRenderingQueue();
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdateCamera()
{
    float DeltaTime = GetFrameTime();
    XM_Input::Update(DeltaTime);
    m_CameraController->Update(DeltaTime);
    
#ifdef _DEBUG
    if (m_Camera->m_bIsCameraMove) {
        std::cout << "Current Camera Position: X: " << m_Camera->GetPosition().GetX()
            << " Y:" << m_Camera->GetPosition().GetY()
            << " Z:" << m_Camera->GetPosition().GetZ()
            << std::endl;
    }
#endif // _DEBUG
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdateCameraParams(int64_t key)
{
    switch (key)
    {
    case VK_PRIOR:
        m_CameraController->ApplyCameraParams(XM_Camera::FlyingFPSCamera::SpeedUp);
        break;
    case VK_NEXT:
        m_CameraController->ApplyCameraParams(XM_Camera::FlyingFPSCamera::SpeedDown);
        break;
    default:
        break;
    }

    switch (key)
    {
    case 0x52:
    {
        //reset cameta position
        XM_Math::Vector3 position = XM_Math::Vector3(0.0f, 0.0f, 5.0f);
        XM_Math::Matrix3 orientation = XM_Math::Matrix3(XM_Math::Vector3(1, 0, 0), XM_Math::Vector3(0, 1, 0), XM_Math::Vector3(0, 0, 1));
        m_Camera->SetTransform(XM_Math::AffineTransform(orientation, position));
        m_Camera->Update();
    }
    break;
    default:
        break;
    }
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdateCubemapIndex()
{
    m_IBLResource->CurrentCubemapIndex = (++m_IBLResource->CurrentCubemapIndex) % (m_IBLResource->IBLImageCount / 2);
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdateRenderingQueue()
{
    if (m_IBLResource->CurrentCubemapIndex != m_IBLResource->LastCubemapIndex) {
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> image_handle_vec;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> brdf_handle_vec;
        DescriptorHandle handle = m_IBLResource->IBLFirstHandle;
        OffsetDescriptorHandle(handle, m_IBLResource->CurrentCubemapIndex * 2);
        image_handle_vec.push_back(handle);
        OffsetDescriptorHandle(handle);
        image_handle_vec.push_back(handle);
        CopyDescriptors(m_IBLResource->FirstHandle, image_handle_vec, 2);
        m_IBLResource->LastCubemapIndex = m_IBLResource->CurrentCubemapIndex;
    }

    RenderAllObjects();
    RenderCubeMap();

    DescriptorHandle Handle;
    int HeapIdx;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> HandleVec;
    HandleVec.push_back(g_SceneColorBuffer.GetSRV());
    Handle = AllocateFromDescriptorHeap(1, HeapIdx);
    CopyDescriptors(Handle, HandleVec, 1);

    GraphicsContext& gfxContext = GraphicsContext::Begin(L"Render Display Buffer");
    gfxContext.SetRootSignature(g_PresentRootSignature);
    gfxContext.SetPipelineState(g_PresentPSO);
    gfxContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gfxContext.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
    gfxContext.TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_RENDER_TARGET);
    gfxContext.SetViewportAndScissor(m_MainViewport, m_MainScissor);
    gfxContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_DescriptorHeaps[HeapIdx]->GetHeapPointer());
    gfxContext.SetDescriptorTable(0, Handle);
    gfxContext.SetRenderTarget(g_DisplayBuffer[g_CurrentBuffer].GetRTV());
    gfxContext.Draw(3);

    {
        gfxContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_DescriptorHeaps[0]->GetHeapPointer());
        ImGui::Render();
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), gfxContext.GetCommandList());
    }

    gfxContext.TransitionResource(g_DisplayBuffer[g_CurrentBuffer], D3D12_RESOURCE_STATE_PRESENT, true);
    gfxContext.Finish();
}

void D3dGraphicsCore::CD3dGraphicsCore::UpdatePresent()
{
    D3dGraphicsCore::Present();

    g_CommandManager.IdleGPU();
    InitializeBuffers();
}

void D3dGraphicsCore::CD3dGraphicsCore::RenderAllObjects()
{
    GraphicsContext& gfxContext = GraphicsContext::Begin(L"Scene Render");

    //g_SceneColorBuffer.SetClearColor(D3dColor::g_ColorBlack);
    gfxContext.TransitionResource(g_DepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
    gfxContext.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
    gfxContext.SetViewportAndScissor(m_MainViewport, m_MainScissor);

    gfxContext.SetRenderTarget(g_SceneColorBuffer.GetRTV(), g_DepthBuffer.GetDSV());
    gfxContext.ClearColor(g_SceneColorBuffer);
    gfxContext.ClearDepth(g_DepthBuffer);


    for (auto it = m_PrimitiveObjects.begin(); it != m_PrimitiveObjects.end(); it++) {
        if ((*it)->MaterialResource.FirstHandle.IsNull()) {
            //没有材质，透明
            continue;
        }
        SetPrimitiveType(gfxContext, (*it)->PrimitiveType);
        gfxContext.SetRootSignature(g_TemplateRootSignature);

        gfxContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            g_DescriptorHeaps[(*it)->MaterialResource.DescriptorHeapIndex]->GetHeapPointer());

        // Material Constants --  register 0 / cb0
        {
            MaterialConstants MatCbv;
            memset(&MatCbv, 0, sizeof(MaterialConstants));
            MatCbv.BaseColorFactor[0] = (*it)->MaterialResource.BaseColorFactor[0];
            MatCbv.BaseColorFactor[1] = (*it)->MaterialResource.BaseColorFactor[1];
            MatCbv.BaseColorFactor[2] = (*it)->MaterialResource.BaseColorFactor[2];
            MatCbv.BaseColorFactor[3] = (*it)->MaterialResource.BaseColorFactor[3];
            MatCbv.MetallicRoughnessFactor[0] = (*it)->MaterialResource.MetallicRoughnessFactor[0];
            MatCbv.MetallicRoughnessFactor[1] = (*it)->MaterialResource.MetallicRoughnessFactor[1];
            MatCbv.EmissiveFactor[0] = (*it)->MaterialResource.EmissiveFactor[0];
            MatCbv.EmissiveFactor[1] = (*it)->MaterialResource.EmissiveFactor[1];
            MatCbv.EmissiveFactor[2] = (*it)->MaterialResource.EmissiveFactor[2];
            MatCbv.NormalTextureScale = (*it)->MaterialResource.NormalScaleFactor;
            gfxContext.SetDynamicConstantBufferView(kMaterialConstant, sizeof(MaterialConstants), &MatCbv);
        }

        // Material Shader Resource --  register 0
        {
            gfxContext.SetDescriptorTable(kMaterialSRVs, (*it)->MaterialResource.FirstHandle);
        }

        // Common Constant Buffer --  register 1 / cb1
        {

            CommonConstants CommonCbv;
            memset(&CommonCbv, 0, sizeof(CommonCbv));
            CommonCbv.ModelMatrix = XMLoadFloat4x4((*it)->transform);
            CommonCbv.ViewMatrix = m_Camera->GetViewMatrix();
            CommonCbv.ProjMatrix = m_Camera->GetProjMatrix();
            gfxContext.SetDynamicConstantBufferView(kCommonCBV, sizeof(CommonConstants), &CommonCbv);
        }

        // irradiance -- register 11
        {
            gfxContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_DescriptorHeaps[m_IBLResource->HeapIndex]->GetHeapPointer());
            gfxContext.SetDescriptorTable(kCommonSRVs, m_IBLResource->FirstHandle);
        }

        gfxContext.SetPipelineState((*it)->MaterialResource.PSO);

        gfxContext.SetVertexBuffer(0, (*it)->VertexBuffer.VertexBufferView());

        gfxContext.SetIndexBuffer((*it)->IndexBuffer.IndexBufferView());

        gfxContext.DrawIndexedInstanced((*it)->indexCountPerInstance, 1, 0, 0, 0);
    }

    gfxContext.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_PRESENT, true);

    gfxContext.Finish();
}

void D3dGraphicsCore::CD3dGraphicsCore::RenderCubeMap()
{
    if (!m_IBLResource) {
        return;
    }

    m_IBLResource->SpecularIBLRange = 0.0f;
    auto& specularTex = m_IBLResource->IBLImages[m_IBLResource->CurrentCubemapIndex]->pSpecular;

    if (specularTex) {
        ID3D12Resource* texRes = const_cast<ID3D12Resource*>(specularTex->GetResource());
        const D3D12_RESOURCE_DESC& texDesc = texRes->GetDesc();
        m_IBLResource->SpecularIBLRange = std::max(0.0f, (float)texDesc.MipLevels - 1);
        m_IBLResource->SpecularIBLBias = std::min(m_IBLResource->SpecularIBLBias, m_IBLResource->SpecularIBLRange);
    }

    __declspec(align(16)) struct SkyboxVSCB
    {
        XM_Math::Matrix4 ProjInverse;
        XM_Math::Matrix3 ViewInverse;
    } skyVSCB;
    skyVSCB.ProjInverse = Invert(m_Camera->GetProjMatrix());
    skyVSCB.ViewInverse = Invert(m_Camera->GetViewMatrix()).Get3x3();

    __declspec(align(16)) struct SkyboxPSCB
    {
        float TextureLevel;
    } skyPSCB;
    skyPSCB.TextureLevel = m_IBLResource->SpecularIBLBias;

    //Common SRVs for cubemap -- register 10 
    GraphicsContext& gfxContext = GraphicsContext::Begin(L"CubeMap Render");
    gfxContext.SetRootSignature(g_TemplateRootSignature);
    gfxContext.SetPipelineState(g_SkyBoxPSO);
    gfxContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    gfxContext.TransitionResource(g_DepthBuffer, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
    gfxContext.TransitionResource(g_SceneColorBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, true);
    gfxContext.SetRenderTarget(g_SceneColorBuffer.GetRTV(), g_DepthBuffer.GetDSV_ReadOnly());
    gfxContext.SetViewportAndScissor(m_MainViewport, m_MainScissor);

    gfxContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, g_DescriptorHeaps[m_IBLResource->HeapIndex]->GetHeapPointer());
    gfxContext.SetDynamicConstantBufferView(kMeshConstant, sizeof(SkyboxVSCB), &skyVSCB);
    gfxContext.SetDynamicConstantBufferView(kMaterialConstant, sizeof(SkyboxPSCB), &skyPSCB);
    gfxContext.SetDescriptorTable(kCommonSRVs, m_IBLResource->FirstHandle);
    gfxContext.Draw(3);
    gfxContext.Finish();
}


void D3dGraphicsCore::CD3dGraphicsCore::LoadIBLTextures()
{
    m_IBLResource = std::make_unique<IBLImageResource>();

    std::vector<std::string> IBLFiles;
    std::string IBLLoadDirectory = _IBL_RESOURCE_DIRECTORY;
    std::string specular_suffix = "_specularIBL.dds";
    size_t specular_offset = specular_suffix.size();
    std::string diffuse_suffix = "_diffuseIBL.dds";
    size_t diffuse_offset = diffuse_suffix.size();

    std::unordered_map<std::string, int> ImageName;

    int fileCount = std::count_if(std::filesystem::directory_iterator(IBLLoadDirectory), std::filesystem::directory_iterator{}, (bool (*)(const std::filesystem::path&))std::filesystem::is_regular_file);

    // initialize heap
    m_IBLResource->IBLDescriptorHeap.Create(L"IBLDescriptorHeap", D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, fileCount + 1);

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


    // Load BRDF_LUT Image
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t pitch = 0;
    uint64_t size = 0;
    std::string BRDF_LUT_Name = std::string(_IBL_RESOURCE_DIRECTORY) + "/BRDF/" + "BRDF_LUT.dds";
    m_IBLResource->BRDF_LUT_Image = std::make_unique<Texture>();
    m_IBLResource->BRDF_LUT_Handle = m_IBLResource->IBLDescriptorHeap.Alloc(1);
    HRESULT hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, Utility::UTF8ToWideString(BRDF_LUT_Name).c_str(), size, false,
        m_IBLResource->BRDF_LUT_Image->GetAddressOf(), m_IBLResource->BRDF_LUT_Handle);
    if (FAILED(hr)) {
        ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
    }
    m_IBLResource->BRDF_offset = m_IBLResource->IBLImageCount;

    // dispaly first cube map

    if (m_IBLResource->CurrentCubemapIndex == -1) {
        m_IBLResource->CurrentCubemapIndex = 0;
        m_IBLResource->LastCubemapIndex = 0;
        DescriptorHandle Handle, prevHandle;
        int HeapIdx = -1;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> image_handle_vec;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> brdf_handle_vec;
        prevHandle = m_IBLResource->IBLFirstHandle;
        image_handle_vec.push_back(prevHandle);
        OffsetDescriptorHandle(prevHandle);
        image_handle_vec.push_back(prevHandle);
        Handle = AllocateFromDescriptorHeap(2, HeapIdx);
        m_IBLResource->FirstHandle = Handle;
        m_IBLResource->HeapIndex = HeapIdx;
        CopyDescriptors(Handle, image_handle_vec, 2);
        Handle = AllocateFromDescriptorHeap(1, HeapIdx);
        brdf_handle_vec.push_back(m_IBLResource->BRDF_LUT_Handle);
        CopyDescriptors(Handle, brdf_handle_vec, 1);
    }
}

void D3dGraphicsCore::CD3dGraphicsCore::LoadIBLDDSImage(std::string& ImagePath, std::string& suffix, std::unordered_map<std::string, int>& ImageName)
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

    std::unique_ptr<Texture> pSpecularTex, pDiffuseTex;

    pSpecularTex = std::make_unique<Texture>();
    pDiffuseTex = std::make_unique<Texture>();

    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t pitch = 0;
    uint64_t size = 0;
    int HeapIndex;
    std::string specularImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + specular_suffix;
    std::string diffuseImage = std::string(_IBL_RESOURCE_DIRECTORY) + '/' + imageName + diffuse_suffix;

    DescriptorHandle HeapHandle = m_IBLResource->IBLDescriptorHeap.Alloc(1);
    if (m_IBLResource->IBLFirstHandle.IsNull()) {
        m_IBLResource->IBLFirstHandle = HeapHandle;
    }
    HRESULT hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, Utility::UTF8ToWideString(specularImage).c_str(), size, false,
        pSpecularTex->GetAddressOf(), HeapHandle);
    if (FAILED(hr)) {
        ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
    }
    //if (m_IBLResource->HeapIndex == -1) {
    //	m_IBLResource->HeapIndex = HeapIndex;
    //}
    //ASSERT(HeapIndex == m_IBLResource->HeapIndex, "TEXTURE RESOURCE NOT IN ONE DESCRIPTOR HEAP! ERROR!");

    HeapHandle = m_IBLResource->IBLDescriptorHeap.Alloc(1);
    hr = CreateDDSTextureFromFile(D3dGraphicsCore::g_Device, Utility::UTF8ToWideString(diffuseImage).c_str(), size, false,
        pDiffuseTex->GetAddressOf(), HeapHandle);
    if (FAILED(hr)) {
        ASSERT(false, "CREATE DDS FROM FILE FAILED! ERROR!");
    }
    //ASSERT(HeapIndex == m_IBLResource->HeapIndex, "TEXTURE RESOURCE NOT IN ONE DESCRIPTOR HEAP! ERROR!");

    std::unique_ptr<IBLImageMap> map = std::make_unique<IBLImageMap>();
    map->name = imageName;
    map->pSpecular = std::move(pSpecularTex);
    map->pDiffuse = std::move(pDiffuseTex);
    m_IBLResource->IBLImages.emplace(m_IBLResource->IBLImages.size(), std::move(map));

    ImageName.emplace(imageName, m_IBLResource->IBLImages.size());
}