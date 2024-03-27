#pragma once

#include "IDrawSubPass.hpp"
#include "IGraphicsManager.hpp"

namespace My {
	class BaseSubPass : __implements IDrawSubPass {
	public:
		BaseSubPass(IGraphicsManager* gmr) : m_pGraphicsManager(gmr) {}
		BaseSubPass() = default;

		void BeginSubPass() override {}
		void EndSubPass() override {}
	protected:
		IGraphicsManager* m_pGraphicsManager;
	};
}