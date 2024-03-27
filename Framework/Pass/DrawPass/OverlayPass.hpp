#pragma once

#include "BaseDrawPass.hpp"
#include "GuiSubPass.hpp"

namespace My {
	class OverlayPass : __implements BaseDrawPass {
	public:
		OverlayPass(IGraphicsManager* gmr) : BaseDrawPass(gmr)
		{
			m_DrawSubPasses.push_back(std::make_shared<GuiSubPass>());
		}
	};
}