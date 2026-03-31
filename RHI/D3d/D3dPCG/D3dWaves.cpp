#include "D3dWaves.h"


using namespace My::PCG;


CD3dWavesPipeline::CD3dWavesPipeline()
{
	m_pWavesRootSignature = std::make_shared<D3dGraphicsCore::RootSignature>();
	m_pWavesPSO = std::make_shared<D3dGraphicsCore::GraphicsPSO>();
}

CD3dWavesPipeline::~CD3dWavesPipeline()
{

}

D3dGraphicsCore::RootSignature* CD3dWavesPipeline::GetRootSignature()
{
	return m_pWavesRootSignature.get();
}

D3dGraphicsCore::GraphicsPSO* CD3dWavesPipeline::GetPSO()
{
	return m_pWavesPSO.get();
}