#pragma once

#include "BaseDrawPass.hpp"

namespace My {
	class ShadowMapPass : __implements BaseDrawPass {
	public:
		ShadowMapPass(IGraphicsManager* gmr) : BaseDrawPass(gmr) {}

		void BeginPass(Frame& frame) override;
		void EndPass(Frame& frame) override;
		void Draw(Frame& frame) override;

	private:
		void GenerateShadowMap(Frame& frame, Light& light, uint8_t idx,
			uint32_t& shadow_map_index, uint32_t& global_shadow_map_index, uint32_t& cube_shadow_map_index);
		void GenerateVolumetric(Frame& frame, Light& light, uint8_t idx,
			uint32_t& shadow_map_index, uint32_t& global_shadow_map_index, uint32_t& cube_shadow_map_index);
	};
}