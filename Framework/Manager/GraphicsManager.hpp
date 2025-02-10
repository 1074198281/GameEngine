#pragma once
#include "IGraphicsManager.hpp"
#include "Scene.hpp"
#include "FrameStructure.hpp"
#include "IDrawPass.hpp"

namespace My {
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
        void DrawBatch(Frame& frame, bool castShadow, bool isDrawSkybox) override {}
        void DrawSkybox(Frame& frame) override {}
        void DrawGui(Frame& frame) override {}
        void DrawPresent(Frame& frame) override {}
        void DrawGuassBlur(Frame& frame) override {}
        void DrawOverlay(Frame& frame) override {}

        void BeginSubPass(const std::string& PassName) override {}
        void EndSubPass() override {}
        void BeginOverlayPass() override {}
        void EndOverlayPass() override {}

        void SetPipelineStatus(const std::string& PSOName) override {}
        void SetBatchResources(Frame& frame) override {}
        void SetShadowResources(Frame& frame, Light lightInfo) override {}

    public:
        void* GetLightInfo() override { return nullptr; };
        std::string GetLightName(int index) override { return std::string(); };

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
        bool* GetDrawSkyboxStatus() override { return &m_bDrawSkybox; }
        bool* GetCastShadowStatus() override { return &m_bCastShadow; }
        bool* GetGuassBlurStatus() override { return &m_bGuassBlur; }
        int* GetSkyboxIndex() override { return &m_iSkyboxIndex; }
        bool* GetOverlayStatus() override { return &m_bDrawOverlay; }
    protected:
        std::vector<Frame> m_Frames;
        std::vector<std::shared_ptr<IDrawPass>> m_DrawPasses;

        bool m_bInitialized{ false };
        uint64_t m_nSceneRevision;
        uint32_t m_nFrameIndex;

        bool m_bDrawSkybox{ false };
        bool m_bCastShadow{ false };
        bool m_bGuassBlur{ false };

        int m_iSkyboxIndex{ 0 };
        bool m_bDrawOverlay{ false };
    };
}
