#pragma once
#include <DirectXMath.h>


namespace D3dGraphicsCore {
	using DirectX::XMVECTOR;
	using DirectX::XMMATRIX;
	using DirectX::XMFLOAT4;

#ifndef MAX_LIGHT_NUM
#define MAX_LIGHT_NUM 16
#endif

	typedef struct ConstantBufferView {
		XMMATRIX ModelMatrix;
		XMMATRIX ViewMatrix;
		XMMATRIX ProjMatrix;
	} ContantBufferView;

	/*
	这里考虑到GPU内存对齐的问题。需要将int放在后面以便获取值。
	如果将int放在第二位，那么需要在float4数组中补齐3个float，导致光源位置在shader中是错误的状态
	*/
	typedef struct ConstantMaterial {
		XMFLOAT4 EyePos;
		XMFLOAT4 LightPosition[MAX_LIGHT_NUM];
		int LightNum;
	} ConstantMaterial;


	// Model_PS -- cb0
	__declspec(align(16)) typedef struct MaterialConstants {
		float BaseColorFactor[4];
		float MetallicRoughnessFactor[2];
		float EmissiveFactor[3];
		float NormalTextureScale;
		float padding[2];
	} MaterialConstants;

	 __declspec(align(16)) typedef struct CommonConstants {
		XMFLOAT4 LightPosition[MAX_LIGHT_NUM];
		XMFLOAT4 LightColor[MAX_LIGHT_NUM];
		XMMATRIX ModelMatrix;
		XMMATRIX ViewMatrix;
		XMMATRIX ProjMatrix;
		XMFLOAT4 ViewerPos;
		XMFLOAT3 SunDirection;
		XMFLOAT3 SunInsensity;
		int LightNum;
		float IBLRange;
		float IBLBias;
		float padding[3];
	} CommonConstants;
}