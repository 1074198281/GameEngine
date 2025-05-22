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
        it.second.pVolumnBuffer->Destroy();
    }
    for (auto& it : m_ShadowMapTexture) {
        it.second.pColorBuffer->Destroy();
        it.second.pDepthBuffer->Destroy();
        it.second.pVolumnBuffer->Destroy();
    }
    for (auto& it : m_GlobalShadowMapTexture) {
        it.second.pColorBuffer->Destroy();
        it.second.pDepthBuffer->Destroy();
        it.second.pVolumnBuffer->Destroy();
    }

    Reset();
    m_iHeapIdx = -1;

    m_ColorBufferCurrHandle = m_ColorBufferFirstHandle;
    m_DepthBufferCurrHandle = m_DepthBufferFirstHandle;
    m_VolumnBufferCurrHandle = m_VolumnBufferFirstHandle;
    m_DescriptorOffset = 0;
}

void My::D3d12LightManager::InitHandle()
{
    m_ColorBufferFirstHandle = D3dGraphicsCore::AllocateFromDescriptorHeap(MAX_LIGHT_NUM, m_iHeapIdx);
    m_DepthBufferFirstHandle = D3dGraphicsCore::AllocateFromDescriptorHeap(MAX_LIGHT_NUM, m_iHeapIdx);
    m_VolumnBufferFirstHandle = D3dGraphicsCore::AllocateFromDescriptorHeap(MAX_LIGHT_NUM, m_iHeapIdx);
    m_ColorBufferCurrHandle = m_ColorBufferFirstHandle;
    m_DepthBufferCurrHandle = m_DepthBufferFirstHandle;
    m_VolumnBufferCurrHandle = m_VolumnBufferFirstHandle;
    m_DescriptorOffset = 0;
}

uint64_t My::D3d12LightManager::GetColorGpuHandle()
{
    return m_ColorBufferFirstHandle.GetGpuPtr();
}

uint64_t My::D3d12LightManager::GetDepthGpuHandle()
{
    return m_DepthBufferFirstHandle.GetGpuPtr();
}

uint64_t My::D3d12LightManager::GetVolumnGpuHandle()
{
    return m_VolumnBufferFirstHandle.GetGpuPtr();
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

std::shared_ptr<D3dGraphicsCore::ColorBuffer> My::D3d12LightManager::GetVolumnBuffer(uint8_t idx)
{
    auto l = m_pLightInfo->Lights[idx];
    switch (l.Type)
    {
    case LightType::Omni:
    {
        if (m_CubeShadowMapTexture.find(l.LightShadowMapIndex) != m_CubeShadowMapTexture.end()) {
            return m_CubeShadowMapTexture[l.LightShadowMapIndex].pVolumnBuffer;
        }
    }
    break;
    case LightType::Area:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].pVolumnBuffer;
        }
    }
    break;
    case LightType::Spot:
    {
        if (m_ShadowMapTexture.find(l.LightShadowMapIndex) != m_ShadowMapTexture.end()) {
            return m_ShadowMapTexture[l.LightShadowMapIndex].pVolumnBuffer;
        }
    }
    break;
    case LightType::Infinity:
    {
        if (m_GlobalShadowMapTexture.find(l.LightShadowMapIndex) != m_GlobalShadowMapTexture.end()) {
            return m_GlobalShadowMapTexture[l.LightShadowMapIndex].pVolumnBuffer;
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

    std::shared_ptr<D3dGraphicsCore::ColorBuffer> pVolumnBuffer = std::make_shared<D3dGraphicsCore::ColorBuffer>();
    std::wstring volumnBufferName = L"VolumnBuffer_" + std::to_wstring(l.LightShadowMapIndex);
    pVolumnBuffer->Create(volumnBufferName, D3dGraphicsCore::g_DisplayWidth, D3dGraphicsCore::g_DisplayHeight, 1, D3dGraphicsCore::g_SceneColorBufferFormat);

    D3D12_CPU_DESCRIPTOR_HANDLE ColorCpuHandle[] = {
        pColorBuffer->GetSRV(),
    };
    D3D12_CPU_DESCRIPTOR_HANDLE DepthCpuHandle[] = {
        pDepthBuffer->GetDepthSRV(),
    };
    D3D12_CPU_DESCRIPTOR_HANDLE VolumnCpuHandle[] = {
        pVolumnBuffer->GetSRV(),
    };

    D3dGraphicsCore::CopyDescriptors(m_ColorBufferCurrHandle, ColorCpuHandle, 1);
    D3dGraphicsCore::CopyDescriptors(m_DepthBufferCurrHandle, DepthCpuHandle, 1);
    D3dGraphicsCore::CopyDescriptors(m_VolumnBufferCurrHandle, VolumnCpuHandle, 1);
    l.DescriptorOffset = m_DescriptorOffset;
    DepthResource depthRe{ pDepthBuffer, pColorBuffer, pVolumnBuffer, m_ColorBufferCurrHandle, m_DepthBufferCurrHandle, m_VolumnBufferCurrHandle, m_iHeapIdx };

    D3dGraphicsCore::OffsetDescriptorHandle(m_ColorBufferCurrHandle);
    D3dGraphicsCore::OffsetDescriptorHandle(m_DepthBufferCurrHandle);
    D3dGraphicsCore::OffsetDescriptorHandle(m_VolumnBufferCurrHandle);
    m_DescriptorOffset++;
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