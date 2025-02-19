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
#include "Core/Resource/DepthBuffer.h"
#include "Core/Resource/ColorBuffer.h"
#include "Modules/LightManager/D3d12LightManager.h"


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
        void DrawBatch(Frame& frame, uint8_t lightIdx = -1, bool castShadow = false, bool isDrawSkybox = false) override;
        void DrawSkybox(Frame& frame) override;
        void DrawGui(Frame& frame) override;
        void DrawPresent(Frame& frame) override;
        void DrawGuassBlur(Frame& frame) override;
        void DrawOverlay(Frame& frame) override;

        void BeginSubPass(const std::string& PassName) override;
        void EndSubPass() override;
        void BeginOverlayPass() override;
        void EndOverlayPass() override;

        void SetPipelineStatus(const std::string& PSOName) override;
        void SetBatchResources(Frame& frame) override;
        void SetShadowResources(Frame& frame, uint8_t lightIdx) override;

    public:
        void* GetLightInfo() override;
        std::string GetLightName(int index) override;

        std::vector<std::string> GetSkyboxInfo() override;

        size_t GetSkyboxTextureGpuPtr(const std::string skyboxName, uint32_t& width, uint32_t& height) override;
        size_t GetTextureGpuPtr(const int& batch_index, int material_index, uint32_t& width, uint32_t& height, uint32_t& size) override;
        size_t GetShadowMapPtr(My::LightType type, int index) override;
        
    private:
        void initializeGeometries(const Scene& scene) override;
        void initializeSkybox(const Scene& scene) override;
        void initializeLight(const Scene& scene) override;
        void initializeFixedHandle() override;

        int InitializeD3dImGUI();

        void LoadIBLDDSImage(std::string& ImagePath, std::string& suffix, std::unordered_map<std::string, int>& ImageName);
        bool GenerateInputLayoutType(uint32_t& InputLayout, const std::string& name);
        
        void ResizeFrameBuffer();

    private:
        std::unique_ptr<D3d12LightManager> m_pLightManager;
    private:
        std::vector<std::unique_ptr<D3dGraphicsCore::StructuredBuffer>> m_VecVertexBuffer;
        std::vector<std::unique_ptr<D3dGraphicsCore::ByteAddressBuffer>> m_VecIndexBuffer;



        std::unique_ptr<SResourceIBLImage> m_IBLResource;
        std::unordered_map<uint32_t, std::unique_ptr<My::SResourceBatchInfo>> m_BatchTextureResource;
        std::unordered_map<std::string, std::unique_ptr<SResourceColorBufferInfo>> m_PixelBufferResources;
    };
}