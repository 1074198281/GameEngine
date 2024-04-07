#pragma once

#include "BaseSubPass.hpp"
#include "Scene.hpp"

namespace My {
	class GuiSubPass : __implements BaseSubPass {
	public:
		GuiSubPass(IGraphicsManager* gmr) : BaseSubPass(gmr) {
			UpdateScene();
		}
		void UpdateScene() { m_pScene = m_pGraphicsManager->GetSceneForGuiBuild(); }

		void BeginSubPass() override;
		void EndSubPass() override;
		void Draw(Frame& frame) override;
	private:
		Scene* m_pScene;
	};
}