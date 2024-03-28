#pragma once

#include "BaseDrawPass.hpp"
#include "GeometrySubPass.hpp"
#include "SkyboxSubPass.hpp"

namespace My {
	class ForwardGeometryPass : __implements BaseDrawPass {
	public:
		ForwardGeometryPass(IGraphicsManager* gmr) : BaseDrawPass(gmr)
		{
			m_DrawSubPasses.push_back(std::make_shared<GeometrySubPass>(gmr));
			m_DrawSubPasses.push_back(std::make_shared<SkyboxSubPass>(gmr));
		}
	};
}