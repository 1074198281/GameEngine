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
		bool* GetDrawVolumetricLight();
	protected:
		int m_iMarchingSteps = 10;
		float m_fSampleIntensity = 1.0f;
		bool m_bDrawVolumetricLight = false;
	};
}