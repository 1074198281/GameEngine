#include "GeometrySubPass.hpp"

void My::GeometrySubPass::Draw(Frame& frame)
{
	//m_pGraphicsManager->SetShadowMaps();
	m_pGraphicsManager->Draw();		//reserved function, for now we haven't clip draw() function dependent from d3d12GraphicsManager
}