#pragma once

#include "BaseDrawPass.hpp"
#include "GeometrySubPass.hpp"
#include "SkyboxSubPass.hpp"
#include "PresentSubPass.hpp"

namespace My {
	class ForwardGeometryPass : __implements BaseDrawPass {
	public:
		ForwardGeometryPass(IGraphicsManager* gmr, IApplication* pApp) : BaseDrawPass(gmr)
		{
			m_PassType = PassType::kForwardGeometry;
			m_DrawSubPasses.push_back(std::make_shared<GeometrySubPass>(gmr, pApp));
			m_DrawSubPasses.push_back(std::make_shared<SkyboxSubPass>(gmr, pApp));
		}
	};
}