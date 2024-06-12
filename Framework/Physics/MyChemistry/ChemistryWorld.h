#pragma once

#include "ChemistryBody.h"

#include <list>
#include <unordered_map>

namespace MyChemistry {

	class ChemistryWorld {
	public:
		ChemistryWorld();
		~ChemistryWorld();

	public:
		void Simulate(float deltaTime);

	public:
		int AddChemistryBodyToWorld(ChemistryBody* body);


	private:
		void SimuForMaterials();
		void SimiForElements();

	private:
		std::list<ChemistryBody*> m_ListChemistryBodies;
		std::unordered_map<physx::PxRigidActor*, ChemistryBody*> m_RigidChemistryMap;

		int m_RegionTemperature = 0;
	};

}