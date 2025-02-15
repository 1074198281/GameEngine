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
			continue;
			if (cube_shadow_map_index > MAX_LIGHT_NUM) {
				continue;
			}
			const char* PipelineStatusName = "OmniLight";
			m_pGraphicsManager->SetPipelineStatus(PipelineStatusName);
			light.LightShadowMapIndex = cube_shadow_map_index++;
			m_pGraphicsManager->SetShadowResources(frame, i);
		}
		break;
		case LightType::Spot:
		{
			if (shadow_map_index > MAX_LIGHT_NUM) {
				continue;
			}
			const char* PipelineStatusName = "SpotLight";
			m_pGraphicsManager->SetPipelineStatus(PipelineStatusName);
			light.LightShadowMapIndex = shadow_map_index++;
			m_pGraphicsManager->SetShadowResources(frame, i);
		}
		break;
		case LightType::Area:
		{
			continue;
			if (shadow_map_index > MAX_LIGHT_NUM) {
				continue;
			}
			const char* PipelineStatusName = "AreaLight";
			m_pGraphicsManager->SetPipelineStatus(PipelineStatusName);
			light.LightShadowMapIndex = shadow_map_index++;
			m_pGraphicsManager->SetShadowResources(frame, i);
		}
		break;
		case LightType::Infinity:
		{
			continue;
			if (global_shadow_map_index > MAX_LIGHT_NUM) {
				continue;
			}
			const char* PipelineStatusName = "InfiLight";
			m_pGraphicsManager->SetPipelineStatus(PipelineStatusName);
			light.LightShadowMapIndex = global_shadow_map_index++;
			m_pGraphicsManager->SetShadowResources(frame, i);
		}
		break;
		default:
			break;
		}

		m_pGraphicsManager->DrawBatch(frame, i, true, false);
		m_pGraphicsManager->SetShadowMapState(light.Type, light.LightShadowMapIndex);
	}

	frame.FrameContext.ShadowMap.size = shadow_map_index;
	frame.FrameContext.GlobalShadowMap.size = global_shadow_map_index;
	frame.FrameContext.CubeShadowMap.size = cube_shadow_map_index;
}