#include "RenderObject.h"
#include "GeometryStructure.h"

void My::RenderObject::Finalize()
{
	m_PSO.SetVertexShader(g_VertexShaderByteCodeMap[m_name].pData, g_VertexShaderByteCodeMap[m_name].size);
	m_PSO.SetPixelShader(g_PixelShaderByteCodeMap[m_name].pData, g_PixelShaderByteCodeMap[m_name].size);
}

void My::RenderObject::InitializeConstantBuffer()
{

}

void My::RenderObject::InitializeSRV()
{

}
