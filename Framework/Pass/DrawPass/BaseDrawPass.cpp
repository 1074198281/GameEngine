#include "BaseDrawPass.hpp"

void My::BaseDrawPass::BeginPass(Frame& frame)
{
	if (m_PassType == kOverlay)
	{
		m_pGraphicsManager->BeginOverlayPass();
	}
}

void My::BaseDrawPass::EndPass(Frame& frame)
{
	if (m_PassType == kOverlay)
	{
		m_pGraphicsManager->EndOverlayPass();
	}
}

void My::BaseDrawPass::Draw(Frame& frame)
{
	for (auto pass : m_DrawSubPasses) {
		pass->BeginSubPass();
		pass->Draw(frame);
		pass->EndSubPass();
	}
}