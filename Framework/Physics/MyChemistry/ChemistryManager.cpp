#include "ChemistryManager.hpp"
#include "BaseApplication.hpp"

My::ChemistryManager::~ChemistryManager()
{

}

int My::ChemistryManager::Initialize()
{

}

void My::ChemistryManager::Finalize()
{

}

void My::ChemistryManager::Tick()
{

}

void My::ChemistryManager::CreateChemistryBody(SceneGeometryNode& node, SceneObjectGeometry& Geometry)
{
	
}

void My::ChemistryManager::DeleteChemistryBode(SceneGeometryNode& node)
{
	if (!node.UnLinkChemistryBody())
	{
		return;
	}

}

int My::ChemistryManager::CreateChemistryBodies()
{
	auto pSceneManager = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager();
	auto& scene = pSceneManager->GetSceneForRendering();

	// Geometries
	for (auto _it : scene.GeometryNodes)
	{
		auto pGeometryNode = _it.second;
		auto pGeometry = scene.GetGeometry(pGeometryNode->GetSceneObjectRef());
		assert(pGeometry);

		CreateChemistryBody(*pGeometryNode, *pGeometry);
	}

	return 0;
}

void My::ChemistryManager::ClearChemistryBodies()
{
	auto pSceneManager = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager();

	auto& scene = pSceneManager->GetSceneForRendering();

	// Geometries
	for (auto _it : scene.GeometryNodes)
	{
		auto pGeometryNode = _it.second;
		DeleteChemistryBode(*pGeometryNode);
	}
}