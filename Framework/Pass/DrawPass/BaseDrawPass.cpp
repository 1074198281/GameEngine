#include "BaseDrawPass.hpp"

void My::BaseDrawPass::BeginPass(Frame& frame)
{
	//m_pGraphicsManager->BeginPass(frame);
}

void My::BaseDrawPass::EndPass(Frame& frame)
{
	//m_pGraphicsManager->EndPass(frame);
}

void My::BaseDrawPass::Draw(Frame& frame)
{
	for (auto pass : m_DrawSubPasses) {
		pass->BeginSubPass();
		pass->Draw(frame);
		pass->EndSubPass();
	}
}