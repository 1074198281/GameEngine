#pragma once

#include "BaseSceneObject.hpp"
#include "SceneObjectTypeDef.hpp"
#include "geommath.hpp"
#include <string>
#include <fstream>

namespace My {
    class SceneObjectCamera : public BaseSceneObject
    {
    protected:
        float m_fAspect;
        float m_fNearClipDistance;
        float m_fFarClipDistance;

    public:
        void SetColor(std::string& attrib, Vector4f& color)
        {
            // TODO: extension
        };

        void SetParam(std::string& attrib, float param)
        {
            if (attrib == "near") {
                m_fNearClipDistance = param;
            }
            else if (attrib == "far") {
                m_fFarClipDistance = param;
            }
            else if (attrib == "aspect") {
                m_fAspect = param;
            }
        };

        void SetTexture(std::string& attrib, std::string& textureName)
        {
            // TODO: extension
        };

    protected:
        // can only be used as base class
        SceneObjectCamera(void) : BaseSceneObject(SceneObjectType::kSceneObjectTypeCamera), m_fAspect(16.0f / 9.0f), m_fNearClipDistance(1.0f), m_fFarClipDistance(100.0f) {};

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectCamera& obj);
    };

    class SceneObjectOrthogonalCamera : public SceneObjectCamera
    {
    public:
        using SceneObjectCamera::SceneObjectCamera;

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectOrthogonalCamera& obj);
    };

    class SceneObjectPerspectiveCamera : public SceneObjectCamera
    {
    protected:
        float m_fFov;

    public:
        void SetParam(std::string& attrib, float param)
        {
            // TODO: handle fovx, fovy
            if (attrib == "fov") {
                m_fFov = param;
            }
        };

    public:
        SceneObjectPerspectiveCamera(float fov = PI / 2.0) : SceneObjectCamera(), m_fFov(fov) {};

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectPerspectiveCamera& obj);
    };
}