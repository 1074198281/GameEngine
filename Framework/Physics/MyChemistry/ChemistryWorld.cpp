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
}

void MyChemistry::ChemistryWorld::Simulate(float deltaTime)
{


}