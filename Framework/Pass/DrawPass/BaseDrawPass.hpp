#pragma once

#include "IDrawPass.hpp"
#include "IGraphicsManager.hpp"
#include "IDrawSubPass.hpp"
#include <vector>
#include <memory>

namespace My {
	class BaseDrawPass : __implements IDrawPass {
	public:
		BaseDrawPass(IGraphicsManager* gmr) : m_pGraphicsManager(gmr) {}

		void BeginPass(Frame& frame) override;
		void EndPass(Frame& frame) override;
		void Draw(Frame& frame) override;
	protected:
		BaseDrawPass() = default;

	protected:
		IGraphicsManager* m_pGraphicsManager;
		std::vector<std::shared_ptr<IDrawSubPass>> m_DrawSubPasses;
	};
}