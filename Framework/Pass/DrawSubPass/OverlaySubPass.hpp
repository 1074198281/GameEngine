#pragma once

#include "BaseSubPass.hpp"

namespace My {
	class OverlaySubPass : __implements BaseSubPass {
	public:
		using BaseSubPass::BaseSubPass;

		void BeginSubPass() override;
		void EndSubPass() override;
		void Draw(Frame& frame) override;

	public:
		bool* GetDrawWaterDropsScreen();
		bool* GetDrawSea();
	private:
		bool m_bDrawWaterDropsScreen = false;
		bool m_bDrawSea = false;
	};
}