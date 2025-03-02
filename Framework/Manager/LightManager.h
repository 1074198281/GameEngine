#pragma once
#include "cbuffer.h"
#include "BaseApplication.hpp"

#include <unordered_map>
#include <string>
#include <memory>

namespace My {

	typedef struct LightObject {
		LightObject() {
			LightViewMatrix = nullptr;
			LightProjectionMatrix = nullptr;
			LightPosition = nullptr;
			LightColor = nullptr;
			LightDirection = nullptr;
			LightShadowMapIndex = nullptr;
			intensity = nullptr;
			IsCastShadow = nullptr;
			Type = LightType::None;
			LightIndex = -1;
			name = "";
			defaultIntensity = 0;
			lastPosition = Vector4f();
			Spot.conAngle = 0;
			Spot.penumbraAngle = 0;
		}

		// constant buffer
		Matrix4X4f* LightViewMatrix;
		Matrix4X4f* LightProjectionMatrix;
		Vector4f* LightPosition;
		Vector4f* LightColor;
		Vector4f* LightDirection;
		int* LightShadowMapIndex;
		float* intensity;
		bool* IsCastShadow;
		LightType Type;
		uint8_t LightIndex;

		std::string name;

		float defaultIntensity;
		Vector4f lastPosition;

		union {
			struct {

			} Omni;
			
			struct {

			} Infinity;

			struct {
				float conAngle;
				float penumbraAngle;
			} Spot;

			struct {

			} Area;
		};
	} LightObject;

	class SceneObjectLight;
	class SceneLightNode;

	class LightManager
	{
	public:
		LightManager();
		virtual ~LightManager();

		virtual void Clear() {};
	public:
		void Reset();
		void Create();

		void UpdateLight();

		void SetPerLightInfo(uint8_t idx, std::shared_ptr<SceneLightNode>& pNodeLight, const std::shared_ptr<SceneObjectLight>& pSceneLight, std::string name);
	public:
		LightInfo GetAllLightInfo();
		LightInfo* GetAllLightInfoPtr();
		uint8_t GetLightNum();
		std::string GetLightName(uint8_t idx);

	protected:
		void UpdateLightViewProjMatrix(Light& l);
	protected:
		LightInfo* m_pLightInfo;
		uint8_t m_iLightNum;
		std::unordered_map<uint8_t, std::unique_ptr<LightObject> > m_LightInfoMap;

		BaseApplication* m_pApp;
	};
}
