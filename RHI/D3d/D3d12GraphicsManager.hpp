#pragma once
#include <stdint.h>
#include <d3d12.h>
#include <DXGI1_4.h>
#include <unordered_map>
#include "GraphicsManager.hpp"
#include "Buffer.hpp"
#include "SceneObject.hpp"
#include "SceneNode.hpp"
#include "StructureSettings.h"
#include "Core/Resource/GpuBuffer.h"


namespace My {
    class D3d12GraphicsManager : public GraphicsManager
    {
    public:
        //~D3d12GraphicsManager() final {}

        int Initialize() override;
        void Finalize() override;

        void Clear() override;
        //void Draw() override;
        void Resize(uint32_t width, uint32_t height) override;
        void Present() override;

        void UpArrowKeyDown() final;
        void DownArrowKeyDown() final;
        void LeftArrowKeyDown() final;
        void RightArrowKeyDown() final;
        void NumPadKeyDown(int64_t key) final;
        void NumPadKeyUp(int64_t key) final;
        void FunctionKeyDown(int64_t key) final;
        void FunctionKeyUp(int64_t key) final;

    public:
        void BeginFrame(Frame& frame) override;
        void EndFrame(Frame& frame) override;
        void UpdateD3dFrameConstants(Frame& frame);
    public:
        // pass interface
        void DrawBatch(Frame& frame) override;
        void DrawSkybox(Frame& frame) override;
        void DrawGui(Frame& frame) override;
        void DrawPresent(Frame& frame) override;

        void BeginSubPass(std::string PassName) override;
        void EndSubPass() override;

        void SetPipelineStatus(std::string PSOName) override;
        void SetBatchResources(Frame& frame) override;
        void SetShadowResources(Frame& frame) override;

    private:
        void initializeGeometries(const Scene& scene) override;
        void initializeSkybox(const Scene& scene) override;
        void initializeFixedHandle() override;

        int InitializeD3dImGUI();

        void LoadIBLDDSImage(std::string& ImagePath, std::string& suffix, std::unordered_map<std::string, int>& ImageName);
        bool GenerateInputLayoutType(uint32_t& InputLayout, const std::string& name);
        
    private:
        std::vector<std::unique_ptr<D3dGraphicsCore::StructuredBuffer>> m_VecVertexBuffer;
        std::vector<std::unique_ptr<D3dGraphicsCore::ByteAddressBuffer>> m_VecIndexBuffer;
        std::vector<std::shared_ptr<D3dGraphicsCore::GpuTexture>> m_VecTexture;
        std::vector<std::shared_ptr<D3dGraphicsCore::DepthBuffer>> m_ShadowTexture;
        std::unique_ptr<IBLImageResource> m_IBLResource;
        std::unordered_map<uint32_t, My::DescriptorHeapHandleInfo> m_BatchHandleStatus;
        std::unordered_map<std::string, My::DescriptorHeapHandleInfo> m_FixedHandleStatus;
    };
}