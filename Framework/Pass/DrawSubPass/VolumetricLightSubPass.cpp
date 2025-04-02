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
	if (m_bDrawVolumetricLight) {
		m_pGraphicsManager->SetPipelineStatus("VolumetricLight");
		m_pGraphicsManager->DrawVolumetricLight(frame);
	}
}

int* My::VolumetricLightSubPass::GetMarchingStepsPtr()
{
	return &m_iMarchingSteps;
}

float* My::VolumetricLightSubPass::GetSampleIntensity()
{
	return &m_fSampleIntensity;
}

bool* My::VolumetricLightSubPass::GetDrawVolumetricLight()
{
	return &m_bDrawVolumetricLight;
}