#pragma once

#include "IDrawSubPass.hpp"
#include "IGraphicsManager.hpp"
#include "IApplication.hpp"

namespace My {
	class BaseSubPass : __implements IDrawSubPass {
	public:
		BaseSubPass(IGraphicsManager* gmr, IApplication* pApp) : m_pGraphicsManager(gmr), m_pApp(pApp) {}
		BaseSubPass() = default;

		void BeginSubPass() override {}
		void EndSubPass() override {}
	protected:
		IGraphicsManager* m_pGraphicsManager;
		IApplication* m_pApp;
	};
}