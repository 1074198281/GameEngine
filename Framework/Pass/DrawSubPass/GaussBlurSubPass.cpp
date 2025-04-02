#include "GaussBlurSubPass.hpp"


void My::GaussBlurSubPass::BeginSubPass()
{
	m_pGraphicsManager->BeginSubPass("GuassBlur_CS");
}

void My::GaussBlurSubPass::EndSubPass()
{
	m_pGraphicsManager->EndSubPass();
}

void My::GaussBlurSubPass::Draw(Frame& frame)
{
	if (m_bEnableGaussBlur)
	{
		m_pGraphicsManager->SetPipelineStatus("GaussBlur_CS");
		m_pGraphicsManager->DrawGaussBlur(frame);
	}
}

bool* My::GaussBlurSubPass::GetGaussBlurPtr()
{
	return &m_bEnableGaussBlur;
}

bool My::GaussBlurSubPass::GetGaussBlur()
{
	return m_bEnableGaussBlur;
}