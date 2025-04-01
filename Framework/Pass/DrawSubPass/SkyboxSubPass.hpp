#pragma once

#include "BaseSubPass.hpp"

namespace My {
	class SkyboxSubPass : __implements BaseSubPass {
	public:
		using BaseSubPass::BaseSubPass;
		void BeginSubPass() override;
		void EndSubPass() override;
		void Draw(Frame& frame) override;

	public:
		bool* GetDrawSkyBox();

	public:
		bool m_bDrawSkybox;

	};
}