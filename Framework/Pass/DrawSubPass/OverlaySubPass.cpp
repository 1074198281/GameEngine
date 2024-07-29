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
	m_pGraphicsManager->SetPipelineStatus("OverlaySub");
	m_pGraphicsManager->DrawOverlay(frame);
}