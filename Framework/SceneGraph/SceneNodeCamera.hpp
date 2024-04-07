#pragma once

#include "BaseSceneNode.hpp"
#include "Quaternion.h"
#include <ostream>
#include <vector>
#include <string>

namespace My {
    class SceneCameraNode : public SceneNode<SceneObjectCamera>
    {
    protected:
        Vector3f m_Target;
        Vector3f m_Translation;
        Quaternion m_Rotation;
        Vector3f m_Scale;
    public:
        using SceneNode::SceneNode;

        void SetTarget(Vector3f& target) { m_Target = target; };
        const Vector3f& GetTarget() { return m_Target; };
        void SetTranslation(Vector3f translation) { m_Translation = translation; }
        const Vector3f& Translation() { return m_Translation; }
        void SetRotation(Quaternion q) { m_Rotation = q; }
        const Quaternion& GetRotation() { return m_Rotation; }
        void SetScale(Vector3f scale) { m_Scale = scale; }
        const Vector3f& GetScale() { return m_Scale; }
    };
}