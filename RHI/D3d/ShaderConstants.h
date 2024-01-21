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
	���￼�ǵ�GPU�ڴ��������⡣��Ҫ��int���ں����Ա��ȡֵ��
	�����int���ڵڶ�λ����ô��Ҫ��float4�����в���3��float�����¹�Դλ����shader���Ǵ����״̬
	*/
	typedef struct ConstantMaterial {
		XMFLOAT4 EyePos;
		XMFLOAT4 LightPosition[MAX_LIGHT_NUM];
		int LightNum;
	} ConstantMaterial;

}