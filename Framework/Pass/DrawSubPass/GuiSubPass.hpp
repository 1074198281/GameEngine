#pragma once

#include "BaseSubPass.hpp"

namespace My {
	class ISceneManager;
	class GuiSubPass : __implements BaseSubPass {
	public:
		GuiSubPass(IGraphicsManager* gmr, IApplication* pApp) : BaseSubPass(gmr, pApp) {}

		void BeginSubPass() override;
		void EndSubPass() override;
		void Draw(Frame& frame) override;

	private:
		void DrawEmptyScene(const Scene& Scene, ISceneManager* pSceneManager);

	};
}