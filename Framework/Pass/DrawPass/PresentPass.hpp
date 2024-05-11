#pragma once

#include "BaseDrawPass.hpp"
#include "PresentSubPass.hpp"

namespace My {
	class PresentPass : __implements BaseDrawPass {
	public:
		PresentPass(IGraphicsManager* gmr, IApplication* pApp) : BaseDrawPass(gmr)
		{
			m_PassType = PassType::kPresent;
			m_DrawSubPasses.push_back(std::make_shared<PresentSubPass>(gmr, pApp));
		}
	};
}