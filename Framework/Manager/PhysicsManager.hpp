#pragma once 

#define BT_USE_DOUBLE_PRECISION 1
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "IPhysicsManager.hpp"
#include "SceneManager.hpp"
#include <vector>

namespace My {
	class PhysicsManager : __implements IPhysicsManager {
	public:
		~PhysicsManager() override = default;
		virtual int Initialize();
		virtual void Finalize();
		virtual void Tick();

		virtual void CreateRigidBody(SceneGeometryNode& node, const SceneObjectGeometry& geometry);
		virtual void DeleteRigidBody(SceneGeometryNode& node);

		virtual int CreateRigidBodies();
		virtual void ClearRigidBodies();

		Matrix4X4f GetRigidBodyTransform(void* rigidBody);

	protected:
		uint64_t m_nSceneRevision;

		btBroadphaseInterface* m_btBroadphase;
		btDefaultCollisionConfiguration* m_btCollisionConfiguration;
		btCollisionDispatcher* m_btDispatcher;
		btSequentialImpulseConstraintSolver* m_btSolver;
		btDiscreteDynamicsWorld* m_btDynamicsWorld;

		std::vector<btCollisionShape*> m_btCollisionShapes;
	};
}