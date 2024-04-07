#pragma once

#include "BaseSceneObject.hpp"
#include "SceneObjectTypeDef.hpp"
#include "ParameterValuemap.hpp"
#include "SceneGraphSettings.h"
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
        Normal      m_Normal;
        Parameter   m_Metallic;
        Parameter   m_Roughness;
        Parameter   m_Specular;
        Parameter   m_AmbientOcclusion;
        Color       m_Opacity;
        Color       m_Transparency;
        Color       m_Emission;

        // gltf pbr
        Color       m_pbrBaseColor;
        Color       m_pbrMetallicRoughness;
        Color       m_pbrOcclusion;
        Color       m_pbrEmissive;
        Color       m_pbrNormal;

        TextureParamFactor m_ParamFactor;
        TextureTransform m_BaseColorTrans;
        TextureTransform m_MetallicRoughnessTrans;
        TextureTransform m_OcclusionTrans;
        TextureTransform m_EmissiveTrans;
        TextureTransform m_NormalTrans;

    public:
        SceneObjectMaterial(const std::string& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(name) {};
        SceneObjectMaterial(std::string&& name) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(std::move(name)) {};
        SceneObjectMaterial(void) : BaseSceneObject(SceneObjectType::kSceneObjectTypeMaterial), m_Name(""), m_BaseColor(Vector4f(1.0f)), m_Normal(Vector3f(0.0f, 0.0f, 1.0f)), m_Specular(0.0f), m_AmbientOcclusion(1.0f) {};
        void SetName(const std::string& name) { m_Name = name; };
        void SetName(std::string&& name) { m_Name = std::move(name); };
        std::string GetName() { return m_Name; };
        void SetColor(std::string& attrib, Vector4f& color)
        {
            if (attrib == "diffuse") {
                m_BaseColor = Color(color);
                m_ParamFactor.BaseColorFactor[0] = color[0];
                m_ParamFactor.BaseColorFactor[1] = color[1];
                m_ParamFactor.BaseColorFactor[2] = color[2];
                m_ParamFactor.BaseColorFactor[3] = color[3];
            }
            else if (attrib == "Opacity") {
                m_Opacity = Color(color);
            }
            else if (attrib == "transparency") {
                m_Transparency = Color(color);
            }
            else if (attrib == "emission") {
                m_Emission = Color(color);
                m_ParamFactor.EmissiveFactor[0] = color[0];
                m_ParamFactor.EmissiveFactor[1] = color[1];
                m_ParamFactor.EmissiveFactor[2] = color[2];
            }
        };

        void SetParam(std::string& attrib, float param)
        {
            if (attrib == "metallic") {
                m_ParamFactor.MetallicRoughnessFactor[0] = param;
            }
            else if (attrib == "roughness") {
                m_ParamFactor.MetallicRoughnessFactor[1] = param;
            }
            else if (attrib == "specular") {
                m_Specular = Parameter(param);
            }
            else if (attrib == "ambient") {
                m_AmbientOcclusion = Parameter(param);
            }
            else if (attrib == "normal") {
                m_ParamFactor.NormalTextureScaleFactor = param;
            }
        };

        void SetTexture(std::string& attrib, std::string& textureName)
        {
            if (attrib == "pbrdiffuse") {
                m_pbrBaseColor = std::make_shared<SceneObjectTexture>(textureName);
            }
            else if (attrib == "pbrmetallicroughness") {
                m_pbrMetallicRoughness = std::make_shared<SceneObjectTexture>(textureName);
            }
            else if (attrib == "pbrocclusion") {
                m_pbrOcclusion = std::make_shared<SceneObjectTexture>(textureName);
            }
            else if (attrib == "pbremissive") {
                m_pbrEmissive = std::make_shared<SceneObjectTexture>(textureName);
            }
            else if (attrib == "pbrnormal") {
                m_pbrNormal = std::make_shared<SceneObjectTexture>(textureName);
            }
        };

        void SetTexture(std::string& attrib, std::shared_ptr<SceneObjectTexture>& texture)
        {
            if (attrib == "pbrdiffuse") {
                m_pbrBaseColor = texture;
            }
            else if (attrib == "pbrmetallicroughness") {
                m_pbrMetallicRoughness = texture;
            }
            else if (attrib == "pbrocclusion") {
                m_pbrOcclusion = texture;
            }
            else if (attrib == "pbremissive") {
                m_pbrEmissive = texture;
            }
            else if (attrib == "pbrnormal") {
                m_pbrNormal = texture;
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
            if (m_pbrBaseColor.ValueMap) {
                m_pbrBaseColor.ValueMap->LoadTexture();
            }
            if (m_pbrMetallicRoughness.ValueMap) {
                m_pbrMetallicRoughness.ValueMap->LoadTexture();
            }
            if (m_pbrOcclusion.ValueMap) {
                m_pbrOcclusion.ValueMap->LoadTexture();
            }
            if (m_pbrEmissive.ValueMap) {
                m_pbrEmissive.ValueMap->LoadTexture();
            }
            if (m_pbrNormal.ValueMap) {
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
            return Vector4f(m_ParamFactor.BaseColorFactor[0], m_ParamFactor.BaseColorFactor[1],
                m_ParamFactor.BaseColorFactor[2], m_ParamFactor.BaseColorFactor[3]);
        }

        float* GetBaseColorFactorData()
        {
            return m_ParamFactor.BaseColorFactor;
        }

        const float GetMetallicFactor() const
        {
            return m_ParamFactor.MetallicRoughnessFactor[0];
        }

        float* GetMetallicFactorData()
        {
            return &m_ParamFactor.MetallicRoughnessFactor[0];
        }

        const float GetRoughnessFactor() const
        {
            return m_ParamFactor.MetallicRoughnessFactor[1];
        }

        float* GetRoughnessFactorData()
        {
            return &m_ParamFactor.MetallicRoughnessFactor[1];
        }

        const Vector4f GetEmissivsFactor() const
        {
            return Vector4f(m_ParamFactor.EmissiveFactor[0], m_ParamFactor.EmissiveFactor[1],
                m_ParamFactor.EmissiveFactor[2], 0.0);
        }

        float* GetEmissiveFactorData()
        {
            return m_ParamFactor.EmissiveFactor;
        }

        const float GetNormalScaleFactor() const
        {
            return m_ParamFactor.NormalTextureScaleFactor;
        }

        float* GetNornalScaleFactorData()
        {
            return &m_ParamFactor.NormalTextureScaleFactor;
        }

        void SetTextureTransform(const std::string attrib, TextureTransform trans) 
        {
            if (attrib == "pbrdiffuse") {
                m_BaseColorTrans = trans;
            }
            else if (attrib == "pbrmetallicroughness") {
                m_MetallicRoughnessTrans = trans;
            }
            else if (attrib == "pbrocclusion") {
                m_OcclusionTrans = trans;
            }
            else if (attrib == "pbremissive") {
                m_EmissiveTrans = trans;
            }
            else if (attrib == "pbrnormal") {
                m_NormalTrans = trans;
            }
        }

        const TextureTransform GetTextureTransform(TextureType type) const
        {
            TextureTransform trans;
            switch (type)
            {
            case My::SceneObjectMaterial::kBaseColor:
                trans = m_BaseColorTrans;
                break;
            case My::SceneObjectMaterial::kMetallicRoughness:
                trans = m_MetallicRoughnessTrans;
                break;
            case My::SceneObjectMaterial::kOcclusion:
                trans = m_OcclusionTrans;
                break;
            case My::SceneObjectMaterial::kEmissive:
                trans = m_EmissiveTrans;
                break;
            case My::SceneObjectMaterial::kNormal:
                trans = m_NormalTrans;
                break;
            default:
                assert(false);
                break;
            }
            return trans;
        }

        friend std::ostream& operator<<(std::ostream& out, const SceneObjectMaterial& obj);
    };
}