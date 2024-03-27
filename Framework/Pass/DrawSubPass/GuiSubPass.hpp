#pragma once

#include "BaseSubPass.hpp"

namespace My {
	class GuiSubPass : __implements BaseSubPass {
	public:
		using BaseSubPass::BaseSubPass;
		void Draw(Frame& frame) override;
	};
}