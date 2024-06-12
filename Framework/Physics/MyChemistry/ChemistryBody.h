#pragma once

#include "ChemistryActor.h"
#include "ChemistryBaseProperty.h"

#include "PxPhysicsAPI.h"

#include "base_math.hpp"
#include <string>

namespace MyChemistry {
	class ChemistryBody : public ChemistryActor {
	public:
		ChemistryBody();
		~ChemistryBody() override;

	public:
#ifdef _DEBUG
		void SetName(std::string name) { m_Name = name; }
#endif

	public:
		void SetPosition(const MyMath::Vector3f& pos);
		void SetBaseProperty(const BaseProperty& pro);
		void SetRigidActor(physx::PxRigidActor* pRigidActor);

	private:
		void ReactionWithOther(ChemistryBody* other);

	protected:
		MyMath::Vector3f m_Position;
		BaseProperty m_Property;
		physx::PxRigidActor* m_pRigidActor;

#ifdef _DEBUG
		std::string m_Name;
#endif
 	};
}