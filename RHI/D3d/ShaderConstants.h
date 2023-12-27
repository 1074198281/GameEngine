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

	typedef struct ConstantMaterial {
		XMFLOAT4 EyePos;
		XMFLOAT4 GlobalInfiniteLightPos;

		int LightNum;
		XMFLOAT4 LightDirection[MAX_LIGHT_NUM];
	} ConstantMaterial;

}