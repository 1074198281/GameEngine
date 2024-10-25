#include "PhysXPhysicsManager.hpp"
#include "BaseApplication.hpp"
#include "PxPhysXConfig.h"
#include "PxConfig.h"
#include "Quaternion.hpp"

using namespace physx;

int My::PhysXPhysicsManager::Initialize()
{
	m_nSceneRevision = 0;

	m_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAllocatorCallBack, m_DefaultErrorCallBack);
	if (!m_PxFoundation) {
		assert(false, "PxCreateFoundation Failed!");
	}


	// for PhysX Visual Debugger
	bool recordMemoryAllocators = true;
	m_PxPvd = PxCreatePvd(*m_PxFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10000);
	m_PxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	m_PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_PxFoundation, PxTolerancesScale(), recordMemoryAllocators, m_PxPvd);
	if (!m_PxPhysics) {
		assert(false, "PxCreatePhysics Failed!");
	}

	PxSceneDesc sceneDesc(m_PxPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -0.981f, 0.0f);
	m_PxDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_PxDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.flags = PxSceneFlag::eENABLE_ACTIVE_ACTORS;
	m_PxScene = m_PxPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = m_PxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	return 0;
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
	auto pSceneManager = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager();
	auto pPhysicsManager = dynamic_cast<BaseApplication*>(m_pApp)->GetPhysicsManager();
	auto rev = pSceneManager->GetSceneRevision();
	if (m_nSceneRevision != rev)
	{
		pPhysicsManager->ClearRigidBodies();
		pPhysicsManager->CreateRigidBodies();
		m_nSceneRevision = rev;
	}

	m_PxScene->simulate(1.0f / 60.0f);
	m_PxScene->fetchResults(true);
}

void My::PhysXPhysicsManager::CreateRigidBody(SceneGeometryNode& node, const SceneObjectGeometry& geometry)
{
	PxRigidActor* rigidActor = nullptr;
	switch (geometry.CollisionType())
	{
	case kSceneObjectCollisionTypeSphere:
	{
		PxRigidDynamic* dynamic;
		PxSphereGeometry GeoSphere;
		PxShape* ShapeSphere = nullptr;
		PxMaterial* MaterialSphere = nullptr;
		float Radius = geometry.Bounding()[0];
		const auto trans = node.GetCalculatedTransform();
		dynamic = m_PxPhysics->createRigidDynamic(PxTransform(PxVec3(trans->data[0][3], trans->data[1][3], trans->data[2][3])));
		MaterialSphere = m_PxPhysics->createMaterial(0.5, 0.5, 0.6);
		GeoSphere.radius = Radius;
		PxRigidActorExt::createExclusiveShape(*dynamic, GeoSphere, *MaterialSphere);
		ShapeSphere = m_PxPhysics->createShape(GeoSphere, *MaterialSphere);
		float mass = 4 / 3.0f * PI * pow(Radius, 3);
		PxRigidBodyExt::updateMassAndInertia(*dynamic, mass);
		ShapeSphere->setContactOffset(0.04);
		ShapeSphere->setRestOffset(0.01);
		rigidActor = dynamic;
		rigidActor->attachShape(*ShapeSphere);
		rigidActor->setName(node.GetSceneObjectRef().c_str());
		m_PxScene->addActor(*rigidActor);
	}
	break;
	case kSceneObjectCollisionTypeBox:
	{
		PxRigidStatic* dynamic;
		PxBoxGeometry GeoBox;
		PxShape* ShapeBox = nullptr;
		PxMaterial* MaterialBox = nullptr;
		My::Vector3f Bounding= geometry.Bounding();
		const auto trans = node.GetCalculatedTransform();
		dynamic = m_PxPhysics->createRigidStatic(PxTransform(PxVec3(trans->data[0][3], trans->data[1][3], trans->data[2][3])));
		dynamic->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
		MaterialBox = m_PxPhysics->createMaterial(0.5, 0.5, 0.6);
		GeoBox.halfExtents = PxVec3(Bounding[1], Bounding[2], Bounding[0]);
		PxRigidActorExt::createExclusiveShape(*dynamic, GeoBox, *MaterialBox);
		ShapeBox = m_PxPhysics->createShape(GeoBox, *MaterialBox);
		//float mass = 0;
		float mass = 2 * Bounding[0] * 2 * Bounding[1] * 2 * Bounding[2];
		//PxRigidBodyExt::updateMassAndInertia(*dynamic, mass);
		ShapeBox->setContactOffset(0.04);
		ShapeBox->setRestOffset(0.01);
		rigidActor = dynamic;
		rigidActor->attachShape(*ShapeBox);
		rigidActor->setName(node.GetSceneObjectRef().c_str());
		m_PxScene->addActor(*rigidActor);
	}
	break;
	case kSceneObjectCollisionTypePlane:
	{
		PxPlaneGeometry GeoPlane;
		PxShape* ShapePlane = nullptr;
		PxMaterial* MaterialPlane = nullptr;
		My::Vector3f Bounding = geometry.Bounding();
		const auto trans = node.GetCalculatedTransform();
		rigidActor = m_PxPhysics->createRigidDynamic(PxTransform(PxVec3(trans->data[0][3], trans->data[1][3], trans->data[2][3])));
		MaterialPlane = m_PxPhysics->createMaterial(0.5, 0.5, 0.6);
		PxRigidActorExt::createExclusiveShape(*rigidActor, GeoPlane, *MaterialPlane);
		ShapePlane = m_PxPhysics->createShape(GeoPlane, *MaterialPlane);
		float mass = 0.f;
		reinterpret_cast<PxRigidDynamic*>(rigidActor)->setMass(mass);
		ShapePlane->setContactOffset(0.04);
		ShapePlane->setRestOffset(0.01);
		rigidActor->attachShape(*ShapePlane);
		rigidActor->setName(node.GetSceneObjectRef().c_str());
		m_PxScene->addActor(*rigidActor);
	}
	break;
	default:
		break;
	}

	node.LinkRigidBody(rigidActor);
}

