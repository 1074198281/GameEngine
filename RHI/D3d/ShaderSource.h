#pragma once

#include <map>
#include <unordered_map>
#include <string>

typedef unsigned char BYTE;

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