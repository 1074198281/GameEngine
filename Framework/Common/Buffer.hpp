#pragma once
#include <memory.h>
#include <stddef.h>
#include "MemoryManager.hpp"

namespace My {
    class Buffer {
    public:
        Buffer() : m_pData(nullptr), m_szSize(0), m_szAlignment(alignof(uint32_t)) {}

        Buffer(size_t size, size_t alignment = 4) : m_szSize(size), m_szAlignment(alignment) { m_pData = reinterpret_cast<uint8_t*>(new uint8_t[size]); }

        Buffer(const Buffer& rhs) = delete;

        Buffer(Buffer&& rhs) {
            m_pData = rhs.m_pData;
            m_szSize = rhs.m_szSize;
            m_szAlignment = rhs.m_szAlignment;
            rhs.m_pData = nullptr;
            rhs.m_szSize = 0;
            rhs.m_szAlignment = 4;
        }

        Buffer& operator = (const Buffer& rhs) = delete;

        Buffer& operator = (Buffer&& rhs) { 
            delete[] m_pData;
            m_pData = rhs.m_pData;
            m_szSize = rhs.m_szSize;
            rhs.m_pData = nullptr;
            rhs.m_szSize = 0;
            return *this;
        }

        ~Buffer() {
            if (m_pData != nullptr) {
                delete[] m_pData;
            }
        }

        uint8_t* MoveData() {
            uint8_t* tmp = m_pData;
            m_pData = nullptr;
            m_szSize = 0;
            return tmp;
        }

        void SetData(uint8_t* data, size_t size) {
            if (m_pData != nullptr) {
                delete[] m_pData;
            }
            m_pData = data;
            m_szSize = size;
        }


        uint8_t* GetData(void) { return m_pData; };
        const uint8_t* GetData(void) const { return m_pData; };
        size_t GetDataSize(void) const { return m_szSize; };
    public:
        uint8_t* m_pData;
        size_t m_szSize;
        size_t m_szAlignment;
    };
}
