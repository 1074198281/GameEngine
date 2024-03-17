#pragma once

#include "IRuntimeModule.hpp"
#include "SceneNodeGeometry.hpp"
#include "SceneObjectGeometry.hpp"

namespace My {
	__Interface IPhysicsManager : public IRuntimeModule{
	public:
		IPhysicsManager() = default;
		virtual ~IPhysicsManager() = default;

		virtual void CreateRigidBody(SceneGeometryNode& node, const SceneObjectGeometry& geometry) = 0;
		virtual void DeleteRigidBody(SceneGeometryNode& node) = 0;

		virtual int CreateRigidBodies() = 0;
		virtual void ClearRigidBodies() = 0;
	};
}