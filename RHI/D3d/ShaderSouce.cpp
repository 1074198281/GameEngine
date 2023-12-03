#include "ShaderSource.h"
#include "Core/Pipeline/PipelineState.h"

namespace D3dGraphicsCore {
	std::unordered_map<std::string, ShaderByteCode> g_ShaderByteMap;
}

void D3dGraphicsCore::SetShaderByteCode(D3dGraphicsCore::GraphicsPSO& PSO, std::string name)
{
	using D3dGraphicsCore::ShaderByteCode;
	using D3dGraphicsCore::g_ShaderByteMap;
	std::string VSName = name + "VS";
	std::string PSName = name + "PS";
	ShaderByteCode VS = g_ShaderByteMap[VSName];
	ShaderByteCode PS = g_ShaderByteMap[PSName];
	PSO.SetVertexShader(VS.pShaderByteCode, VS.size);
	PSO.SetPixelShader(PS.pShaderByteCode, PS.size);
}

void D3dGraphicsCore::InitializeShaderByteMap()
{
#define TOSTRING( NAME ) #NAME
#define GENERATE_SHADER_BYTE_CODE( NAME ) \
	ShaderByteCode NAME##VS = { g_p##NAME##VS, sizeof(g_p##NAME##VS) }; \
	ShaderByteCode NAME##PS = { g_p##NAME##PS, sizeof(g_p##NAME##PS) }; \
	g_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##VS), NAME##VS)); \
	g_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##PS), NAME##PS)); \

	GENERATE_SHADER_BYTE_CODE(Cube);

	//ShaderByteCode CubeVS = { g_pCubeVS, sizeof(g_pCubeVS) };
	//ShaderByteCode CubePS = { g_pCubePS, sizeof(g_pCubePS) };

	//g_ShaderByteMap.insert(std::make_pair("CubeVS", CubeVS));
	//g_ShaderByteMap.insert(std::make_pair("CubePS", CubePS));
}

void D3dGraphicsCore::FinalizeShaderByteMap()
{
	g_ShaderByteMap.clear();
}
