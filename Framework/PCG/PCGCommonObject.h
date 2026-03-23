#pragma once

#include <string>
#include "../SceneGraph/SceneNodeGeometry.hpp"

namespace My::PCG
{
	class PCGCommonObject
	{
	public:
		PCGCommonObject();
		PCGCommonObject(std::string name);
		~PCGCommonObject();


	public:
		std::shared_ptr<SceneGeometryNode> GetGeoNode() { return m_pGeoNode; }
		std::shared_ptr<SceneObjectGeometry> GetGeoObject() { return m_pGeoObject; }

	public:
		std::string m_Name;
	protected:
		std::shared_ptr<SceneGeometryNode> m_pGeoNode;
		std::shared_ptr<SceneObjectGeometry> m_pGeoObject;
	};
}