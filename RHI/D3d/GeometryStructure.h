#pragma once
#include <vector>
#include <math.h>
#include <unordered_map>
#include "d3dx12.h"
#include <DirectXMath.h>
#include <memory>
#include <utility>



namespace My {


	//---------------------------------------shaders---------------------------------------//
	typedef struct ShaderByteCode {
		const unsigned char* pData;
		size_t size;
	}ShaderByteCode;
	void InitializeShaderByteCodeMap();
	extern std::unordered_map<std::string, ShaderByteCode> g_VertexShaderByteCodeMap;
	extern std::unordered_map<std::string, ShaderByteCode> g_PixelShaderByteCodeMap;


	//---------------------------------------samplers---------------------------------------//
	void InitializeSamplers();
	extern D3D12_SAMPLER_DESC g_PointWarpSampler;
	extern D3D12_SAMPLER_DESC g_PointClampSampler;
	extern D3D12_SAMPLER_DESC g_LinearWarpSampler;
	extern D3D12_SAMPLER_DESC g_LinearClampSampler;
	extern D3D12_SAMPLER_DESC g_AnisotropicWarpSampler;
	extern D3D12_SAMPLER_DESC g_AnisotropicClampSampler;

	//---------------------------------------inputlayout---------------------------------------//
	void InitializeInputLayout();
	extern D3D12_INPUT_ELEMENT_DESC* g_Pos;
	extern D3D12_INPUT_ELEMENT_DESC* g_PosTex;
	extern D3D12_INPUT_ELEMENT_DESC* g_PosNorTex;
	extern D3D12_INPUT_ELEMENT_DESC* g_PosNorTanTex;
}
