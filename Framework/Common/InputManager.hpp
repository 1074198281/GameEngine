#pragma once

#include "IRuntimeModule.hpp"

namespace My {

    class InputManager : __implements IRuntimeModule
    {
    public:
        virtual int Initialize();
        virtual void Finalize();
        virtual void Tick();

        void UpArrowKeyDown();
        void UpArrowKeyUp();
        void DownArrowKeyDown();
        void DownArrowKeyUp();
        void LeftArrowKeyDown();
        void LeftArrowKeyUp();
        void RightArrowKeyDown();
        void RightArrowKeyUp();
    };
    extern InputManager* g_pInputManager;
}