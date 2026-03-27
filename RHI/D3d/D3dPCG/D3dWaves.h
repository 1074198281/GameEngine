#pragma once 

#include "../Core/Pipeline/PipelineState.h"
#include "../Core/Pipeline/RootSignature.h"

namespace My::PCG
{
	class CD3dWavesPipeline
	{
	public:
		CD3dWavesPipeline();
		~CD3dWavesPipeline();

	public:
		D3dGraphicsCore::RootSignature GetRootSignature();
		D3dGraphicsCore::GraphicsPSO GetPSO();

	private:
		D3dGraphicsCore::RootSignature m_WavesRootSignature;
		D3dGraphicsCore::GraphicsPSO m_WavesPSO;


	//private:
	//	理论上好像是可以有复制构造的，管线状态可以是多个mesh使用一个pso
	//	CD3dWavesPipeline(CWavesPipeline& rhs) = default;
	//	CD3dWavesPipeline(CWavesPipeline&& rhs) = default;
	};
}
