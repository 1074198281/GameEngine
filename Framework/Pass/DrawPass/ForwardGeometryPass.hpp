#pragma once

#include "BaseDrawPass.hpp"
#include "GeometrySubPass.hpp"
#include "SkyboxSubPass.hpp"

namespace My {
	enum ForwardGeometrySubPass
	{
		kGeometrySubPass,
		kSkyboxSubPass,

		kForwardGeometrySubPass
	};

	class ForwardGeometryPass : __implements BaseDrawPass {
	public:
		ForwardGeometryPass(IGraphicsManager* gmr, IApplication* pApp) : BaseDrawPass(gmr)
		{
			m_PassType = DrawPassType::kForwardGeometryPass;
			m_DrawSubPasses.push_back(std::make_shared<GeometrySubPass>(gmr, pApp));
			m_DrawSubPasses.push_back(std::make_shared<SkyboxSubPass>(gmr, pApp));
		}

		GeometrySubPass* GetGeometrySubPass()
		{
			return reinterpret_cast<GeometrySubPass*>(m_DrawSubPasses[kGeometrySubPass].get());
		}

		SkyboxSubPass* GetSkyboxSubPass()
		{
			return reinterpret_cast<SkyboxSubPass*>(m_DrawSubPasses[kSkyboxSubPass].get());
		}
	};
}