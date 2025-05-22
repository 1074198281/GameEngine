#include "D3dGraphicsCoreManager.h"
#include "Command/CommandLineArg.h"
#include "Common/SystemTime.h"
#include "GraphicsStructure.h"
#include <algorithm>
#include <cwctype>
#include <iostream>

#ifdef _GAMING_DESKTOP
#include <winreg.h>		// To read the registry
#endif

#define DSV_FORMAT DXGI_FORMAT_D32_FLOAT

namespace D3dGraphicsCore
{
    HWND g_hWnd = nullptr;

	ID3D12Device* g_Device = nullptr;
	CommandListManager g_CommandManager;
	ContextManager g_ContextManager;

	D3D_FEATURE_LEVEL g_D3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;
    bool g_bTypedUAVLoadSupport_R11G11B10_FLOAT = false;
    bool g_bTypedUAVLoadSupport_R16G16B16A16_FLOAT = false;

    DescriptorAllocator g_DescriptorAllocator[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES] =
    {
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV
    };

    static const uint32_t vendorID_Nvidia = 0x10DE;
    static const uint32_t vendorID_AMD = 0x1002;
    static const uint32_t vendorID_Intel = 0x8086;

    bool g_bEnableHDROutput = false;

    uint32_t g_DisplayWidth;
    uint32_t g_DisplayHeight;

    float s_FrameTime = 0.0f;
    uint64_t s_FrameIndex = 0;
    int64_t s_FrameStartTick = 0;
    ColorBuffer g_SceneColorBuffer;	// R11G11B10_FLOAT
    DXGI_FORMAT g_SceneColorBufferFormat = DXGI_FORMAT_R11G11B10_FLOAT;

    DXGI_FORMAT g_SwapChainFormat = DXGI_FORMAT_R10G10B10A2_UNORM;
    ColorBuffer g_PreDisplayBuffer;
    ColorBuffer g_DisplayBuffer[SWAP_CHAIN_BUFFER_COUNT];
    DepthBuffer g_DepthBuffer(1.0f);
    DescriptorHandle g_DepthBufferSRVHandle;
    int g_iDepthBufferHeapIdx = -1;
    DXGI_FORMAT g_SceneDepthBufferFormat(DSV_FORMAT);
    UINT g_CurrentBuffer = 0;
    IDXGISwapChain1* s_SwapChain1 = nullptr;
}

void D3dGraphicsCore::InitializeBuffers()
{
    g_SceneColorBuffer.Destroy();
    g_SceneColorBuffer.Create(L"Scene Buffer", g_DisplayWidth, g_DisplayHeight, 1, g_SceneColorBufferFormat);
    g_DepthBuffer.Destroy();
    g_DepthBuffer.Create(L"DepthBuffer", g_DisplayWidth, g_DisplayHeight, DSV_FORMAT);

    // set depth srv
    if (g_DepthBufferSRVHandle.IsNull()) {
        g_DepthBufferSRVHandle = AllocateFromDescriptorHeap(1, g_iDepthBufferHeapIdx);
    }
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpuHandle;
    cpuHandle.push_back(g_DepthBuffer.GetDepthSRV());
    CopyDescriptors(g_DepthBufferSRVHandle, cpuHandle, 1);
}

uint32_t GetDesiredGPUVendor()
{
    uint32_t desiredVendor = 0;

    std::wstring vendorVal;
    if (CommandLineArgs::GetString(L"vendor", vendorVal))
    {
        // Convert to lower case
        std::transform(vendorVal.begin(), vendorVal.end(), vendorVal.begin(), std::towlower);

        if (vendorVal.find(L"amd") != std::wstring::npos)
        {
            desiredVendor = D3dGraphicsCore::vendorID_AMD;
        }
        else if (vendorVal.find(L"nvidia") != std::wstring::npos || vendorVal.find(L"nvd") != std::wstring::npos ||
            vendorVal.find(L"nvda") != std::wstring::npos || vendorVal.find(L"nv") != std::wstring::npos)
        {
            desiredVendor = D3dGraphicsCore::vendorID_Nvidia;
        }
        else if (vendorVal.find(L"intel") != std::wstring::npos || vendorVal.find(L"intc") != std::wstring::npos)
        {
            desiredVendor = D3dGraphicsCore::vendorID_Intel;
        }
    }

    return desiredVendor;
}

