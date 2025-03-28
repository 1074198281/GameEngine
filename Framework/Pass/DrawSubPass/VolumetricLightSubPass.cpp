#include "VolumetricLightSubPass.hpp"

void My::VolumetricLightSubPass::BeginSubPass()
{
	m_pGraphicsManager->BeginSubPass("VolumetricLightSubPass");
}

void My::VolumetricLightSubPass::EndSubPass()
{
	m_pGraphicsManager->EndSubPass();
}

void My::VolumetricLightSubPass::Draw(Frame& frame)
{
	m_pGraphicsManager->SetPipelineStatus("VolumetricLight");
	m_pGraphicsManager->DrawVolumetricLight(frame);
}

int* My::VolumetricLightSubPass::GetMarchingSteps()
{
	return &m_iMarchingSteps;
}