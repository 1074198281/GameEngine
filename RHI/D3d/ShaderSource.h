#pragma once

#include <map>
#include <unordered_map>
#include <string>

typedef unsigned char BYTE;

#include "../Asset/Shaders/CompiledShaders/g_CubeVS.h"
#include "../Asset/Shaders/CompiledShaders/g_CubePS.h"



namespace D3dGraphicsCore {
	class GraphicsPSO;

	typedef struct ShaderByteCode {
		const unsigned char* pShaderByteCode;
		size_t size;
	} ShaderByteCode;

	extern std::unordered_map<std::string, ShaderByteCode> g_ShaderByteMap;

	void InitializeShaderByteMap();

	void FinalizeShaderByteMap();

	void SetShaderByteCode(GraphicsPSO& PSO, std::string name);
}