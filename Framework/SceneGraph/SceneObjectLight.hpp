#pragma once

#include "BaseSceneObject.hpp"
#include "SceneObjectTypeDef.hpp"
#include "ParameterValuemap.hpp"
#include "geommath.hpp"
#include "shader_base.h"
#include <string>
#include <ostream>

namespace My {
    typedef float (*AttenFunc)(float /* Intensity */, float /* Distance */);

    float DefaultAttenFunc(float intensity, float distance);

    class SceneObjectLight : public BaseSceneObject
    {
    protected:
        Color       m_LightColor;
        float       m_fIntensity;
        AttenFunc   m_LightAttenuation;
        bool        m_bCastShadows;
        std::string m_strTexture;
        LightType   m_LightType;

    public:
        void SetIfCastShadow(bool shadow) { m_bCastShadows = shadow; };

        void SetColor(std::string& attrib, Vector4f& color)
        {
            if (attrib == "light") {
                m_LightColor = Color(color);
            }
        };

        void SetParam(std::string& attrib, float param)
        {
            if (attrib == "intensity") {
                m_fIntensity = param;
            }
            if (attrib == "lightType") {
                m_LightType = (My::LightType)param;
            }
        };

        void SetTexture(std::string& attrib, std::string& textureName)
        {
            if (attrib == "projection") {
                m_strTexture = textureName;
            }
        };

        void SetAttenuation(AttenFunc func)
        {
            m_LightAttenuation = func;
        }

        const Color& GetColor() { return m_LightColor; };
        float* GetIntensity() { return &m_fIntensity; };
        LightType GetLightType() { return m_LightType; };
        bool* GetIfCastShadow() { return &m_bCastShadows; };
    protected:
        // can only be used as base class of delivered lighting objects
        SceneObjectLight(void) : BaseSceneObject(SceneObjectType::kSceneObjectTypeLight), m_LightColor(Vector4f(1.0f)), m_fIntensity(100.0f), m_LightAttenuation(DefaultAttenFunc), m_bCastShadows(false) {};

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectLight& obj);
    };

    class SceneObjectOmniLight : public SceneObjectLight
    {
    public:
        using SceneObjectLight::SceneObjectLight;

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectOmniLight& obj);
    };

    class SceneObjectSpotLight : public SceneObjectLight
    {
    protected:
        float   m_fConeAngle;
        float   m_fPenumbraAngle;
    public:
        SceneObjectSpotLight(void) : SceneObjectLight(), m_fConeAngle(PI / 4.0f), m_fPenumbraAngle(PI / 3.0f) {};

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectSpotLight& obj);
    };

    class SceneObjectInfiniteLight : public SceneObjectLight
    {
    public:
        using SceneObjectLight::SceneObjectLight;

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectInfiniteLight& obj);
    };
}