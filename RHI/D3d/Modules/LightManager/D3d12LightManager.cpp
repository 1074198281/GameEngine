#include "D3d12LightManager.h"
#include "D3d/Core/D3dGraphicsCoreManager.h"
#include "GraphicsStructure.h"
#include "D3d/Core/Pipeline/DescriptorHeap.h"

My::D3d12LightManager::D3d12LightManager()
{

}

My::D3d12LightManager::D3d12LightManager(BaseApplication* pApp)
{
    LightManager();
    m_pApp = pApp;
}

My::D3d12LightManager::~D3d12LightManager()
{
    Clear();
}

void My::D3d12LightManager::Create()
{
    Clear();
    LightManager::Create();
    InitHandle();
}

void My::D3d12LightManager::Clear()
{
    for (auto& it : m_CubeShadowMapTexture) {
        it.second.pColorBuffer->Destroy();
        it.second.pDepthBuffer->Destroy();
    }
    for (auto& it : m_ShadowMapTexture) {
        it.second.pColorBuffer->Destroy();
        it.second.pDepthBuffer->Destroy();
    }
    for (auto& it : m_GlobalShadowMapTexture) {
        it.second.pColorBuffer->Destroy();
        it.second.pDepthBuffer->Destroy();
    }

    Reset();
    m_iHeapIdx = -1;
}

void My::D3d12LightManager::InitHandle()
{
    m_FirstHandle = D3dGraphicsCore::AllocateFromDescriptorHeap(3, m_iHeapIdx);
}

uint64_t My::D3d12LightManager::GetGpuHandle()
{
    return m_FirstHandle.GetGpuPtr();
}

std::shared_ptr<D3dGraphicsCore::DepthBuffer> My::D3d12LightManager::GetDepthBuffer(uint8_t idx)
{
    auto l = m_pLightInfo->Lights[idx];
    switch (l.Type)
    {
    case LightType::Omni:
    {
        if (m_CubeShadowMapTexture.find(l.LightShadowMapIndex) != m_CubeShadowMapTexture.end()) {
            return m_CubeShadowMapTexture[l.LightShadowMapIndex].pDepthBuffer;
        }
    }
    break;
    case LightType::Area:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].pDepthBuffer;
        }
    }
    break;
    case LightType::Spot:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].pDepthBuffer;
        }
    }
    break;
    case LightType::Infinity:
    {
        if (m_GlobalShadowMapTexture.find(l.LightShadowMapIndex) != m_GlobalShadowMapTexture.end()) {
            return m_GlobalShadowMapTexture[l.LightShadowMapIndex].pDepthBuffer;
        }
    }
    break;
    default:
        std::cout << "[D3d12 Light Manager] Undefined Light Type." << std::endl;
        break;
    }

    // haven't create depth buffer for current light
    return CreateDepthBuffer(idx);
}

std::shared_ptr<D3dGraphicsCore::ColorBuffer> My::D3d12LightManager::GetColorBuffer(uint8_t idx)
{
    auto l = m_pLightInfo->Lights[idx];
    switch (l.Type)
    {
    case LightType::Omni:
    {
        if (m_CubeShadowMapTexture.find(l.LightShadowMapIndex) != m_CubeShadowMapTexture.end()) {
            return m_CubeShadowMapTexture[l.LightShadowMapIndex].pColorBuffer;
        }
    }
    break;
    case LightType::Area:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].pColorBuffer;
        }
    }
    break;
    case LightType::Spot:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].pColorBuffer;
        }
    }
    break;
    case LightType::Infinity:
    {
        if (m_GlobalShadowMapTexture.find(l.LightShadowMapIndex) != m_GlobalShadowMapTexture.end()) {
            return m_GlobalShadowMapTexture[l.LightShadowMapIndex].pColorBuffer;
        }
    }
    break;
    default:
        std::cout << "[D3d12 Light Manager] Undefined Light Type." << std::endl;
        break;
    }

    return nullptr;
}

D3dGraphicsCore::DescriptorHandle My::D3d12LightManager::GetDepthSRVDescriptorHandle(uint8_t idx)
{
    auto l = m_pLightInfo->Lights[idx];
    switch (l.Type)
    {
    case LightType::Omni:
    {
        if (m_CubeShadowMapTexture.find(l.LightShadowMapIndex) != m_CubeShadowMapTexture.end()) {
            return m_CubeShadowMapTexture[l.LightShadowMapIndex].depthBufferSRVHandle;
        }
    }
    break;
    case LightType::Area:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].depthBufferSRVHandle;
        }
    }
    break;
    case LightType::Spot:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].depthBufferSRVHandle;
        }
    }
    break;
    case LightType::Infinity:
    {
        if (m_GlobalShadowMapTexture.find(l.LightShadowMapIndex) != m_GlobalShadowMapTexture.end()) {
            return m_GlobalShadowMapTexture[l.LightShadowMapIndex].depthBufferSRVHandle;
        }
    }
    break;
    default:
        std::cout << "[D3d12 Light Manager] Undefined Light Type." << std::endl;
        break;
    }

    return D3dGraphicsCore::DescriptorHandle();
}

