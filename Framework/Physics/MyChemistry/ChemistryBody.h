#pragma once

#include "ChemistryActor.h"
#include "ChemistryBaseProperty.h"
#include "base_math.hpp"

namespace MyChemistry {
	class ChemistryBody : public ChemistryActor {
	public:
		ChemistryBody();
		~ChemistryBody() override;

	public:
		void SetPosition(MyMath::Vector3f pos);
	protected:
		MyMath::Vector3f m_Position;
		BaseProperty m_Property;
	};
}