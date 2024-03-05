#pragma once

#include "BaseSceneNode.hpp"
#include <ostream>
#include <vector>
#include <string>


namespace My {
    class SceneLightNode : public SceneNode<SceneObjectLight>
    {
    protected:
        bool        m_bShadow;

    public:
        using SceneNode::SceneNode;

        void SetIfCastShadow(bool shadow) { m_bShadow = shadow; };
        const bool CastShadow() { return m_bShadow; };
    };
}