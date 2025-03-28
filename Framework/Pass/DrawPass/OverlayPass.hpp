#pragma once

#include "BaseDrawPass.hpp"
#include "GuiSubPass.hpp"
#include "GuassBlurSubPass.hpp"
#include "OverlaySubPass.hpp"
#include "VolumetricLightSubPass.hpp"

namespace My {
	enum OverlaySubPassType {
		kVolumetricLightSub,
		kOverlaySub,
		kGaussBlurSub,
		kGuiSub,

		kOverlaySubPassType
	};

	class OverlayPass : __implements BaseDrawPass {
	public:
		OverlayPass(IGraphicsManager* gmr, IApplication* pApp) : BaseDrawPass(gmr)
		{
			m_PassType = PassType::kOverlay;
			m_DrawSubPasses.push_back(std::make_shared<VolumetricLightSubPass>(gmr, pApp));
			m_DrawSubPasses.push_back(std::make_shared<OverlaySubPass>(gmr, pApp));
			m_DrawSubPasses.push_back(std::make_shared<GuassBlurSubPass>(gmr, pApp));
			m_DrawSubPasses.push_back(std::make_shared<GuiSubPass>(gmr, pApp));
		}

		IDrawSubPass* GetSubPass(OverlaySubPassType Type)
		{
			return m_DrawSubPasses[Type].get();
		}
	};
}