#pragma once

#include "IInputManager.hpp"


namespace My {

    class InputManager : __implements IInputManager
    {
    public:
        ~InputManager() override = default;
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
        void NumPadKeyDown(long long key);
        void NumPadKeyUp(long long key);
        void FunctionKeyDown(long long key);
        void FunctionKeyUp(long long key);
    };
}