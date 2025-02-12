#include "ShaderSource.h"
#include "Core/Pipeline/PipelineState.h"

#include "../Asset/Shaders/CompiledShaders/g_Cube_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_Cube_PS.h"
#include "../Asset/Shaders/CompiledShaders/g_ToyCar_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_ToyCar_PS.h"
#include "../Asset/Shaders/CompiledShaders/g_Fabric_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_Fabric_PS.h"
#include "../Asset/Shaders/CompiledShaders/g_SkyBox_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_SkyBox_PS.h"
#include "../Asset/Shaders/CompiledShaders/g_Present_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_Present_PS.h"
#include "../Asset/Shaders/CompiledShaders/g_Model_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_Model_PS.h"
#include "../Asset/Shaders/CompiledShaders/g_OverlaySub_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_OverlaySub_PS.h"


#include "../Asset/Shaders/CompiledShaders/g_GuassBlur_CS.h"
#include "../Asset/Shaders/CompiledShaders/g_MyGuassBlur_CS.h"


// lighting
#include "../Asset/Shaders/CompiledShaders/g_LambertGouraudLighting_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_LambertGouraudLighting_PS.h"
#include "../Asset/Shaders/CompiledShaders/g_BlinnPhongLighting_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_BlinnPhongLighting_PS.h"


// shadow
#include "../Asset/Shaders/CompiledShaders/g_SpotLight_VS.h"
#include "../Asset/Shaders/CompiledShaders/g_SpotLight_PS.h"

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
	ShaderByteCode NAME##VS = { g_p##NAME##_VS, sizeof(g_p##NAME##_VS) }; \
	ShaderByteCode NAME##PS = { g_p##NAME##_PS, sizeof(g_p##NAME##_PS) }; \
	g_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##VS), NAME##VS)); \
	g_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##PS), NAME##PS)); \

	GENERATE_SHADER_BYTE_CODE(Cube);
	GENERATE_SHADER_BYTE_CODE(ToyCar);
	GENERATE_SHADER_BYTE_CODE(Fabric);
	GENERATE_SHADER_BYTE_CODE(SkyBox);
	GENERATE_SHADER_BYTE_CODE(Present);
	GENERATE_SHADER_BYTE_CODE(Model);
	GENERATE_SHADER_BYTE_CODE(OverlaySub);

	// lighting
	GENERATE_SHADER_BYTE_CODE(LambertGouraudLighting);
	GENERATE_SHADER_BYTE_CODE(BlinnPhongLighting);

	//shadow
	GENERATE_SHADER_BYTE_CODE(SpotLight);
#undef TOSTRING
#undef GENERATE_SHADER_BYTE_CODE
#endif // !GENERATE_SHADER_BYTE_CODE

#ifndef GENERATE_CS_SHADER_BYTE_CODE
#ifdef TOSTRING
#undef TOSTRING
#endif // TOSTRING
#define TOSTRING( NAME ) #NAME
#define GENERATE_CS_SHADER_BYTE_CODE(NAME)\
	ShaderByteCode cs##NAME##ByteCode = { g_p##NAME##_CS, sizeof(g_p##NAME##_CS)};\
	g_ShaderByteMap.insert(std::make_pair(TOSTRING(NAME##CS), cs##NAME##ByteCode)); \

	GENERATE_CS_SHADER_BYTE_CODE(GuassBlur)
	GENERATE_CS_SHADER_BYTE_CODE(MyGuassBlur)

#endif // GENERATE_CS_SHADER_BYTE_CODE

}

void D3dGraphicsCore::FinalizeShaderByteMap()
{
	g_ShaderByteMap.clear();
}