void My::PhysXPhysicsManager::DeleteRigidBody(SceneGeometryNode& node)
{
	if (!node.UnlinkRigidBody()) {
		return;
	}
	PxRigidBody* rigidBody = reinterpret_cast<PxRigidBody*>(node.UnlinkRigidBody());
	if (rigidBody) {
		
	}
}

int My::PhysXPhysicsManager::CreateRigidBodies()
{
	auto pSceneManager = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager();
	auto& scene = pSceneManager->GetSceneForRendering();

	// Geometries
	for (auto _it : scene.GeometryNodes)
	{
		auto pGeometryNode = _it.second;
		auto pGeometry = scene.GetGeometry(pGeometryNode->GetSceneObjectRef());
		assert(pGeometry);

		CreateRigidBody(*pGeometryNode, *pGeometry);
	}

	return 0;
}

void My::PhysXPhysicsManager::ClearRigidBodies()
{
	auto pSceneManager = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager();

	auto& scene = pSceneManager->GetSceneForRendering();

	// Geometries
	for (auto _it : scene.GeometryNodes)
	{
		auto pGeometryNode = _it.second;
		DeleteRigidBody(*pGeometryNode);
	}

}

My::Matrix4X4f My::PhysXPhysicsManager::GetRigidBodyTransform(void* rigidBody)
{
	My::Matrix4X4f result;
	BuildIdentityMatrix(result);

	auto trans = reinterpret_cast<PxRigidActor*>(rigidBody)->getGlobalPose();
	My::Quaternion q(trans.q.x, trans.q.y, trans.q.z, trans.q.w);
	MatrixRotationQuaternion(result, q);
	result[0][3] = trans.p.x;
	result[1][3] = trans.p.y;
	result[2][3] = trans.p.z;
	return result;
}

uint32_t My::PhysXPhysicsManager::GetTimestamp()
{
	return m_PxScene->getTimestamp();
}