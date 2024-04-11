#include "PhysXPhysicsManager.hpp"
#include "PxPhysXConfig.h"
#include "PxConfig.h"
#include "geommath.hpp"

using namespace physx;

int My::PhysXPhysicsManager::Initialize()
{
	
	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocatorCallBack, m_DefaultErrorCallBack);
	if (!m_PxFoundation) {
		assert(false, "PxCreateFoundation Failed!");
	}


	// for PhysX Visual Debugger
	bool recordMemoryAllocators = true;
	m_PxPvd = PxCreatePvd(*m_PxFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_PxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	m_PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), recordMemoryAllocators, m_PxPvd);
	if (!m_PxPhysics) {
		assert(false, "PxCreatePhysics Failed!");
	}

	PxSceneDesc sceneDesc(m_PxPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_PxDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	m_PxScene = m_PxPhysics->createScene(sceneDesc);

	//PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
	//if (pvdClient)
	//{
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}
}

void My::PhysXPhysicsManager::Finalize()
{
	PX_RELEASE(m_PxScene);
	PX_RELEASE(m_PxDispatcher);
	PX_RELEASE(m_PxPhysics);
	if (m_PxPvd)
	{
		PxPvdTransport* transport = m_PxPvd->getTransport();
		m_PxPvd->release();	m_PxPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(m_PxFoundation);
}

void My::PhysXPhysicsManager::Tick()
{
	m_PxScene->simulate(1.0f / 60.0f);
	m_PxScene->fetchResults(true);
}

void My::PhysXPhysicsManager::CreateRigidBody(SceneGeometryNode& node, const SceneObjectGeometry& geometry)
{
	switch (geometry.CollisionType())
	{
	case kSceneObjectCollisionTypeSphere:
	{

	}
	break;
	case kSceneObjectCollisionTypeBox:
	{

	}
	break;
	case kSceneObjectCollisionTypePlane:
	{

	}
	break;
	default:
		break;
	}

}
void My::PhysXPhysicsManager::DeleteRigidBody(SceneGeometryNode& node)
{

}

int My::PhysXPhysicsManager::CreateRigidBodies()
{

}

void My::PhysXPhysicsManager::ClearRigidBodies()
{

}

My::Matrix4X4f My::PhysXPhysicsManager::GetRigidBodyTransform(void* rigidBody)
{

}