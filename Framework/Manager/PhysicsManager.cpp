#include <iostream>
#include "PhysicsManager.hpp"
#include "BaseApplication.hpp"

int My::PhysicsManager::Initialize()
{
    m_nSceneRevision = 0;

	//build broadphase
	m_btBroadphase = new btDbvtBroadphase();

	//set up collision configuration and dispatcher
	m_btCollisionConfiguration = new btDefaultCollisionConfiguration();
	m_btDispatcher = new btCollisionDispatcher(m_btCollisionConfiguration);
	
	//the actual physics solver
	m_btSolver = new btSequentialImpulseConstraintSolver;

	//world
	m_btDynamicsWorld = new btDiscreteDynamicsWorld(m_btDispatcher, m_btBroadphase, m_btSolver, m_btCollisionConfiguration);
	m_btDynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));
	
	return 0;
}

void My::PhysicsManager::Finalize()
{
	// Clean up
	ClearRigidBodies();

	delete m_btDynamicsWorld;
	delete m_btSolver;
	delete m_btDispatcher;
	delete m_btCollisionConfiguration;
	delete m_btBroadphase;
}

void My::PhysicsManager::Tick()
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

	m_btDynamicsWorld->stepSimulation(1.0f / 60.0f, 10);
}

void My::PhysicsManager::CreateRigidBody(My::SceneGeometryNode& node, const My::SceneObjectGeometry& geometry)
{
    btRigidBody* rigidBody = nullptr;
    Vector3f Bounding = geometry.Bounding();
    switch (geometry.CollisionType())
    {
    case SceneObjectCollisionType::kSceneObjectCollisionTypeSphere:
    {
        btSphereShape* sphere = new btSphereShape(Bounding[0]);
        m_btCollisionShapes.push_back(sphere);
        const auto trans = node.GetCalculatedTransform();
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(trans->data[0][3], trans->data[1][3], trans->data[2][3]));
        btDefaultMotionState* motionState =
            new btDefaultMotionState(
                startTransform
            );
        btScalar mass = pow(Bounding[0], 3) * 4 * PI / 3.0;
        btVector3 fallInertia(0.0f, 0.0f, 0.0f);
        sphere->calculateLocalInertia(mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo
            rigidBodyCI(mass, motionState, sphere, fallInertia);
        rigidBody = new btRigidBody(rigidBodyCI);
        m_btDynamicsWorld->addRigidBody(rigidBody);
    }
    break;
    case SceneObjectCollisionType::kSceneObjectCollisionTypeBox:
    {
        btBoxShape* box = new btBoxShape(btVector3(Bounding[0], Bounding[2], Bounding[1]));
        m_btCollisionShapes.push_back(box);
        const auto trans = node.GetCalculatedTransform();
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(trans->data[0][3], trans->data[1][3], trans->data[2][3]));
        btDefaultMotionState* motionState =
            new btDefaultMotionState(
                startTransform
            );
        btScalar mass = 0.0f;
        btRigidBody::btRigidBodyConstructionInfo
            rigidBodyCI(mass, motionState, box, btVector3(0.0f, 0.0f, 0.0f));
        rigidBody = new btRigidBody(rigidBodyCI);
        m_btDynamicsWorld->addRigidBody(rigidBody);
    }
    break;
    case SceneObjectCollisionType::kSceneObjectCollisionTypePlane:
    {
        btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(Bounding[0], Bounding[1], Bounding[2]), 0);
        m_btCollisionShapes.push_back(plane);
        const auto trans = node.GetCalculatedTransform();
        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(btVector3(trans->data[0][3], trans->data[1][3], trans->data[2][3]));
        btDefaultMotionState* motionState =
            new btDefaultMotionState(
                startTransform
            );
        btScalar mass = 0.0f;
        btRigidBody::btRigidBodyConstructionInfo
            rigidBodyCI(mass, motionState, plane, btVector3(0.0f, 0.0f, 0.0f));
        rigidBody = new btRigidBody(rigidBodyCI);
        m_btDynamicsWorld->addRigidBody(rigidBody);
    }
    break;
    default:
        ;
    }

    node.LinkRigidBody(rigidBody);
}

void My::PhysicsManager::DeleteRigidBody(My::SceneGeometryNode& node)
{
    if (!node.UnlinkRigidBody()) {
        return;
    }
    btRigidBody* rigidBody = reinterpret_cast<btRigidBody*>(node.UnlinkRigidBody());
    if (rigidBody) {
        m_btDynamicsWorld->removeRigidBody(rigidBody);
        if (auto motionState = rigidBody->getMotionState())
            delete motionState;
        delete rigidBody;
        //m_btDynamicsWorld->removeCollisionObject(rigidBody);
    }
}

int My::PhysicsManager::CreateRigidBodies()
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

void My::PhysicsManager::ClearRigidBodies()
{
    auto pSceneManager = dynamic_cast<BaseApplication*>(m_pApp)->GetSceneManager();

    auto& scene = pSceneManager->GetSceneForRendering();

    // Geometries
    for (auto _it : scene.GeometryNodes)
    {
        auto pGeometryNode = _it.second;
        DeleteRigidBody(*pGeometryNode);
    }

    for (auto shape : m_btCollisionShapes)
    {
        delete shape;
    }

    m_btCollisionShapes.clear();
}

My::Matrix4X4f My::PhysicsManager::GetRigidBodyTransform(void* rigidBody)
{
    Matrix4X4f result;
    btTransform trans;
    reinterpret_cast<btRigidBody*>(rigidBody)->getMotionState()->getWorldTransform(trans);
    auto basis = trans.getBasis();
    auto origin = trans.getOrigin();
    BuildIdentityMatrix(result);
    // 24.4.8 convert result from vec*matrix to matrix*vec 
    result.data[0][0] = basis[0][0];
    result.data[1][0] = basis[0][1];
    result.data[2][0] = basis[0][2];
    result.data[0][1] = basis[1][0];
    result.data[1][1] = basis[1][1];
    result.data[2][1] = basis[1][2];
    result.data[0][2] = basis[2][0];
    result.data[1][2] = basis[2][1];
    result.data[2][2] = basis[2][2];
    result.data[0][3] = origin.getX();
    result.data[1][3] = origin.getY();
    result.data[2][3] = origin.getZ();

    return result;
}