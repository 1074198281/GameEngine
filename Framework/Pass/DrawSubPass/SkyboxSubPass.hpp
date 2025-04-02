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
		bool* GetDrawSkyBoxPtr();
		bool GetDrawSkyBox();
		int* GetSkyBoxIndexPtr();
		int GetSkyBoxIndex();

	public:
		bool m_bDrawSkybox = false;
		int m_iSkyBoxIndex{ 0 };
	};
}