const wchar_t* GPUVendorToString(uint32_t vendorID)
{
    switch (vendorID)
    {
    case D3dGraphicsCore::vendorID_Nvidia:
        return L"Nvidia";
    case D3dGraphicsCore::vendorID_AMD:
        return L"AMD";
    case D3dGraphicsCore::vendorID_Intel:
        return L"Intel";
    default:
        return L"Unknown";
        break;
    }
}

uint32_t GetVendorIdFromDevice(ID3D12Device* pDevice)
{
    LUID luid = pDevice->GetAdapterLuid();

    // Obtain the DXGI factory
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    ASSERT_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

    Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;

    if (SUCCEEDED(dxgiFactory->EnumAdapterByLuid(luid, IID_PPV_ARGS(&pAdapter))))
    {
        DXGI_ADAPTER_DESC1 desc;
        if (SUCCEEDED(pAdapter->GetDesc1(&desc)))
        {
            return desc.VendorId;
        }
    }

    return 0;
}

// Check adapter support for DirectX Raytracing.
bool IsDirectXRaytracingSupported(ID3D12Device* testDevice)
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupport = {};

    if (FAILED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupport, sizeof(featureSupport))))
        return false;

    return featureSupport.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
}

bool D3dGraphicsCore::IsDeviceNvidia(ID3D12Device* pDevice)
{
    return GetVendorIdFromDevice(pDevice) == vendorID_Nvidia;
}

bool D3dGraphicsCore::IsDeviceAMD(ID3D12Device* pDevice)
{
    return GetVendorIdFromDevice(pDevice) == vendorID_AMD;
}

bool D3dGraphicsCore::IsDeviceIntel(ID3D12Device* pDevice)
{
    return GetVendorIdFromDevice(pDevice) == vendorID_Intel;
}

void D3dGraphicsCore::Initialize(bool RequireDXRSupport)
{
    Microsoft::WRL::ComPtr<ID3D12Device> pDevice;

    uint32_t useDebugLayers = 0;
    CommandLineArgs::GetInteger(L"debug", useDebugLayers);
#if _DEBUG
    // Default to true for debug builds
    useDebugLayers = 1;
#endif

    DWORD dxgiFactoryFlags = 0;

    if (useDebugLayers)
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
        {
            debugInterface->EnableDebugLayer();

            uint32_t useGPUBasedValidation = 0;
            CommandLineArgs::GetInteger(L"gpu_debug", useGPUBasedValidation);
            if (useGPUBasedValidation)
            {
                Microsoft::WRL::ComPtr<ID3D12Debug1> debugInterface1;
                if (SUCCEEDED((debugInterface->QueryInterface(IID_PPV_ARGS(&debugInterface1)))))
                {
                    debugInterface1->SetEnableGPUBasedValidation(true);
                }
            }
        }
        else
        {
            My::Print("WARNING:  Unable to enable D3D12 debug validation layer\n");
        }

#if _DEBUG
        ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
        {
            dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

            DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
            {
                80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
            };
            DXGI_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
        }
#endif
    }

    // Obtain the DXGI factory
    Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
    ASSERT_SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    // Create the D3D graphics device
    Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter;

    uint32_t bUseWarpDriver = false;
    CommandLineArgs::GetInteger(L"warp", bUseWarpDriver);

    uint32_t desiredVendor = GetDesiredGPUVendor();

    if (desiredVendor)
    {
        My::Printf(L"Looking for a %s GPU\n", GPUVendorToString(desiredVendor));
    }

    // Temporary workaround because SetStablePowerState() is crashing
    D3D12EnableExperimentalFeatures(0, nullptr, nullptr, nullptr);

    if (!bUseWarpDriver)
    {
        SIZE_T MaxSize = 0;

        for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != dxgiFactory->EnumAdapters1(Idx, &pAdapter); ++Idx)
        {
            DXGI_ADAPTER_DESC1 desc;
            pAdapter->GetDesc1(&desc);

            // Is a software adapter?
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            // Is this the desired vendor desired?
            if (desiredVendor != 0 && desiredVendor != desc.VendorId)
                continue;

            // Can create a D3D12 device?
            if (FAILED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
                continue;

            // Does support DXR if required?
            if (RequireDXRSupport && !IsDirectXRaytracingSupported(pDevice.Get()))
                continue;

            // By default, search for the adapter with the most memory because that's usually the dGPU.
            if (desc.DedicatedVideoMemory < MaxSize)
                continue;

            MaxSize = desc.DedicatedVideoMemory;

            if (g_Device != nullptr)
                g_Device->Release();

            g_Device = pDevice.Detach();

            My::Printf(L"Selected GPU:  %s (%u MB)\n", desc.Description, desc.DedicatedVideoMemory >> 20);
        }
    }

    if (RequireDXRSupport && !g_Device)
    {
        My::Printf("Unable to find a DXR-capable device. Halting.\n");
        __debugbreak();
    }

    if (g_Device == nullptr)
    {
        if (bUseWarpDriver)
            My::Print("WARP software adapter requested.  Initializing...\n");
        else
            My::Print("Failed to find a hardware adapter.  Falling back to WARP.\n");
        ASSERT_SUCCEEDED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pAdapter)));
        ASSERT_SUCCEEDED(D3D12CreateDevice(pAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice)));
        g_Device = pDevice.Detach();
    }
