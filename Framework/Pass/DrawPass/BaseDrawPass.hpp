#pragma once

#include "IDrawPass.hpp"
#include "IGraphicsManager.hpp"
#include "IDrawSubPass.hpp"
#include <vector>
#include <memory>

namespace My {
	enum PassType { kPassTypeNone, kForwardGeometry, kOverlay, kPresent, kPassType };
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
		PassType m_PassType;
		std::vector<std::shared_ptr<IDrawSubPass>> m_DrawSubPasses;
	};
}