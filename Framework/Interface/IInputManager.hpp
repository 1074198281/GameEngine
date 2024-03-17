#pragma once

#include "IRuntimeModule.hpp"

namespace My {
	__Interface IInputManager : public IRuntimeModule{
	public:
		IInputManager() = default;
		virtual ~IInputManager() = default;

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