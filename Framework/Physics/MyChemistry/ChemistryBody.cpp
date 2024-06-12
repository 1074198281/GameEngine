#include "ChemistryBody.h"

using namespace MyMath;

MyChemistry::ChemistryBody::ChemistryBody()
{

}

MyChemistry::ChemistryBody::~ChemistryBody()
{

}

void MyChemistry::ChemistryBody::SetPosition(const Vector3f& pos)
{
	m_Position = pos;
}

void MyChemistry::ChemistryBody::SetBaseProperty(const BaseProperty& pro)
{
	m_Property = pro;
}

void MyChemistry::ChemistryBody::SetRigidActor(physx::PxRigidActor* pRigidActor)
{
	m_pRigidActor = pRigidActor;
}

void MyChemistry::ChemistryBody::ReactionWithOther(ChemistryBody* other)
{

}