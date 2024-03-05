#pragma once

#include "BaseSceneObject.hpp"
#include "SceneObjectTypeDef.hpp"
#include "SceneObjectIndexArray.hpp"
#include "SceneObjectVertexArray.hpp"
#include <string>
#include <ostream>

namespace My {
    class SceneObjectMesh : public BaseSceneObject
    {
    protected:
        std::vector<SceneObjectIndexArray>  m_IndexArray;
        std::vector<SceneObjectVertexArray> m_VertexArray;
        PrimitiveType	m_PrimitiveType;

        bool        m_bVisible;
        bool        m_bShadow;
        bool        m_bMotionBlur;

    public:
        SceneObjectMesh(bool visible = true, bool shadow = true, bool motion_blur = true) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMesh), m_bVisible(visible), m_bShadow(shadow), m_bMotionBlur(motion_blur) {};
        SceneObjectMesh(SceneObjectMesh&& mesh)
            : BaseSceneObject(SceneObjectType::kSceneObjectTypeMesh),
            m_IndexArray(std::move(mesh.m_IndexArray)),
            m_VertexArray(std::move(mesh.m_VertexArray)),
            m_PrimitiveType(mesh.m_PrimitiveType),
            m_bVisible(mesh.m_bVisible),
            m_bShadow(mesh.m_bShadow),
            m_bMotionBlur(mesh.m_bMotionBlur)
        {
        };
        void AddIndexArray(SceneObjectIndexArray&& array) { m_IndexArray.push_back(std::move(array)); };
        void AddVertexArray(SceneObjectVertexArray&& array) { m_VertexArray.push_back(std::move(array)); };
        void SetPrimitiveType(PrimitiveType type) { m_PrimitiveType = type; };

        size_t GetIndexGroupCount() const { return m_IndexArray.size(); }
        size_t GetIndexCount(const size_t index) const { return (m_IndexArray.empty() ? 0 : m_IndexArray[index].GetIndexCount()); };
        size_t GetVertexCount() const { return (m_VertexArray.empty() ? 0 : m_VertexArray[0].GetVertexCount()); };
        size_t GetVertexPropertiesCount() const { return m_VertexArray.size(); };
        const SceneObjectVertexArray& GetVertexPropertyArray(const size_t index) const { return m_VertexArray[index]; };
        const SceneObjectIndexArray& GetIndexArray(const size_t index) const { return m_IndexArray[index]; };
        const PrimitiveType& GetPrimitiveType() { return m_PrimitiveType; };

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectMesh& obj);
    };
}