#ifndef RELEASE
    else
    {
        bool DeveloperModeEnabled = false;

        // Look in the Windows Registry to determine if Developer Mode is enabled
        HKEY hKey;
        LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCSTR)L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\AppModelUnlock", 0, KEY_READ, &hKey);
        if (result == ERROR_SUCCESS)
        {
            DWORD keyValue, keySize = sizeof(DWORD);
            result = RegQueryValueEx(hKey, (LPCSTR)L"AllowDevelopmentWithoutDevLicense", 0, NULL, (byte*)&keyValue, &keySize);
            if (result == ERROR_SUCCESS && keyValue == 1)
                DeveloperModeEnabled = true;
            RegCloseKey(hKey);
        }

        WARN_ONCE_IF_NOT(DeveloperModeEnabled, "Enable Developer Mode on Windows 10 to get consistent profiling results");

        // Prevent the GPU from overclocking or underclocking to get consistent timings
        if (DeveloperModeEnabled)
            g_Device->SetStablePowerState(TRUE);
    }
#endif	

#if _DEBUG
    ID3D12InfoQueue* pInfoQueue = nullptr;
    if (SUCCEEDED(g_Device->QueryInterface(IID_PPV_ARGS(&pInfoQueue))))
    {
        // Suppress whole categories of messages
        //D3D12_MESSAGE_CATEGORY Categories[] = {};

        // Suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY Severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        // Suppress individual messages by their ID
        D3D12_MESSAGE_ID DenyIds[] =
        {
            // This occurs when there are uninitialized descriptors in a descriptor table, even when a
            // shader does not access the missing descriptors.  I find this is common when switching
            // shader permutations and not wanting to change much code to reorder resources.
            D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE,

            // Triggered when a shader does not export all color components of a render target, such as
            // when only writing RGB to an R10G10B10A2 buffer, ignoring alpha.
            D3D12_MESSAGE_ID_CREATEGRAPHICSPIPELINESTATE_PS_OUTPUT_RT_OUTPUT_MISMATCH,

            // This occurs when a descriptor table is unbound even when a shader does not access the missing
            // descriptors.  This is common with a root signature shared between disparate shaders that
            // don't all need the same types of resources.
            D3D12_MESSAGE_ID_COMMAND_LIST_DESCRIPTOR_TABLE_NOT_SET,

            // RESOURCE_BARRIER_DUPLICATE_SUBRESOURCE_TRANSITIONS
            (D3D12_MESSAGE_ID)1008,
        };

        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        //NewFilter.DenyList.NumCategories = _countof(Categories);
        //NewFilter.DenyList.pCategoryList = Categories;
        NewFilter.DenyList.NumSeverities = _countof(Severities);
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof(DenyIds);
        NewFilter.DenyList.pIDList = DenyIds;

        pInfoQueue->PushStorageFilter(&NewFilter);
        pInfoQueue->Release();
    }
