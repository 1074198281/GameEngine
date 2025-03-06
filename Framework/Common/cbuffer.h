#pragma once
#include "shader_base.h"

namespace My {

    struct BasicVertex
    {
        Vector3f m_Position;
        Vector3f m_Normal;
        Vector4f m_Tangent;
        Vector2f m_TextureUV;
    };

	// Model_PS -- cb0
// TextureTransform[4] ==> offset0, offset1, scale0, scale1,   total:40 * float
	__declspec(align(16)) typedef struct MaterialConstants {
		//constants
		float BaseColorFactor[4];
		float EmissiveFactor[3];
		float NormalTextureScale;
		float MetallicRoughnessFactor[2];
		float padding0[2];
		//texture trans [5] ===> offset[2] + scale[2] + rotation[1]
		float BaseColorTextureTransform[4];
		float MetallicRoughnessTextureTransform[4];
		float OcclusionTextureTransform[4];
		float EmissiveTextureTransform[4];
		float NormalTextureTransform[4];
		float BaseColorRotation;
		float MetallicRoughnessRotation;
		float OcclusionRotation;
		float EmissiveRotation;
		float NormalRotation;
		float depth_bias;
		float padding1[2];
	} MaterialConstants;

	// cb1
	__declspec(align(16)) typedef struct PerBatchConstants {
		Matrix4X4f ModelMatrix;
	} PerBatchConstants;

	// cb2
	__declspec(align(16)) typedef struct PerFrameConstants {
		Matrix4X4f ViewMatrix;
		Matrix4X4f ProjectionMatrix;
		Vector4f CameraPosition;
		int LightNum;
		int clip_space_type;			//0: opengl ,1: others ≤√ºÙø’º‰¿‡–Õ
		float screenWidth;
		float screenHeight;
	} PerFrameConstants;

	// cb3
	__declspec(align(16)) typedef struct Light {
		Matrix4X4f LightViewMatrix;				// 16 float
		Matrix4X4f LightProjectionMatrix;		// 16 float
		Vector4f LightPosition;					// 4  float
		Vector4f LightColor;					// 4  float
		Vector4f LightDirection;				// 4  float
		int LightShadowMapIndex;				// 
		LightType Type;							// 
		float Intensity;						// 
		float conAngle;							// 4  float
		float penumbraAngle;					// 
		int IsCastShadow;						//
		int LightIndex;							//
		int DescriptorOffset;					// 4  float
	} Light;

	// cb4
	__declspec(align(16)) typedef struct LightProperties {
		union {
			struct {
				float conAngle;
				float penumbraAngle;
			} Spot;
		};
	} LightProperties;

	__declspec(align(16)) typedef struct LightInfo {
		Light Lights[MAX_LIGHT_NUM];
	} LightInfo;

}
