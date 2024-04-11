#pragma once 

#include "IPhysicsManager.hpp"
#include "SceneManager.hpp"
#include <vector>

namespace My {
	class PhysicsManager : __implements IPhysicsManager {
	public:
		~PhysicsManager() override = default;
		int Initialize() override = 0;
		void Finalize() override = 0;
		void Tick() override = 0;

		void CreateRigidBody(SceneGeometryNode& node, const SceneObjectGeometry& geometry) override = 0;
		void DeleteRigidBody(SceneGeometryNode& node) override = 0;

		int CreateRigidBodies() override = 0;
		void ClearRigidBodies() override = 0;

		Matrix4X4f GetRigidBodyTransform(void* rigidBody) override = 0;

	protected:
		uint64_t m_nSceneRevision;
	};
}