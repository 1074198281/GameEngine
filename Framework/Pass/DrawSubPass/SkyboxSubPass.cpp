#include "SkyboxSubPass.hpp"

void My::SkyboxSubPass::Draw(Frame& frame)
{
	m_pGraphicsManager->DrawSkybox(frame);
}