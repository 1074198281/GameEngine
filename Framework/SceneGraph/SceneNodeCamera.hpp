#pragma once

#include "BaseSceneNode.hpp"
#include <ostream>
#include <vector>
#include <string>

namespace My {
    class SceneCameraNode : public SceneNode<SceneObjectCamera>
    {
    protected:
        Vector3f m_Target;

    public:
        using SceneNode::SceneNode;

        void SetTarget(Vector3f& target) { m_Target = target; };
        const Vector3f& GetTarget() { return m_Target; };
    };
}