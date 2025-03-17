#pragma once
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include "SceneObjectTransform.hpp"
#include "geommath.hpp"

namespace My {
    class BaseSceneNode {
    protected:
        std::string m_strName;
        std::list<std::shared_ptr<BaseSceneNode>> m_Children;
        std::list<std::shared_ptr<SceneObjectTransform>> m_Transforms;

    protected:
        virtual void dump(std::ostream& out) const {};

    public:
        BaseSceneNode() {};
        BaseSceneNode(const char* name) { m_strName = name; };
        BaseSceneNode(const std::string& name) { m_strName = name; };
        BaseSceneNode(const std::string&& name) { m_strName = name; };
        virtual ~BaseSceneNode() {};

        void AppendChild(std::shared_ptr<BaseSceneNode>&& sub_node)
        {
            m_Children.push_back(std::move(sub_node));
        }

        void AppendChild(std::shared_ptr<SceneObjectTransform>&& transform)
        {
            m_Transforms.push_back(std::move(transform));
        }

        const std::shared_ptr<My::Matrix4X4f> GetCalculatedTransform() const
        {
            std::shared_ptr<Matrix4X4f> result(new Matrix4X4f());
            BuildIdentityMatrix(*result);

            for (auto trans : m_Transforms)
            {
                *result = *result * static_cast<Matrix4X4f>(*trans);
            }

            return result;
        }

        friend std::ostream& operator<<(std::ostream& out, const BaseSceneNode& node)
        {
            static thread_local int32_t indent = 0;
            indent++;

            out << std::string(indent, ' ') << "Scene Node" << std::endl;
            out << std::string(indent, ' ') << "----------" << std::endl;
            out << std::string(indent, ' ') << "Name: " << node.m_strName << std::endl;
            node.dump(out);
            out << std::endl;

            for (const std::shared_ptr<BaseSceneNode>& sub_node : node.m_Children) {
                out << *sub_node << std::endl;
            }

            for (const std::shared_ptr<SceneObjectTransform>& sub_node : node.m_Transforms) {
                out << *sub_node << std::endl;
            }

            indent--;

            return out;
        }
    };

    template <typename T>
    class SceneNode : public BaseSceneNode {
    protected:
        std::string m_keySceneObject;

    protected:
        virtual void dump(std::ostream& out) const
        {
            out << m_keySceneObject << std::endl;
        };

    public:
        using BaseSceneNode::BaseSceneNode;
        SceneNode() = default;

        void AddSceneObjectRef(const std::string& key) { m_keySceneObject = key; };

        const std::string& GetSceneObjectRef() { return m_keySceneObject; }
    };

    typedef BaseSceneNode SceneEmptyNode;
}