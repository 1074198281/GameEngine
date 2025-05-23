#pragma once

#include "BaseSubPass.hpp"

namespace My {
	class GaussBlurSubPass : __implements BaseSubPass {
	public:
		using BaseSubPass::BaseSubPass;

		void BeginSubPass() override;
		void EndSubPass() override;
		void Draw(Frame& frame) override;

	public:
		bool* GetGaussBlurPtr();
		bool GetGaussBlur();
	private:
		bool m_bEnableGaussBlur = false;
	};
}