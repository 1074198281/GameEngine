#pragma once
#include "IGraphicsManager.hpp"
#include "Scene.hpp"
#include "FrameStructure.hpp"
#include "IDrawPass.hpp"

namespace My {
    class LightManager;

    class GraphicsManager : __implements IGraphicsManager
    {
    public:
         ~GraphicsManager() override = default;

        int Initialize() override;
        void Finalize() override;

        void Tick() override;

        void Clear() override;
        void Draw() override;

        void Present() override {};

        void Resize(uint32_t width, uint32_t height) override;

        void UpArrowKeyDown() override;
        void DownArrowKeyDown() override;
        void LeftArrowKeyDown() override;
        void RightArrowKeyDown() override;
        void NumPadKeyDown(int64_t key) override;
        void NumPadKeyUp(int64_t key) override;
        void FunctionKeyDown(int64_t key) override;
        void FunctionKeyUp(int64_t key) override;

    public:
        // pass interface
        void DrawBatch(Frame& frame, uint8_t lightIdx = -1, bool castShadow = false, bool isDrawSkybox = false) override {}
        void DrawSkybox(Frame& frame) override {}
        void DrawGui(Frame& frame) override {}
        void DrawPresent(Frame& frame) override {}
        void DrawGaussBlur(Frame& frame) override {}
        void DrawWaterDrops(Frame& frame) override {}
        void DrawSea(Frame& frame) override {}

        void BeginSubPass(const std::string& PassName) override {}
        void EndSubPass() override {}
        void BeginOverlayPass() override {}
        void EndOverlayPass() override {}

        void SetPipelineStatus(const std::string& PSOName) override {}
        void SetBatchResources(Frame& frame) override {}
        void SetShadowResources(Frame& frame, uint8_t lightIdx) override {}
        void SetShadowMapDebugPresent(uint8_t lightIdx) override {}

    public:
        LightManager* GetLightManager() override { return nullptr; };

        std::vector<std::string> GetSkyboxInfo() override { return std::vector<std::string>(); }

        size_t GetSkyboxTextureGpuPtr(const std::string skyboxName, uint32_t& width, uint32_t& height) override { return 0; }
        size_t GetTextureGpuPtr(const int& batch_index, int material_index, uint32_t& width, uint32_t& height, uint32_t& size) override { return 0; }
    protected:
        virtual void BeginScene(const Scene& scene);
        virtual void EndScene();
        virtual void initializeGeometries(const Scene& scene) {}
        virtual void initializeSkybox(const Scene& scene) {}
        virtual void initializeLight(const Scene& scene) {}
        virtual void initializeFixedHandle() {}

        virtual void BeginFrame(Frame& frame) {}
        virtual void EndFrame(Frame& frame) {}

    public:
        ShadowMapPass* GetShadowMapPass() override;
        ForwardGeometryPass* GetForwardGeometryPass() override;
        OverlayPass* GetOverlayPass() override;
        PresentPass* GetPresentPass() override;

    protected:
        std::vector<Frame> m_Frames;
        std::vector<std::shared_ptr<IDrawPass>> m_DrawPasses;

        uint64_t m_nSceneRevision;
        uint32_t m_nFrameIndex;

    };
}
