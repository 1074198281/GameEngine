#include "SkyboxSubPass.hpp"

void My::SkyboxSubPass::BeginSubPass()
{
	m_pGraphicsManager->BeginSubPass("RenderSkybox");
}

void My::SkyboxSubPass::EndSubPass()
{
	m_pGraphicsManager->EndSubPass();
}

void My::SkyboxSubPass::Draw(Frame& frame)
{
	m_pGraphicsManager->DrawSkybox(frame);
}