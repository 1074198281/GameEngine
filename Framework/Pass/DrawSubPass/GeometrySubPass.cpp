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
	//m_pGraphicsManager->SetPipelineStatus("BlinnPhongLighting");
	//m_pGraphicsManager->SetPipelineStatus("Default");
	//m_pGraphicsManager->SetBatchResources(frame);
	// 将SetPipelineStatus和SetBatchResources迁移到DrawBatch里面去
	// PCG渲染时需要设置不同的pso，为PCG生成的mesh使用自己的pso
	// 
	//m_pGraphicsManager->SetShadowMaps();
	m_pGraphicsManager->DrawBatch(frame, -1, false, true);
}