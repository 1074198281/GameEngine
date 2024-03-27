#pragma once

#include "BaseSubPass.hpp"

namespace My {
	class GeometrySubPass : __implements BaseSubPass {
	public:
		using BaseSubPass::BaseSubPass;
		void Draw(Frame& frame) override;
	};
}