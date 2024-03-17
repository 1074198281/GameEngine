#pragma once
#include "IMemoryManager.hpp"
#include "Allocator.hpp"
#include <new>

namespace My {
    class MemoryManager : __implements IMemoryManager
    {
    public:
        template<typename T, typename... Arguments>
        T* New(Arguments... parameters)
        {
            return new (Allocate(sizeof(T))) T(parameters...);
        }

        template<typename T>
        void Delete(T *p)
        {
            reinterpret_cast<T*>(p)->~T();
            Free(p, sizeof(T));
        }

    public:
        ~MemoryManager() override = default;

        virtual int Initialize();
        virtual void Finalize();
        virtual void Tick();

        void* Allocate(size_t size);
        void* Allocate(size_t size, size_t alignment);
        void  Free(void* p, size_t size);
    private:
        static size_t*        m_pBlockSizeLookup;
        static Allocator*     m_pAllocators;
    private:
        static Allocator* LookUpAllocator(size_t size);
    };
}