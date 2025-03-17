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
			Intensity = nullptr;
			IsCastShadow = nullptr;
			Type = LightType::None;
			LightIndex = -1;
			name = "";
			lastIntensity = 0;
			lastPosition = Vector4f();
			isCastVolumetric = false;
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
		float* Intensity;
		int* IsCastShadow;
		LightType Type;
		int LightIndex;

		std::string name;

		float lastIntensity;
		Vector4f lastPosition;

		bool isCastVolumetric;
		union {
			struct {

			} Omni;
			
			struct {

			} Infinity;

			struct {
				float* conAngle;
				float* penumbraAngle;
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
		virtual void InitHandle() {};
		virtual void Create() ;
	public:
		virtual uint64_t GetColorGpuHandle() { return 0; };
		virtual uint64_t GetDepthGpuHandle() { return 0; };
		virtual uint64_t GetVolumnGpuHandle() { return 0; }
	public:
		void Reset();
		void UpdateLight();

		void SetPerLightInfo(uint8_t idx, std::shared_ptr<SceneLightNode>& pNodeLight, const std::shared_ptr<SceneObjectLight>& pSceneLight, std::string name);
	public:
		LightInfo GetAllLightInfo();
		LightInfo* GetAllLightInfoPtr();
		uint8_t GetLightNum();
		float* GetDepthBias();

		std::string GetLightName(uint8_t idx);
		bool* GetCastVolumetricPtr(uint8_t idx);
		bool GetCastVolumetric(uint8_t idx);
	public:
		virtual size_t GetShadowMapHandle(uint8_t idx) { return 0; };
	protected:
		void UpdateLightViewProjMatrix(Light& l);
	protected:
		LightInfo* m_pLightInfo;
		uint8_t m_iLightNum;
		std::unordered_map<uint8_t, std::unique_ptr<LightObject> > m_LightInfoMap;

		BaseApplication* m_pApp;

		float m_fDepthBias;
	};
}
