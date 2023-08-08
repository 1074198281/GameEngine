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

        virtual void MoveCameraXPositive();
        virtual void MoveCameraXNegative();
        virtual void MoveCameraYPositive();
        virtual void MoveCameraYNegative();
    };

    extern GraphicsManager* g_pGraphicsManager;
}
