#pragma once
#include <iostream>
#include "Interface.hpp"
#include "IPass.hpp"

namespace My {
	class IDrawPass : __implements IPass {
	public:
		virtual void Draw(Frame& frame) = 0;
	};
}