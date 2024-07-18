#pragma once

#include "BaseSubPass.hpp"

namespace My {
	class OverlaySubPass : __implements BaseSubPass {
	public:
		using BaseSubPass::BaseSubPass;

		void BeginSubPass() override;
		void EndSubPass() override;
		void Draw(Frame& frame) override;
	};
}