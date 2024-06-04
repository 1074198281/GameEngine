#pragma once

#include "ChemistryBody.h"

#include <list>

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
		std::list<ChemistryBody*> m_ListChemistryBodies;


		int m_RegionTemperature = 0;

	};

}