#endif

    // We like to do read-modify-write operations on UAVs during post processing.  To support that, we
    // need to either have the hardware do typed UAV loads of R11G11B10_FLOAT or we need to manually
    // decode an R32_UINT representation of the same buffer.  This code determines if we get the hardware
    // load support.
    D3D12_FEATURE_DATA_D3D12_OPTIONS FeatureData = {};
    if (SUCCEEDED(g_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &FeatureData, sizeof(FeatureData))))
    {
        if (FeatureData.TypedUAVLoadAdditionalFormats)
        {
            D3D12_FEATURE_DATA_FORMAT_SUPPORT Support =
            {
                DXGI_FORMAT_R11G11B10_FLOAT, D3D12_FORMAT_SUPPORT1_NONE, D3D12_FORMAT_SUPPORT2_NONE
            };

            if (SUCCEEDED(g_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
                (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
            {
                g_bTypedUAVLoadSupport_R11G11B10_FLOAT = true;
            }

            Support.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

            if (SUCCEEDED(g_Device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &Support, sizeof(Support))) &&
                (Support.Support2 & D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) != 0)
            {
                g_bTypedUAVLoadSupport_R16G16B16A16_FLOAT = true;
            }
        }
    }

    g_CommandManager.Create(g_Device);

    //InitializeCommonState();

    //Initialize Display columns, now combine all Display columns into D3dGraphicsCore
    InitializeDisplay();
    InitializeCommonState();
}

void D3dGraphicsCore::Shutdown(void)
{
    g_CommandManager.IdleGPU();

    CommandContext::DestroyAllContexts();
    g_CommandManager.Shutdown();
    //GpuTimeManager::Shutdown();
    PSO::DestroyAll();
    RootSignature::DestroyAll();
    DescriptorAllocator::DestroyAll();

    //DestroyCommonState();

#if defined(_DEBUG)
    ID3D12DebugDevice* debugInterface;
    if (SUCCEEDED(g_Device->QueryInterface(&debugInterface)))
    {
        debugInterface->ReportLiveDeviceObjects( D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
        //__debugbreak();
        debugInterface->Release();
    }
#endif

    if (g_Device != nullptr)
    {
        g_Device->Release();
        g_Device = nullptr;
    }
}

void D3dGraphicsCore::InitializeDisplay(void)
{
    ASSERT(s_SwapChain1 == nullptr, "Graphics has already been initialized");

    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    ASSERT_SUCCEEDED(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = g_DisplayWidth;
    swapChainDesc.Height = g_DisplayHeight;
    swapChainDesc.Format = g_SwapChainFormat;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
    fsSwapChainDesc.Windowed = TRUE;

    ASSERT_SUCCEEDED(dxgiFactory->CreateSwapChainForHwnd(
        g_CommandManager.GetCommandQueue(),
        g_hWnd,
        &swapChainDesc,
        &fsSwapChainDesc,
        nullptr,
        &s_SwapChain1));

#if CONDITIONALLY_ENABLE_HDR_OUTPUT
    {
        IDXGISwapChain4* swapChain = (IDXGISwapChain4*)s_SwapChain1;
        ComPtr<IDXGIOutput> output;
        ComPtr<IDXGIOutput6> output6;
        DXGI_OUTPUT_DESC1 outputDesc;
        UINT colorSpaceSupport;

        // Query support for ST.2084 on the display and set the color space accordingly
        if (SUCCEEDED(swapChain->GetContainingOutput(&output)) && SUCCEEDED(output.As(&output6)) &&
            SUCCEEDED(output6->GetDesc1(&outputDesc)) && outputDesc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 &&
            SUCCEEDED(swapChain->CheckColorSpaceSupport(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020, &colorSpaceSupport)) &&
            (colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT) &&
            SUCCEEDED(swapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)))
        {
            g_bEnableHDROutput = true;
        }
    }
#endif // End CONDITIONALLY_ENABLE_HDR_OUTPUT

    for (uint32_t i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
    {
        ComPtr<ID3D12Resource> DisplayPlane;
        ASSERT_SUCCEEDED(s_SwapChain1->GetBuffer(i, IID_PPV_ARGS(&DisplayPlane)));
        g_DisplayBuffer[i].CreateFromSwapChain(L"Primary SwapChain Buffer", DisplayPlane.Detach());
    }

    g_PreDisplayBuffer.Create(L"PreDisplay Buffer", g_DisplayWidth, g_DisplayHeight, 1, g_SwapChainFormat);

    g_DepthBuffer.Create(L"DepthBuffer", g_DisplayWidth, g_DisplayHeight, DSV_FORMAT);
}

void D3dGraphicsCore::ShutdownDisplay()
{
    s_SwapChain1->SetFullscreenState(FALSE, nullptr);
    s_SwapChain1->Release();

    for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
        g_DisplayBuffer[i].Destroy();

    g_PreDisplayBuffer.Destroy();
    g_SceneColorBuffer.Destroy();
    g_DepthBuffer.Destroy();
}

void D3dGraphicsCore::DisplayResize(uint32_t width, uint32_t height)
{
    g_CommandManager.IdleGPU();

    g_DisplayWidth = width;
    g_DisplayHeight = height;

    DEBUGPRINT("Changing display resolution to %ux%u", width, height);
    g_PreDisplayBuffer.Destroy();
    g_PreDisplayBuffer.Create(L"PreDisplay Buffer", width, height, 1, g_SwapChainFormat);

    for (uint32_t i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
        g_DisplayBuffer[i].Destroy();

    ASSERT(s_SwapChain1 != nullptr);
    ASSERT_SUCCEEDED(s_SwapChain1->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT, width, height, g_SwapChainFormat, 0));

    for (uint32_t i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
    {
        ComPtr<ID3D12Resource> DisplayPlane;
        ASSERT_SUCCEEDED(s_SwapChain1->GetBuffer(i, IID_PPV_ARGS(&DisplayPlane)));
        g_DisplayBuffer[i].CreateFromSwapChain(L"Primary SwapChain Buffer", DisplayPlane.Detach());
    }

    g_CurrentBuffer = 0;

    g_CommandManager.IdleGPU();

    InitializeBuffers();
    //ResizeDisplayDependentBuffers(g_NativeWidth, g_NativeHeight);
}

void D3dGraphicsCore::Present(void)
{
    UINT PresentInterval = 0;
    //UINT PresentInterval = s_EnableVSync ? std::min(4, (int)Round(s_FrameTime * 60.0f)) : 0;

    HRESULT hr= s_SwapChain1->Present(0, 0);
    if (hr != S_OK) {
        hr = g_Device->GetDeviceRemovedReason();
        const char* reason = "";
        switch (hr) {
        case DXGI_ERROR_DEVICE_HUNG: reason = "DEVICE_HUNG"; break;
        case DXGI_ERROR_DEVICE_REMOVED: reason = "DEVICE_REMOVED"; break;
        case DXGI_ERROR_DEVICE_RESET: reason = "DEVICE_RESET"; break;
        case DXGI_ERROR_DRIVER_INTERNAL_ERROR: reason = "DRIVER_INTERNAL_ERROR"; break;
        }
        std::cout << "[D3dGraphicsCore] Device removed! Reason: " << reason << std::endl;
        ASSERT(false);
    }


    g_CurrentBuffer = (g_CurrentBuffer + 1) % SWAP_CHAIN_BUFFER_COUNT;

    UINT swapBackIndex = ((IDXGISwapChain3*)s_SwapChain1)->GetCurrentBackBufferIndex();
    ASSERT(g_CurrentBuffer == swapBackIndex);

    // Test robustness to handle spikes in CPU time
    //if (s_DropRandomFrames)
    //{
    //	if (std::rand() % 25 == 0)
    //		BusyLoopSleep(0.010);
    //}

    int64_t CurrentTick = SystemTime::GetCurrentTick();

    //if (s_EnableVSync)
    //{
    //    // With VSync enabled, the time step between frames becomes a multiple of 16.666 ms.  We need
    //    // to add logic to vary between 1 and 2 (or 3 fields).  This delta time also determines how
    //    // long the previous frame should be displayed (i.e. the present interval.)
    //    s_FrameTime = (s_LimitTo30Hz ? 2.0f : 1.0f) / 60.0f;
    //    if (s_DropRandomFrames)
    //    {
    //        if (std::rand() % 50 == 0)
    //            s_FrameTime += (1.0f / 60.0f);
    //    }
    //}
    //else
    //{
    //    // When running free, keep the most recent total frame time as the time step for
    //    // the next frame simulation.  This is not super-accurate, but assuming a frame
    //    // time varies smoothly, it should be close enough.
    //    s_FrameTime = (float)SystemTime::TimeBetweenTicks(s_FrameStartTick, CurrentTick);
    //}
    s_FrameTime = (float)SystemTime::TimeBetweenTicks(s_FrameStartTick, CurrentTick);


    s_FrameStartTick = CurrentTick;

    ++s_FrameIndex;

    //TemporalEffects::Update((uint32_t)s_FrameIndex);

    //SetNativeResolution();
    //SetDisplayResolution();
}


//--------------------------Time----------------------------//
uint64_t D3dGraphicsCore::GetFrameCount(void)
{
    return s_FrameIndex;
}

float D3dGraphicsCore::GetFrameTime(void)
{
    return s_FrameTime;
}

float D3dGraphicsCore::GetFrameRate(void)
{
    return s_FrameTime == 0.0f ? 0.0f : 1.0f / s_FrameTime;
}