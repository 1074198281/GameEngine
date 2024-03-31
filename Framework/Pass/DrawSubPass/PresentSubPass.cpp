#include "PresentSubPass.hpp"

void My::PresentSubPass::BeginSubPass()
{
	m_pGraphicsManager->BeginSubPass("RenderPresent");
}

void My::PresentSubPass::EndSubPass()
{
	m_pGraphicsManager->EndSubPass();
}

void My::PresentSubPass::Draw(Frame& frame)
{
	//m_pGraphicsManager->SetShadowMaps();
	m_pGraphicsManager->DrawPresent(frame);
}