size_t My::D3d12LightManager::GetShadowMapHandle(uint8_t idx)
{
    auto l = m_pLightInfo->Lights[idx];
    switch (l.Type)
    {
    case LightType::Omni:
    {
        if (m_CubeShadowMapTexture.find(l.LightShadowMapIndex) != m_CubeShadowMapTexture.end()) {
            return m_CubeShadowMapTexture[l.LightShadowMapIndex].colorBufferSRVHandle.GetGpuPtr();
        }
    }
    break;
    case LightType::Area:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].colorBufferSRVHandle.GetGpuPtr();
        }
    }
    break;
    case LightType::Spot:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].colorBufferSRVHandle.GetGpuPtr();
        }
    }
    break;
    case LightType::Infinity:
    {
        if (m_GlobalShadowMapTexture.find(l.LightShadowMapIndex) != m_GlobalShadowMapTexture.end()) {
            return m_GlobalShadowMapTexture[l.LightShadowMapIndex].colorBufferSRVHandle.GetGpuPtr();
        }
    }
    break;
    default:
        std::cout << "[D3d12 Light Manager] Undefined Light Type." << std::endl;
        break;
    }

    return 0;
}

std::shared_ptr<D3dGraphicsCore::DepthBuffer> My::D3d12LightManager::CreateDepthBuffer(uint8_t idx)
{
	auto& l = m_pLightInfo->Lights[idx];

	std::shared_ptr<D3dGraphicsCore::DepthBuffer> pDepthBuffer = std::make_shared<D3dGraphicsCore::DepthBuffer>(1.0);
	std::wstring depthBufferName = L"ShadowMap_" + std::to_wstring(l.LightShadowMapIndex);
	pDepthBuffer->Create(depthBufferName, D3dGraphicsCore::g_DisplayWidth, D3dGraphicsCore::g_DisplayHeight, DSV_FORMAT);

    std::shared_ptr<D3dGraphicsCore::ColorBuffer> pColorBuffer = std::make_shared<D3dGraphicsCore::ColorBuffer>();
    std::wstring colorBufferName = L"ShadowColor_" + std::to_wstring(l.LightShadowMapIndex);
    pColorBuffer->Create(colorBufferName, D3dGraphicsCore::g_DisplayWidth, D3dGraphicsCore::g_DisplayHeight, 1, D3dGraphicsCore::g_SceneColorBufferFormat);

    D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle[] = {
        pColorBuffer->GetSRV(),
        pDepthBuffer->GetDepthSRV()
    };
    const uint32_t NumSrc = _countof(CpuHandle);
    uint32_t pArray[NumSrc];
    for (int i = 0; i < NumSrc; i++) {
        pArray[i] = 1;
    }
    uint32_t NumDest = NumSrc;
    D3dGraphicsCore::DescriptorHandle GpuHandle = m_FirstHandle;
    D3dGraphicsCore::g_Device->CopyDescriptors(1, &GpuHandle, &NumDest, NumSrc, CpuHandle, pArray, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    D3dGraphicsCore::DescriptorHandle colorbufferSRV = GpuHandle;
    D3dGraphicsCore::OffsetDescriptorHandle(GpuHandle);
    D3dGraphicsCore::DescriptorHandle depthbufferSRV = GpuHandle;

    DepthResource depthRe{ pDepthBuffer, pColorBuffer, colorbufferSRV, depthbufferSRV, m_iHeapIdx };
    switch (l.Type)
    {
    case LightType::Omni:
    {
        m_CubeShadowMapTexture.emplace(l.LightShadowMapIndex, depthRe);
        ASSERT(l.LightShadowMapIndex == m_CubeShadowMapTexture.size() - 1);
    }
    break;
    case LightType::Area:
    {
        m_ShadowMapTexture.emplace(l.LightShadowMapIndex, depthRe);
        ASSERT(l.LightShadowMapIndex == m_ShadowMapTexture.size() - 1);
    }
    break;
    case LightType::Spot:
    {
        m_ShadowMapTexture.emplace(l.LightShadowMapIndex, depthRe);
        ASSERT(l.LightShadowMapIndex == m_ShadowMapTexture.size() - 1);
    }
    break;
    case LightType::Infinity:
    {
        m_GlobalShadowMapTexture.emplace(l.LightShadowMapIndex, depthRe);
        ASSERT(l.LightShadowMapIndex == m_GlobalShadowMapTexture.size() - 1);
    }
    break;
    default:
        ASSERT(false, "Error Light Type");
        std::cout << "[D3d12 Light Manager] Error Light Type!" << std::endl;
        break;
    }

    return pDepthBuffer;
}