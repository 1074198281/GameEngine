#include "GeometrySubPass.hpp"

void My::GeometrySubPass::BeginSubPass()
{
	m_pGraphicsManager->BeginSubPass("RenderGeometry");
}

void My::GeometrySubPass::EndSubPass()
{
	m_pGraphicsManager->EndSubPass();
}

void My::GeometrySubPass::Draw(Frame& frame)
{
	//m_pGraphicsManager->SetPipelineStatus("LambertGouraudLighting");
	m_pGraphicsManager->SetPipelineStatus("BlinnPhongLighting");
	m_pGraphicsManager->SetBatchResources(frame);
	//m_pGraphicsManager->SetShadowMaps();
	m_pGraphicsManager->DrawBatch(frame);
}