#include "OverlaySubPass.hpp"


void My::OverlaySubPass::BeginSubPass()
{
	m_pGraphicsManager->BeginSubPass("OverlaySubPass");
}

void My::OverlaySubPass::EndSubPass()
{
	m_pGraphicsManager->EndSubPass();
}

void My::OverlaySubPass::Draw(Frame& frame)
{
	if (m_bDrawWaterDropsScreen) {
		m_pGraphicsManager->SetPipelineStatus("WaterDrops");
		m_pGraphicsManager->DrawWaterDrops(frame);
	}
	if (m_bDrawSea) {
		m_pGraphicsManager->SetPipelineStatus("Sea");
		m_pGraphicsManager->DrawSea(frame);
	}
}

bool* My::OverlaySubPass::GetDrawWaterDropsScreen()
{
	return &m_bDrawWaterDropsScreen;
}

bool* My::OverlaySubPass::GetDrawSea()
{
	return &m_bDrawSea;
}