#pragma once

#include "GpuBuffer.h"

namespace D3dGraphicsCore {

    class ReadbackBuffer : public GpuBuffer
    {
    public:
        virtual ~ReadbackBuffer() { Destroy(); }

        void Create(const std::wstring& name, uint32_t NumElements, uint32_t ElementSize);

        void* Map(void);
        void Unmap(void);

    protected:

        void CreateDerivedViews(void) {}

    };

}