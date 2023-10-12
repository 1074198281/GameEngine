#include "GeometryStructure.h"
#include "MemoryManager.hpp"


//#include "../Asset/Shaders/CompiledShaders/g_mainPS.h"
//#include "../Asset/Shaders/CompiledShaders/g_mainVS.h"
#include "../Asset/Shaders/CompiledShaders/g_main4PS.h"
#include "../Asset/Shaders/CompiledShaders/g_main4VS.h"

#include "../Asset/Shaders/CompiledShaders/g_cubeTexturePS.h"
#include "../Asset/Shaders/CompiledShaders/g_cubeTextureVS.h"


namespace My {
	std::unordered_map<std::string, ShaderByteCode> g_VertexShaderByteCodeMap;
	std::unordered_map<std::string, ShaderByteCode> g_PixelShaderByteCodeMap;

	D3D12_SAMPLER_DESC g_PointWarpSampler;
	D3D12_SAMPLER_DESC g_PointClampSampler;
	D3D12_SAMPLER_DESC g_LinearWarpSampler;
	D3D12_SAMPLER_DESC g_LinearClampSampler;
	D3D12_SAMPLER_DESC g_AnisotropicWarpSampler;
	D3D12_SAMPLER_DESC g_AnisotropicClampSampler;

	D3D12_INPUT_ELEMENT_DESC* g_Pos;
	D3D12_INPUT_ELEMENT_DESC* g_PosTex;
	D3D12_INPUT_ELEMENT_DESC* g_PosNorTex;
	D3D12_INPUT_ELEMENT_DESC* g_PosNorTanTex;
}

void My::InitializeShaderByteCodeMap()
{
#ifndef CREATE_SHADER_BYTE_CODE_MAP
#define CREATE_SHADER_BYTE_CODE_MAP( codename , g_pVS, g_pPS ) \
		std::string name = codename;\
		ShaderByteCode vertexcode;\
		vertexcode.pData = g_pVS;\
		vertexcode.size = sizeof(g_pVS);\
		ShaderByteCode pixelcode;\
		pixelcode.pData = g_pPS;\
		pixelcode.size = sizeof(g_pPS);\
		g_VertexShaderByteCodeMap.emplace(name, vertexcode);\
		g_PixelShaderByteCodeMap.emplace(name, pixelcode);

	CREATE_SHADER_BYTE_CODE_MAP("cubeTexture", g_pcubeTextureVS, g_pcubeTexturePS);


#endif // !CREATE_SHADER_BYTE_CODE_MAP
}

void My::InitializeSamplers()
{
	g_PointWarpSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	g_PointWarpSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_PointWarpSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_PointWarpSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	g_PointWarpSampler.BorderColor[0] = 1.0f;
	g_PointWarpSampler.BorderColor[1] = 1.0f;
	g_PointWarpSampler.BorderColor[2] = 1.0f;
	g_PointWarpSampler.BorderColor[3] = 1.0f;
	g_PointWarpSampler.MinLOD = 0.0f;
	g_PointWarpSampler.MaxLOD = D3D12_FLOAT32_MAX;
	g_PointWarpSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NONE;

	g_LinearWarpSampler = g_PointWarpSampler;
	g_LinearWarpSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;

	g_AnisotropicWarpSampler = g_PointWarpSampler;
	g_LinearWarpSampler.Filter = D3D12_FILTER_ANISOTROPIC;


	g_PointClampSampler = g_PointWarpSampler;
	g_PointClampSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	g_PointClampSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	g_PointClampSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	g_PointClampSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	g_LinearClampSampler = g_PointClampSampler;
	g_LinearClampSampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;

	g_AnisotropicClampSampler = g_PointClampSampler;
	g_AnisotropicClampSampler.Filter = D3D12_FILTER_ANISOTROPIC;
}

void My::InitializeInputLayout()
{
	g_Pos = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(sizeof(D3D12_INPUT_ELEMENT_DESC));
	g_PosTex = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(2 * sizeof(D3D12_INPUT_ELEMENT_DESC));
	g_PosNorTex = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(3 * sizeof(D3D12_INPUT_ELEMENT_DESC));
	g_PosNorTanTex = (D3D12_INPUT_ELEMENT_DESC*)My::g_pMemoryManager->Allocate(4 * sizeof(D3D12_INPUT_ELEMENT_DESC));

	g_Pos[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

	g_PosTex[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosTex[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

	g_PosNorTex[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTex[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTex[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };

	g_PosNorTanTex[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTanTex[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTanTex[2] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
	g_PosNorTanTex[3] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 };
}