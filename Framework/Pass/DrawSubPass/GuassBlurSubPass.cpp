#include "GuassBlurSubPass.hpp"


void My::GuassBlurSubPass::BeginSubPass()
{
	m_pGraphicsManager->BeginSubPass("GuassBlur_CS");
}

void My::GuassBlurSubPass::EndSubPass()
{
	m_pGraphicsManager->EndSubPass();
}

void My::GuassBlurSubPass::Draw(Frame& frame)
{
	m_pGraphicsManager->SetPipelineStatus("GuassBlur_CS");
	m_pGraphicsManager->DrawGuassBlur(frame);
}