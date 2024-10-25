#pragma once

#include "IRuntimeModule.hpp"
#include "SceneNodeGeometry.hpp"
#include "SceneObjectGeometry.hpp"
#include "geommath.hpp"

namespace My {
	__Interface IPhysicsManager : public IRuntimeModule{
	public:
		IPhysicsManager() = default;
		virtual ~IPhysicsManager() = default;

		virtual void CreateRigidBody(SceneGeometryNode& node, const SceneObjectGeometry& geometry) = 0;
		virtual void DeleteRigidBody(SceneGeometryNode& node) = 0;

		virtual int CreateRigidBodies() = 0;
		virtual void ClearRigidBodies() = 0;

		virtual Matrix4X4f GetRigidBodyTransform(void* rigidbody) = 0;

		virtual uint32_t GetTimestamp() = 0;
	};
}