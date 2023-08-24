//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard 
//


#include "DescriptorHeap.h"
#include "GraphicsCore.h"
#include "D3dGraphicsCoreManager.h"
#include "Command/CommandListManager.h"


//
// DescriptorAllocator implementation
//
std::mutex D3dGraphicsCore::DescriptorAllocator::sm_AllocationMutex;
std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> D3dGraphicsCore::DescriptorAllocator::sm_DescriptorHeapPool;

void D3dGraphicsCore::DescriptorAllocator::DestroyAll(void)
{
    sm_DescriptorHeapPool.clear();
}

ID3D12DescriptorHeap* D3dGraphicsCore::DescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE Type)
{
    std::lock_guard<std::mutex> LockGuard(sm_AllocationMutex);

    D3D12_DESCRIPTOR_HEAP_DESC Desc;
    Desc.Type = Type;
    Desc.NumDescriptors = sm_NumDescriptorsPerHeap;
    Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    Desc.NodeMask = 1;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pHeap;
    ASSERT_SUCCEEDED(D3dGraphicsCore::g_Device->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pHeap)));
    sm_DescriptorHeapPool.emplace_back(pHeap);
    return pHeap.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3dGraphicsCore::DescriptorAllocator::Allocate(uint32_t Count)
{
    if (m_CurrentHeap == nullptr || m_RemainingFreeHandles < Count)
    {
        m_CurrentHeap = RequestNewHeap(m_Type);
        m_CurrentHandle = m_CurrentHeap->GetCPUDescriptorHandleForHeapStart();
        m_RemainingFreeHandles = sm_NumDescriptorsPerHeap;

        if (m_DescriptorSize == 0)
            m_DescriptorSize = D3dGraphicsCore::g_Device->GetDescriptorHandleIncrementSize(m_Type);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE ret = m_CurrentHandle;
    m_CurrentHandle.ptr += Count * m_DescriptorSize;
    m_RemainingFreeHandles -= Count;
    return ret;
}

//
// DescriptorHeap implementation
//

void D3dGraphicsCore::DescriptorHeap::Create(const std::wstring& Name, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32_t MaxCount)
{
    m_HeapDesc.Type = Type;
    m_HeapDesc.NumDescriptors = MaxCount;
    m_HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    m_HeapDesc.NodeMask = 1;

    ASSERT_SUCCEEDED(D3dGraphicsCore::g_Device->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(m_Heap.ReleaseAndGetAddressOf())));

#ifdef RELEASE
    (void)Name;
#else
    m_Heap->SetName(Name.c_str());
#endif

    m_DescriptorSize = g_Device->GetDescriptorHandleIncrementSize(m_HeapDesc.Type);
    m_NumFreeDescriptors = m_HeapDesc.NumDescriptors;
    m_FirstHandle = DescriptorHandle(
        m_Heap->GetCPUDescriptorHandleForHeapStart(),
        m_Heap->GetGPUDescriptorHandleForHeapStart());
    m_NextFreeHandle = m_FirstHandle;
}

D3dGraphicsCore::DescriptorHandle D3dGraphicsCore::DescriptorHeap::Alloc(uint32_t Count)
{
    ASSERT(HasAvailableSpace(Count), "Descriptor Heap out of space.  Increase heap size.");
    DescriptorHandle ret = m_NextFreeHandle;
    m_NextFreeHandle += Count * m_DescriptorSize;
    m_NumFreeDescriptors -= Count;
    return ret;
}

bool D3dGraphicsCore::DescriptorHeap::ValidateHandle(const D3dGraphicsCore::DescriptorHandle& DHandle) const
{
    if (DHandle.GetCpuPtr() < m_FirstHandle.GetCpuPtr() ||
        DHandle.GetCpuPtr() >= m_FirstHandle.GetCpuPtr() + m_HeapDesc.NumDescriptors * m_DescriptorSize)
        return false;

    if (DHandle.GetGpuPtr() - m_FirstHandle.GetGpuPtr() !=
        DHandle.GetCpuPtr() - m_FirstHandle.GetCpuPtr())
        return false;

    return true;
}
