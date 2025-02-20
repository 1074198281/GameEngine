#pragma once

#include "BaseSceneNode.hpp"
#include "SceneObjectGeometry.hpp"
#include <ostream>
#include <vector>
#include <string>


namespace My {
    class SceneGeometryNode : public SceneNode<SceneObjectGeometry>
    {
    protected:
        bool        m_bVisible;
        bool        m_bShadow;
        bool        m_bMotionBlur;
        std::vector<std::string> m_Materials;
        void*       m_pRigidBody;
        void*       m_pChemistryBody;

    protected:
        virtual void dump(std::ostream& out) const
        {
            SceneNode::dump(out);
            out << "Visible: " << m_bVisible << std::endl;
            out << "Shadow: " << m_bShadow << std::endl;
            out << "Motion Blur: " << m_bMotionBlur << std::endl;
            out << "Material(s): " << std::endl;
            for (auto material : m_Materials) {
                out << material << std::endl;
            }
        };

    public:
        using SceneNode::SceneNode;

        void SetVisibility(bool visible) { m_bVisible = visible; };
        const bool Visible() { return m_bVisible; };
        void SetIfCastShadow(bool shadow) { m_bShadow = shadow; };
        const bool CastShadow() { return m_bShadow; };
        void SetIfMotionBlur(bool motion_blur) { m_bMotionBlur = motion_blur; };
        const bool MotionBlur() { return m_bMotionBlur; };
        using SceneNode::AddSceneObjectRef;
        void AddMaterialRef(const std::string& key) { m_Materials.push_back(key); };
        void AddMaterialRef(const std::string&& key) { m_Materials.push_back(std::move(key)); };
        size_t GetMaterialCount() { return m_Materials.size(); };
        std::string GetMaterialRef(const size_t index)
        {
            if (index < m_Materials.size()) {
                return m_Materials[index];
            } else {
                return std::string("default");
            }
        }

        void LinkRigidBody(void* rigidBody)
        {
            m_pRigidBody = rigidBody;
        }

        void* UnlinkRigidBody()
        {
            void* rigidBody = m_pRigidBody;
            m_pRigidBody = nullptr;
            return rigidBody;
        }

        void* GetRigidBody()
        {
            return m_pRigidBody;
        }

        void LinkChemistryBody(void* chemistryBody)
        {
            m_pChemistryBody = chemistryBody;
        }

        void* UnLinkChemistryBody()
        {
            void* chemistryBody = m_pChemistryBody;
            m_pChemistryBody = nullptr;
            return chemistryBody;
        }

        void* GetChemistryBody()
        {
            return m_pChemistryBody;
        }
    };
}