#pragma once

#include "BaseDrawPass.hpp"
#include "GuiSubPass.hpp"
#include "GaussBlurSubPass.hpp"
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
			m_PassType = DrawPassType::kOverlayPass;
			m_DrawSubPasses.push_back(std::make_shared<VolumetricLightSubPass>(gmr, pApp));
			m_DrawSubPasses.push_back(std::make_shared<OverlaySubPass>(gmr, pApp));
			m_DrawSubPasses.push_back(std::make_shared<GaussBlurSubPass>(gmr, pApp));
			m_DrawSubPasses.push_back(std::make_shared<GuiSubPass>(gmr, pApp));
		}

		IDrawSubPass* GetSubPass(OverlaySubPassType Type)
		{
			return m_DrawSubPasses[Type].get();
		}

		VolumetricLightSubPass* GetVolumetricSubPass()
		{
			return reinterpret_cast<VolumetricLightSubPass*>(m_DrawSubPasses[kVolumetricLightSub].get());
		}

		OverlaySubPass* GetOverlaySubPass()
		{
			return reinterpret_cast<OverlaySubPass*>(m_DrawSubPasses[kOverlaySub].get());
		}

		GaussBlurSubPass* GetGaussSubPass()
		{
			return reinterpret_cast<GaussBlurSubPass*>(m_DrawSubPasses[kGaussBlurSub].get());
		}

		GuiSubPass* GetGuiSubPass()
		{
			return reinterpret_cast<GuiSubPass*>(m_DrawSubPasses[kGuiSub].get());
		}

	private:
		
	};
}