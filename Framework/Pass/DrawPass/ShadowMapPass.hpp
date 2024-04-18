#pragma once

#include "BaseDrawPass.hpp"

namespace My {
	class ShadowMapPass : __implements BaseDrawPass {
	public:
		ShadowMapPass(IGraphicsManager* gmr) : BaseDrawPass(gmr) {}

		void BeginPass(Frame& frame) override;
		void EndPass(Frame& frame) override;
		void Draw(Frame& frame) override;
	};
}