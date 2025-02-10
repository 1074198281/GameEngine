#include "ShadowMapPass.hpp"


void My::ShadowMapPass::BeginPass(Frame& frame)
{
	m_pGraphicsManager->BeginSubPass("Generate Shadow Map");
}

void My::ShadowMapPass::EndPass(Frame& frame)
{
	m_pGraphicsManager->EndSubPass();
}

void My::ShadowMapPass::Draw(Frame& frame)
{
	uint32_t shadow_map_index = 0;
	uint32_t global_shadow_map_index = 0;
	uint32_t cube_shadow_map_index = 0;

	for (int i = 0; i < frame.FrameContext.LightNum; i++) {
		auto& light = frame.LightInfomation.Lights[i];

		switch (light.Type)
		{
		case LightType::Omni:
		{
			if (cube_shadow_map_index > MAX_LIGHT_NUM) {
				continue;
			}
			const char* PipelineStatusName = "Omni Light";
			m_pGraphicsManager->SetPipelineStatus(PipelineStatusName);
			m_pGraphicsManager->SetShadowResources(frame, light);
			light.LightShadowMapIndex = cube_shadow_map_index++;
		}
		break;
		case LightType::Spot:
		{
			if (shadow_map_index > MAX_LIGHT_NUM) {
				continue;
			}
			const char* PipelineStatusName = "Spot Light";
			m_pGraphicsManager->SetPipelineStatus(PipelineStatusName);
			m_pGraphicsManager->SetShadowResources(frame, light);
			light.LightShadowMapIndex = shadow_map_index++;
		}
		break;
		case LightType::Area:
		{
			if (shadow_map_index > MAX_LIGHT_NUM) {
				continue;
			}
			const char* PipelineStatusName = "Area Light";
			m_pGraphicsManager->SetPipelineStatus(PipelineStatusName);
			m_pGraphicsManager->SetShadowResources(frame, light);
			light.LightShadowMapIndex = shadow_map_index++;
		}
		break;
		case LightType::Infinity:
		{
			if (global_shadow_map_index > MAX_LIGHT_NUM) {
				continue;
			}
			const char* PipelineStatusName = "Infi Light";
			m_pGraphicsManager->SetPipelineStatus(PipelineStatusName);
			m_pGraphicsManager->SetShadowResources(frame, light);
			light.LightShadowMapIndex = global_shadow_map_index++;
		}
		break;
		default:
			break;
		}

		m_pGraphicsManager->DrawBatch(frame, true, false);
	}

	frame.FrameContext.ShadowMap.size = shadow_map_index;
	frame.FrameContext.GlobalShadowMap.size = global_shadow_map_index;
	frame.FrameContext.CubeShadowMap.size = cube_shadow_map_index;
}