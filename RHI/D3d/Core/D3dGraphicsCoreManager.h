#pragma once

#include "D3dCommonDef.h"

#include "Command/CommandListManager.h"
#include "Command/CommandContext.h"
#include "Pipeline/DescriptorHeap.h"
#include "Resource/ColorBuffer.h"
#include "Resource/DepthBuffer.h"

#define DSV_FORMAT DXGI_FORMAT_D32_FLOAT

namespace D3dGraphicsCore {

	/*-------------------------Graphics Column--------------------------*/
#ifndef RELEASE
	extern const GUID WKPDID_D3DDebugObjectName;
#endif // !RELEASE
	using namespace Microsoft::WRL;
	
	extern HWND g_hWnd;

	void Initialize(bool RequireDXRSupport = false);
	void Shutdown(void);

	bool IsDeviceNvidia(ID3D12Device* pDevice);
	bool IsDeviceAMD(ID3D12Device* pDevice);
	bool IsDeviceIntel(ID3D12Device* pDevice);

	extern ID3D12Device* g_Device;
	extern CommandListManager g_CommandManager;
	extern ContextManager g_ContextManager;

	extern D3D_FEATURE_LEVEL g_D3DFeatureLevel;
	extern bool g_bTypedUAVLoadSupport_R11G11B10_FLOAT;
	extern bool g_bTypedUAVLoadSupport_R16G16B16A16_FLOAT;


	extern DescriptorAllocator g_DescriptorAllocator[];
	inline D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE Type, UINT Count = 1)
	{
		return g_DescriptorAllocator[Type].Allocate(Count);
	}


	/*-------------------------Display column--------------------------*/
	extern uint32_t g_DisplayWidth;
	extern uint32_t g_DisplayHeight;
	extern bool g_bEnableHDROutput;

	// Returns the number of elapsed frames since application start
	uint64_t GetFrameCount(void);

	// The amount of time elapsed during the last completed frame.  The CPU and/or
	// GPU may be idle during parts of the frame.  The frame time measures the time
	// between calls to present each frame.
	float GetFrameTime(void);

	// The total number of frames per second
	float GetFrameRate(void);



	/*-------------------------
	Pre columns in Display 
	move them to GraphicsCore all
	--------------------------*/
	extern float s_FrameTime;
	extern uint64_t s_FrameIndex;
	extern int64_t s_FrameStartTick;

	extern DepthBuffer g_SceneDepthBuffer;	// D32_FLOAT_S8_UINT
	extern ColorBuffer g_SceneColorBuffer;	// R11G11B10_FLOAT
	extern DXGI_FORMAT g_SceneDepthBufferFormat;
	extern DXGI_FORMAT g_SceneColorBufferFormat;
	void InitializeBuffers();

	extern DXGI_FORMAT g_SwapChainFormat;
	extern ColorBuffer g_PreDisplayBuffer;
	extern ColorBuffer g_DisplayBuffer[SWAP_CHAIN_BUFFER_COUNT];
	extern DepthBuffer g_DepthBuffer;
	extern DescriptorHandle g_DepthBufferSRVHandle;
	extern int g_iDepthBufferHeapIdx;
	extern UINT g_CurrentBuffer;
	extern IDXGISwapChain1* s_SwapChain1;

	void InitializeDisplay(void);
	void ShutdownDisplay();
	void DisplayResize(uint32_t width, uint32_t height);
	void Present(void);
}

