#include "PCGCommonObject.h"

using namespace My::PCG;

PCGCommonObject::PCGCommonObject()
{
	m_pGeoNode = std::make_shared<SceneGeometryNode>();
	m_pGeoObject = std::make_shared<SceneObjectGeometry>();
}

PCGCommonObject::PCGCommonObject(std::string name)
{
	m_Name = name;
	m_pGeoNode = std::make_shared<SceneGeometryNode>();
	m_pGeoObject = std::make_shared<SceneObjectGeometry>();
}

PCGCommonObject::~PCGCommonObject()
{
	
}