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

	typedef struct PerFrameConstants {
		Matrix4X4f ViewMatrix;
		Matrix4X4f ProjectionMatrix;
		Vector4f CameraPosition;
		int LightNum;
		int clip_space_type;			//0: opengl ,1: others ²Ã¼ô¿Õ¼äÀàÐÍ
		float padding0[2];
	} PerFrameConstants;

	typedef struct PerBatchConstants {
		Matrix4X4f ModelMatrix;
	} PerBatchConstants;

	// Model_PS -- cb0
	// TextureTransform[4] ==> offset0, offset1, scale0, scale1
	__declspec(align(16)) typedef struct MaterialConstants {
		float BaseColorFactor[4];
		float EmissiveFactor[3];
		float NormalTextureScale;
		float MetallicRoughnessFactor[2];
		float padding0[2];
		float BaseColorTextureTransform[4];
		float MetallicRoughnessTextureTransform[4];
		float OcclusionTransform[4];
		float EmissiveTextureTransform[4];
		float NormalTextureTransform[4];
		float BaseColorRotation;
		float MetallicRoughnessRotation;
		float OcclusionRotation;
		float EmissiveRotation;
		float NormalRotation;
		float padding1[3];
	} MaterialConstants;

	// cb1
	__declspec(align(16)) typedef struct CommonConstants {
		Matrix4X4f ModelMatrix;
		Matrix4X4f ViewMatrix;
		Matrix4X4f ProjMatrix;
		Vector4f CameraPosition;
		int LightNum;
		float padding0[3];
	} CommonConstants;

	typedef struct Light {
		Vector4f LightPosition;
		Vector4f LightColor;
		Vector4f LightDirection;
		float Insensity;
	} Light;

	// cb3
	typedef struct LightInfo {
		Light Lights[MAX_LIGHT_NUM];
	} LightInfo;

}
