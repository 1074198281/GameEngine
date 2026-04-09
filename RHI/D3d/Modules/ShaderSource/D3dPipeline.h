#pragma once


#include "../../Core/Pipeline/RootSignature.h"
#include "../../Core/Pipeline/PipelineState.h"

#include <unordered_map>

namespace My
{
	class CD3dPipeline
	{
	public:
		CD3dPipeline(bool isCompute = false);
		~CD3dPipeline();
		CD3dPipeline(const wchar_t* name, bool isCompute = false);

		CD3dPipeline(CD3dPipeline& rhs) = default;
		CD3dPipeline(CD3dPipeline&& rhs) = default;

	public:
		D3dGraphicsCore::ComputePSO* GetComputePSO() { return m_pComputePSO.get(); }
		D3dGraphicsCore::GraphicsPSO* GetPSO() { return m_pPSO.get(); }
		D3dGraphicsCore::RootSignature* GetRootSignature() { return m_pRootSignature.get(); }
		void SetRootSignature(std::shared_ptr<D3dGraphicsCore::RootSignature> rootsignature);

	private:
		std::shared_ptr<D3dGraphicsCore::ComputePSO> m_pComputePSO;
		std::shared_ptr<D3dGraphicsCore::GraphicsPSO> m_pPSO;
		std::shared_ptr<D3dGraphicsCore::RootSignature> m_pRootSignature;
	};


	typedef struct ShaderByteCode {
		const unsigned char* pShaderByteCode;
		size_t size;
	} ShaderByteCode;

	class CD3dPipelineManager
	{
	public:
		CD3dPipelineManager();
		~CD3dPipelineManager();

	public:
		CD3dPipeline* GetGraphicsPSO(std::string PSOName);
		CD3dPipeline* GetComputePSO(std::string PSOName);

	public:
		void InitializeShaderByteMap();
		void FinalizeShaderByteMap();
		void SetShaderByteCode(D3dGraphicsCore::GraphicsPSO& PSO, std::string name);

		void InitializePipelineTemplates();
		void FinalizePipelineTemplates();

		//void SetPipelineSettings(D3dGraphicsCore::GraphicsPSO& PSO, const int& InputLayoutType, const My::PrimitiveType& PrimitiveType, const std::string& Name);
	private:
		void InitializeGeometryPipelines();
		void InitializeOverlayPipelines();
		void InitializeLightPipelines();

		void AddLightingShaders();

	private:
		std::unordered_map<std::string, ShaderByteCode> m_ShaderByteMap;
		std::unordered_map<std::string, std::unique_ptr<CD3dPipeline>> m_GraphicsPSOMap;
		std::unordered_map<std::string, std::unique_ptr<CD3dPipeline>> m_ComputePSOMap;
		std::shared_ptr<D3dGraphicsCore::RootSignature> m_pDefaultRootSignature;
	};

}