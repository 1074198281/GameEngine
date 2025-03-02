#include "LightManager.h"
#include "SceneObjectLight.hpp"
#include "SceneNodeLight.hpp"
#include "geommath.hpp"

My::LightManager::LightManager()
{
	m_pLightInfo = nullptr;
	m_pApp = nullptr;
	m_iLightNum = 0;
}

My::LightManager::~LightManager()
{
	m_LightInfoMap.clear();
}

void My::LightManager::Reset()
{
	if (m_pLightInfo) {
		dynamic_cast<MemoryManager*>(m_pApp->GetMemoryManager())->Free(m_pLightInfo, sizeof(My::LightInfo));
		m_pLightInfo = nullptr;
		m_iLightNum = 0;
	}
}

void My::LightManager::Create()
{
	if (m_pLightInfo) {
		std::cout << "[LightManager] Already Has Light Info!" << std::endl;
		Reset();
	}
	m_pLightInfo = (LightInfo*)(dynamic_cast<MemoryManager*>(m_pApp->GetMemoryManager())->Allocate(sizeof(My::LightInfo)));
}

void My::LightManager::UpdateLight()
{
    for (int i = 0; i < m_iLightNum; i++) {
        auto& l = m_pLightInfo->Lights[i];
        auto& info = m_LightInfoMap[l.LightIndex];
        if (!isNear(info->lastPosition, *info->LightPosition) || abs(info->defaultIntensity - *info->intensity) > EP) {
            UpdateLightViewProjMatrix(l);
        }
        
    }
}

void My::LightManager::SetPerLightInfo(uint8_t idx, std::shared_ptr<SceneLightNode>& pNodeLight, const std::shared_ptr<SceneObjectLight>& pSceneLight, std::string name)
{
    std::unique_ptr<LightObject> lo = std::make_unique<LightObject>();
    auto pLight = pSceneLight.get();
    Light& l = m_pLightInfo->Lights[idx];
    memset(&l, 0, sizeof(Light));
    l.Intensity = pSceneLight->GetIntensity();
    l.LightColor = pSceneLight->GetColor().Value;
    l.Type = pSceneLight->GetLightType();
    l.IsCastShadow = pSceneLight->GetIfCastShadow();
    l.LightIndex = idx;

    Matrix4X4f lightTrans = *pNodeLight->GetCalculatedTransform();
    Matrix4X4f T;
    Transpose(T, lightTrans);
    // direction, in gltf default light direction is set by z-reserve, multiple its rotation matrix to get its dir 
    MatrixMulVector(l.LightDirection, g_VectorYReserve, T);
    l.LightPosition = Vector4f(lightTrans[0][3], lightTrans[1][3], lightTrans[2][3], 1.0f);

    lo->defaultIntensity = l.Intensity;
    lo->intensity = &l.Intensity;
    lo->IsCastShadow = &l.IsCastShadow;
    lo->lastPosition = l.LightPosition;
    lo->LightColor = &l.LightColor;
    lo->LightDirection = &l.LightDirection;
    lo->LightPosition = &l.LightPosition;
    lo->LightIndex = idx;
    lo->name = pNodeLight->GetSceneObjectRef();
    lo->LightShadowMapIndex = &l.LightShadowMapIndex;

    switch (l.Type) {
    case LightType::Omni:
    {

    }
    break;
    case LightType::Infinity:
    {

    }
    break;
    case LightType::Spot:
    {
        lo->Spot.conAngle = reinterpret_cast<SceneObjectSpotLight*>(pLight)->GetConAngle();
        lo->Spot.penumbraAngle = reinterpret_cast<SceneObjectSpotLight*>(pLight)->GetPenumbraAngle();
    }
    break;
    case LightType::Area:
    {

    }
    break;
    default:
        break;
    }

    m_LightInfoMap.emplace(idx, std::move(lo));

    // 先存起来再更新矩阵，否则会在update中找不到对应的数据
    UpdateLightViewProjMatrix(l);

    m_LightInfoMap[idx]->LightProjectionMatrix = &l.LightProjectionMatrix;
    m_LightInfoMap[idx]->LightViewMatrix = &l.LightViewMatrix;

    m_iLightNum++;
    std::cout << "[Light Manager] Add Light Index: " << (int)idx << std::endl;
}

void My::LightManager::UpdateLightViewProjMatrix(Light& l)
{
    auto& info = m_LightInfoMap[l.LightIndex];

    switch (l.Type)
    {
    case LightType::Omni:
    {

    }
    break;
    case LightType::Infinity:
    {

    }
    break;
    case LightType::Spot:
    {
        l.LightProjectionMatrix = My::BuildPerspectiveMatrix(2 * info->Spot.penumbraAngle, 1.0f, 1.0f, l.Intensity);

        Vector4f up = Vector4f(.0f, 1.0f, 0.0f, .0f);
        // 为了投影矩阵的方向正确性，矩阵的正方向与光照方向相反。
        OpsiteVector(l.LightDirection);
        //Vector4f right = CrossProduct(up, l.LightDirection);
        //Normalize(right);
        //up = CrossProduct(l.LightDirection, right);
        //Normalize(up);

        //float lightDotRight = -DotProduct(Vector3f(l.LightPosition.x, l.LightPosition.y, l.LightPosition.z), Vector3f(right.x, right.y, right.z));
        //float lightDotUp = -DotProduct(Vector3f(l.LightPosition.x, l.LightPosition.y, l.LightPosition.z), Vector3f(up.x, up.y, up.z));
        //float lightDotDir = -DotProduct(Vector3f(l.LightPosition.x, l.LightPosition.y, l.LightPosition.z), Vector3f(l.LightDirection.x, l.LightDirection.y, l.LightDirection.z));

        //l.LightViewMatrix = { {{
        //    {right.x, up.x, l.LightDirection.x, 0.0f},
        //    {right.y, up.y, l.LightDirection.y, 0.0f},
        //    {right.z, up.z, l.LightDirection.z, 0.0f},
        //    {lightDotRight, lightDotUp, lightDotDir, 1.0f}
        //}} };

        My::Vector4f lookAt = l.LightPosition + l.LightDirection;
        BuildViewMatrix(l.LightViewMatrix, l.LightPosition, lookAt, up);
    }
    break;
    case LightType::Area:
    {

    }
    break;
    default:
        assert(false, "Unknown Light Type!");
        std::cout << "[D3d12 Initialize Light] Unknown Light Type!" << std::endl;
        break;
    }
}

My::LightInfo My::LightManager::GetAllLightInfo()
{
	return *m_pLightInfo;
}

My::LightInfo* My::LightManager::GetAllLightInfoPtr()
{
	return m_pLightInfo;
}

uint8_t My::LightManager::GetLightNum()
{
	return m_iLightNum;
}

std::string My::LightManager::GetLightName(uint8_t idx)
{
	if (m_LightInfoMap.find(idx) != m_LightInfoMap.end()) {
		return m_LightInfoMap[idx]->name;
	}
	return std::string();
}