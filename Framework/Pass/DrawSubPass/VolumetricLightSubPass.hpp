#pragma once

#include "BaseSubPass.hpp"

namespace My {
	class VolumetricLightSubPass : __implements BaseSubPass {
	public:
		using BaseSubPass::BaseSubPass;
		void BeginSubPass() override;
		void EndSubPass() override;
		void Draw(Frame& frame) override;

	public:
		int* GetMarchingStepsPtr();
		float* GetSampleIntensity();
	protected:
		int m_iMarchingSteps = 20;
		float m_fSampleIntensity = 1.0f;
	};
}