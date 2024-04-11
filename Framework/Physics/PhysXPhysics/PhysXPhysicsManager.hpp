#pragma once

#include "PhysicsManager.hpp"
#include "PxPhysicsAPI.h"

namespace My {
	class PhysXPhysicsManager : __implements PhysicsManager
	{
	public:
		~PhysXPhysicsManager() = default;
		int Initialize() override;
		void Finalize() override;
		void Tick() override;

		void CreateRigidBody(SceneGeometryNode& node, const SceneObjectGeometry& geometry) override;
		void DeleteRigidBody(SceneGeometryNode& node) override;

		int CreateRigidBodies() override;
		void ClearRigidBodies() override;

		Matrix4X4f GetRigidBodyTransform(void* rigidBody) override;
	protected:
		physx::PxDefaultErrorCallback m_DefaultErrorCallBack;
		physx::PxDefaultAllocator m_DefaultAllocatorCallBack;
		physx::PxFoundation* m_PxFoundation;

		physx::PxPvd* m_PxPvd;
		physx::PxPhysics* m_PxPhysics;
		physx::PxScene* m_PxScene; 
		physx::PxDefaultCpuDispatcher* m_PxDispatcher;	// cpu task dispatcher for scene
	};

	using TPhysicsManager = PhysXPhysicsManager;
}