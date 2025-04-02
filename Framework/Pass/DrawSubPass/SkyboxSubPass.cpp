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
	if (m_bDrawSkybox) {
		m_pGraphicsManager->SetPipelineStatus("Skybox");
		m_pGraphicsManager->DrawSkybox(frame);
	}
}

bool* My::SkyboxSubPass::GetDrawSkyBoxPtr()
{
	return &m_bDrawSkybox;
}

bool My::SkyboxSubPass::GetDrawSkyBox()
{
	return m_bDrawSkybox;
}

int* My::SkyboxSubPass::GetSkyBoxIndexPtr()
{
	return &m_iSkyBoxIndex;
}

int My::SkyboxSubPass::GetSkyBoxIndex()
{
	return m_iSkyBoxIndex;
}