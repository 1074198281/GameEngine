#pragma once

#include "IRuntimeModule.hpp"
#include "FrameStructure.hpp"

namespace My {
	__Interface IPass : public IRuntimeModule{
	public:
		virtual ~IPass() = 0;
		virtual void BeginPass(Frame& frame) = 0;
		virtual void EndPass(Frame& frame) = 0;
	};
}
