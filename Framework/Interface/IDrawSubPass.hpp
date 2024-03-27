#pragma once
#include <iostream>
#include "FrameStructure.hpp"
#include "ISubPass.hpp"

namespace My {
	class IDrawSubPass : __implements ISubPass {
	public:
		virtual void Draw(Frame& frame) = 0;
	};
}