#include "LightManager.h"

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

void My::LightManager::SetPerLightInfo(uint8_t idx, const Light& perLightInfo, std::string name)
{
	assert(idx >= 0 && idx <= MAX_LIGHT_NUM);
	if (idx < 0 || idx > MAX_LIGHT_NUM) return;
	m_pLightInfo->Lights[idx] = perLightInfo;
	m_LightInfoMap.emplace(idx, name);
	m_iLightNum++;
	std::cout << "[LightManager] Add Light Index: " << (int)idx << ", Light Name: " << name << std::endl;
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
		return m_LightInfoMap[idx];
	}
	return std::string();
}