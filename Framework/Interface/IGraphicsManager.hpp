#pragma once

#include "IRuntimeModule.hpp"

namespace My {
    __Interface IGraphicsManager : public IRuntimeModule{
    public:
        IGraphicsManager() = default;
        virtual ~IGraphicsManager() = default;

        virtual int Initialize() { return 0; }
        virtual void Finalize() {}

        virtual void Tick() {}

        virtual void Clear() {}
        virtual void Draw() {}
        virtual void Present() {}

        virtual void Resize(uint32_t width, uint32_t height) {} 

        virtual void UpArrowKeyDown() {}
        virtual void DownArrowKeyDown() {}
        virtual void LeftArrowKeyDown() {}
        virtual void RightArrowKeyDown() {}
        virtual void NumPadKeyDown(int64_t key) {}
        virtual void NumPadKeyUp(int64_t key) {}
        virtual void FunctionKeyDown(int64_t key) {}
        virtual void FunctionKeyUp(int64_t key) {}
	};
}