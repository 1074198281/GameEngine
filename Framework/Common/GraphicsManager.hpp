#pragma once
#include "Image.hpp"
#include "IRuntimeModule.hpp"

namespace My {
    class GraphicsManager : __implements IRuntimeModule
    {
    public:
        virtual ~GraphicsManager() {}

        virtual int Initialize();
        virtual void Finalize();

        virtual void Tick();

        virtual void Clear();
        virtual void Draw();

        virtual void UpArrowKeyDown();
        virtual void DownArrowKeyDown();
        virtual void LeftArrowKeyDown();
        virtual void RightArrowKeyDown();
    };

    extern GraphicsManager* g_pGraphicsManager;
}
