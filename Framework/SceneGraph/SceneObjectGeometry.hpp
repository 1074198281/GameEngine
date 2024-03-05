#pragma once

#include "BaseSceneObject.hpp"
#include "SceneObjectTypeDef.hpp"
#include "SceneObjectMesh.hpp"
#include <string>
#include <fstream>

namespace My {
    class SceneObjectGeometry : public BaseSceneObject
    {
    protected:
        std::vector<std::shared_ptr<SceneObjectMesh>> m_Mesh;
        bool        m_bVisible;
        bool        m_bShadow;
        bool        m_bMotionBlur;
        SceneObjectCollisionType m_CollisionType;

    public:
        SceneObjectGeometry(void) : BaseSceneObject(SceneObjectType::kSceneObjectTypeGeometry), m_CollisionType(SceneObjectCollisionType::kSceneObjectCollisionTypeNone) {};

        void SetVisibility(bool visible) { m_bVisible = visible; };
        const bool Visible() { return m_bVisible; };
        void SetIfCastShadow(bool shadow) { m_bShadow = shadow; };
        const bool CastShadow() { return m_bShadow; };
        void SetIfMotionBlur(bool motion_blur) { m_bMotionBlur = motion_blur; };
        const bool MotionBlur() { return m_bMotionBlur; };
        void SetCollisionType(SceneObjectCollisionType collision_type) { m_CollisionType = collision_type; };
        const SceneObjectCollisionType CollisionType() const { return  m_CollisionType; }

        void AddMesh(std::shared_ptr<SceneObjectMesh>& mesh) { m_Mesh.push_back(std::move(mesh)); };
        const std::weak_ptr<SceneObjectMesh> GetMesh() { return (m_Mesh.empty() ? nullptr : m_Mesh[0]); };
        const std::weak_ptr<SceneObjectMesh> GetMeshLOD(size_t lod) { return (lod < m_Mesh.size() ? m_Mesh[lod] : nullptr); };

        operator std::string() { return m_Guid; }
        friend std::ostream& operator<<(std::ostream& out, const SceneObjectGeometry& obj);
    };
}