#pragma once

#include "BaseSubPass.hpp"

namespace My {
	class GuiSubPass : __implements BaseSubPass {
	public:
		GuiSubPass(IGraphicsManager* gmr, IApplication* pApp) : BaseSubPass(gmr, pApp) {}

		void BeginSubPass() override;
		void EndSubPass() override;
		void Draw(Frame& frame) override;
	};
}