#include "GeometryStructure.h"

namespace My {
	D3D12_SAMPLER_DESC g_PointWarpSampler;
	D3D12_SAMPLER_DESC g_PointClampSampler;
	D3D12_SAMPLER_DESC g_LinearWarpSampler;
	D3D12_SAMPLER_DESC g_LinearClampSampler;
	D3D12_SAMPLER_DESC g_AnisotropicWarpSampler;
	D3D12_SAMPLER_DESC g_AnisotropicClampSampler;
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