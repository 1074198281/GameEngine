#pragma once
#include <iostream>
#include "Interface.hpp"
#include "IPass.hpp"

namespace My {
	class IDispatchPass : __implements IPass {
	public:
		virtual void Dispatch(Frame& frame) = 0;
	};
}