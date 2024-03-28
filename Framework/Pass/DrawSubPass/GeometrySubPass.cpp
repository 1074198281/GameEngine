#include "GeometrySubPass.hpp"

void My::GeometrySubPass::Draw(Frame& frame)
{
	//m_pGraphicsManager->SetShadowMaps();
	m_pGraphicsManager->DrawBatch(frame);
}