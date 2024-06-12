#include "ChemistryWorld.h"

MyChemistry::ChemistryWorld::ChemistryWorld()
{

}

MyChemistry::ChemistryWorld::~ChemistryWorld()
{

}

int MyChemistry::ChemistryWorld::AddChemistryBodyToWorld(ChemistryBody* body)
{
	m_ListChemistryBodies.emplace_back(body);
	return 0;
}

void MyChemistry::ChemistryWorld::Simulate(float deltaTime)
{
	SimuForMaterials();
	SimiForElements();
}

void MyChemistry::ChemistryWorld::SimuForMaterials()
{

}

void MyChemistry::ChemistryWorld::SimiForElements()
{
	auto active = m_ListChemistryBodies.begin();
	std::list<ChemistryBody*>::iterator passive = m_ListChemistryBodies.begin();
	for ( ; passive != m_ListChemistryBodies.end(); passive++)
	{
		if (active == passive)
		{
			continue;
		}
		for (; active != m_ListChemistryBodies.end(); active++)
		{

		}
	}
}