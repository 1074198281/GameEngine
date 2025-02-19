#pragma once
#include "cbuffer.h"
#include "BaseApplication.hpp"

#include <unordered_map>
#include <string>

namespace My {
	class LightManager
	{
	public:
		LightManager();
		virtual ~LightManager();

	public:
		void Reset();
		void Create();

		void SetPerLightInfo(uint8_t idx, const Light& perLightInfo, std::string name);

	public:
		LightInfo GetAllLightInfo();
		LightInfo* GetAllLightInfoPtr();
		uint8_t GetLightNum();
		std::string GetLightName(uint8_t idx);
	protected:
		LightInfo* m_pLightInfo;
		uint8_t m_iLightNum;
		std::unordered_map<uint8_t, std::string> m_LightInfoMap;

		BaseApplication* m_pApp;
	};
}
