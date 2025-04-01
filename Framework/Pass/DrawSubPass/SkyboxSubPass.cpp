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
	m_pGraphicsManager->SetPipelineStatus("Skybox");
	m_pGraphicsManager->DrawSkybox(frame);
}

bool* My::SkyboxSubPass::GetDrawSkyBox()
{
	return &m_bDrawSkybox;
}