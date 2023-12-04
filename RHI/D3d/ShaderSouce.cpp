#include "ShaderSource.h"
#include "Core/Pipeline/PipelineState.h"

#include "../Asset/Shaders/CompiledShaders/g_CubeVS.h"
#include "../Asset/Shaders/CompiledShaders/g_CubePS.h"
#include "../Asset/Shaders/CompiledShaders/g_ToyCarVS.h"
#include "../Asset/Shaders/CompiledShaders/g_ToyCarPS.h"
#include "../Asset/Shaders/CompiledShaders/g_FabricVS.h"
#include "../Asset/Shaders/CompiledShaders/g_FabricPS.h"


namespace D3dGraphicsCore {
	std::unordered_map<std::string, ShaderByteCode> g_ShaderByteMap;
}

void D3dGraphicsCore::SetShaderByteCode(D3dGraphicsCore::GraphicsPSO& PSO, std::string name)
{
	using D3dGraphicsCore::ShaderByteCode;
	using D3dGraphicsCore::g_ShaderByteMap;

#ifndef SET_SHADER_BYTE_CODE
#define SET_SHADER_BYTE_CODE( NAME ) \
	std::string VSName = NAME + "VS"; \
	std::string PSName = NAME + "PS"; \
	ShaderByteCode VS = g_ShaderByteMap[VSName]; \
	ShaderByteCode PS = g_ShaderByteMap[PSName]; \
	PSO.SetVertexShader(VS.pShaderByteCode, VS.size); \
	PSO.SetPixelShader(PS.pShaderByteCode, PS.size);

	SET_SHADER_BYTE_CODE(name);

#undef SET_SHADER_BYTE_CODE
#endif // !SET_SHADER_BYTE_CODE
}

void D3dGraphicsCore::InitializeShaderByteMap()
{
#ifndef GENERATE_SHADER_BYTE_CODE
#ifdef TOSTRING
#undef TOSTRING
#endif // TOSTRING
#define TOSTRING( NAME ) #NAME
#define GENERATE_SHADER_BYTE_CODE( NAME ) \
	ShaderByteCode NAME##VS = { g_p##NAME##VS, sizeof(g_p##NAME##VS) }; \
	ShaderByteCode NAME##PS = { g_p##NAME##PS, sizeof(g_p##NAME##PS) }; \
	g_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##VS), NAME##VS)); \
	g_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##PS), NAME##PS)); \

	GENERATE_SHADER_BYTE_CODE(Cube);
	GENERATE_SHADER_BYTE_CODE(ToyCar);
	GENERATE_SHADER_BYTE_CODE(Fabric);

#undef TOSTRING
#undef GENERATE_SHADER_BYTE_CODE
#endif // !GENERATE_SHADER_BYTE_CODE
}

void D3dGraphicsCore::FinalizeShaderByteMap()
{
	g_ShaderByteMap.clear();
}
