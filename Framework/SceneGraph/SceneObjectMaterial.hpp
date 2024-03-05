#pragma once

#include "BaseSceneObject.hpp"
#include "SceneObjectTypeDef.hpp"
#include "ParameterValuemap.hpp"
#include <string>
#include <ostream>

namespace My {
    class SceneObjectMaterial : public BaseSceneObject
    {
    public:
    enum TextureType { kBaseColor, kMetallicRoughness, kOcclusion, kEmissive, kNormal, kpbrType };
    protected:
        std::string m_Name;
        Color       m_BaseColor;
        Parameter   m_Metallic;
        Parameter   m_Roughness;
        Normal      m_Normal;
        Parameter   m_NormalScale;
        Parameter   m_Specular;
        Parameter   m_AmbientOcclusion;
        Color       m_Opacity;
        Color       m_Transparency;
        Color       m_Emission;

        //pbr
        Color       m_pbrBaseColor;
        Color       m_pbrMetallicRoughness;
        Color       m_pbrOcclusion;
        Color       m_pbrEmissive;
        Color       m_pbrNormal;
        int         m_TextureTypeFlag;

    public:
        SceneObjectMaterial(const std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(name), m_TextureTypeFlag(0) {};
        SceneObjectMaterial(std::string&& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(std::move(name)), m_TextureTypeFlag(0) {};
        SceneObjectMaterial(void) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(""), m_BaseColor(Vector4f(1.0f)), m_Metallic(0.0f), m_Roughness(0.0f), m_Normal(Vector3f(0.0f, 0.0f, 1.0f)), m_Specular(0.0f), m_AmbientOcclusion(1.0f), m_TextureTypeFlag(0) {};
        int GetTextureTypeFlag() { return m_TextureTypeFlag; };
        void SetName(const std::string& name) { m_Name = name; };
        void SetName(std::string&& name) { m_Name = std::move(name); };
        std::string GetName() { return m_Name; };
        void SetColor(std::string& attrib, Vector4f& color)
        {
            if (attrib == "diffuse") {
                m_BaseColor = Color(color);
            }
            else if (attrib == "Opacity") {
                m_Opacity = Color(color);
            }
            else if (attrib == "transparency") {
                m_Transparency = Color(color);
            }
            else if (attrib == "emission") {
                m_Emission = Color(color);
            }
        };

        void SetParam(std::string& attrib, float param)
        {
            if (attrib == "metallic") {
                m_Metallic = Parameter(param);
            }
            else if (attrib == "roughness") {
                m_Roughness = Parameter(param);
            }
            else if (attrib == "specular") {
                m_Specular = Parameter(param);
            }
            else if (attrib == "ambient") {
                m_AmbientOcclusion = Parameter(param);
            }
            else if (attrib == "normal") {
                m_NormalScale = Parameter(param);
            }
        };

        void SetTexture(std::string& attrib, std::string& textureName)
        {
            if (attrib == "pbrdiffuse") {
                m_pbrBaseColor = std::make_shared<SceneObjectTexture>(textureName);
                m_TextureTypeFlag |= (1 << kBaseColor);
            }
            else if (attrib == "pbrmetallicroughness") {
                m_pbrMetallicRoughness = std::make_shared<SceneObjectTexture>(textureName);
                m_TextureTypeFlag |= (1 << kMetallicRoughness);
            }
            else if (attrib == "pbrocclusion") {
                m_pbrOcclusion = std::make_shared<SceneObjectTexture>(textureName);
                m_TextureTypeFlag |= (1 << kOcclusion);
            }
            else if (attrib == "pbremissive") {
                m_pbrEmissive = std::make_shared<SceneObjectTexture>(textureName);
                m_TextureTypeFlag |= (1 << kEmissive);
            }
            else if (attrib == "pbrnormal") {
                m_pbrNormal = std::make_shared<SceneObjectTexture>(textureName);
                m_TextureTypeFlag |= (1 << kNormal);
            }
        };

        void SetTexture(std::string& attrib, std::shared_ptr<SceneObjectTexture>& texture)
        {
            if (attrib == "pbrdiffuse") {
                m_pbrBaseColor = texture;
                m_TextureTypeFlag |= (1 << kBaseColor);
            }
            else if (attrib == "pbrmetallicroughness") {
                m_pbrMetallicRoughness = texture;
                m_TextureTypeFlag |= (1 << kMetallicRoughness);
            }
            else if (attrib == "pbrocclusion") {
                m_pbrOcclusion = texture;
                m_TextureTypeFlag |= (1 << kOcclusion);
            }
            else if (attrib == "pbremissive") {
                m_pbrEmissive = texture;
                m_TextureTypeFlag |= (1 << kEmissive);
            }
            else if (attrib == "pbrnormal") {
                m_pbrNormal = texture;
                m_TextureTypeFlag |= (1 << kNormal);
            }
        };

        void SetSampler(std::string& attrib, int filter, int wrapS, int wrapT)
        {
            if (attrib == "pbrdiffuse") {
                m_pbrBaseColor.ValueMap->SetSampler(filter, wrapS, wrapT);
            }
            else if (attrib == "pbrmetallicroughness") {
                m_pbrMetallicRoughness.ValueMap->SetSampler(filter, wrapS, wrapT);
            }
            else if (attrib == "pbrocclusion") {
                m_pbrOcclusion.ValueMap->SetSampler(filter, wrapS, wrapT);
            }
            else if (attrib == "pbremissive") {
                m_pbrEmissive.ValueMap->SetSampler(filter, wrapS, wrapT);
            }
            else if (attrib == "pbrnormal") {
                m_pbrNormal.ValueMap->SetSampler(filter, wrapS, wrapT);
            }
        };

        void LoadTextures() {
            if (TEST_BIT(m_TextureTypeFlag, kBaseColor)) {
                m_pbrBaseColor.ValueMap->LoadTexture();
            }
            if (TEST_BIT(m_TextureTypeFlag, kMetallicRoughness)) {
                m_pbrMetallicRoughness.ValueMap->LoadTexture();
            }
            if (TEST_BIT(m_TextureTypeFlag, kOcclusion)) {
                m_pbrOcclusion.ValueMap->LoadTexture();
            }
            if (TEST_BIT(m_TextureTypeFlag, kEmissive)) {
                m_pbrEmissive.ValueMap->LoadTexture();
            }
            if (TEST_BIT(m_TextureTypeFlag, kNormal)) {
                m_pbrNormal.ValueMap->LoadTexture();
            }
        }

        const std::shared_ptr<SceneObjectTexture> GetTexture(TextureType type) const
        {
            if (type == kBaseColor) {
                return m_pbrBaseColor.ValueMap;
            }
            else if (type == kMetallicRoughness) {
                return m_pbrMetallicRoughness.ValueMap;
            }
            else if (type == kOcclusion) {
                return m_pbrOcclusion.ValueMap;
            }
            else if (type == kEmissive) {
                return m_pbrEmissive.ValueMap;
            }
            else if (type == kNormal) {
                return m_pbrNormal.ValueMap;
            }
            return nullptr;
        }

        const Vector4f GetBaseColorFactor() const
        {
            return m_BaseColor.Value;
        }

        const float GetMetallicFactor() const
        {
            return m_Metallic.Value;
        }

        const float GetRoughnessFactor() const
        {
            return m_Roughness.Value;
        }

        const Vector4f GetEmissivsFactor() const
        {
            return m_Emission.Value;
        }

        const float GetNormalScaleFactor() const
        {
            return m_NormalScale.Value;
        }

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectMaterial& obj);
    };
}