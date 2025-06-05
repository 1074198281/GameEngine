#pragma once

#include "IRuntimeModule.hpp"
#include "FrameStructure.hpp"
#include "Scene.hpp"

namespace My {
    class LightManager;
    class ShadowMapPass;
    class ForwardGeometryPass;
    class OverlayPass;
    class PresentPass;

    __Interface IGraphicsManager : public IRuntimeModule {
    public:
        IGraphicsManager() = default;
        virtual ~IGraphicsManager() = default;

        virtual int Initialize() { return 0; }
        virtual void Finalize() {}

        virtual void Tick() {}

        virtual void Clear() {}
        virtual void Draw() {}
        virtual void Present() {}

        virtual void Resize(uint32_t width, uint32_t height) {} 

        virtual void UpArrowKeyDown() {}
        virtual void DownArrowKeyDown() {}
        virtual void LeftArrowKeyDown() {}
        virtual void RightArrowKeyDown() {}
        virtual void NumPadKeyDown(int64_t key) {}
        virtual void NumPadKeyUp(int64_t key) {}
        virtual void FunctionKeyDown(int64_t key) {}
        virtual void FunctionKeyUp(int64_t key) {}

    public:
    // pass interface
        virtual void DrawBatch(Frame& frame, uint8_t lightIdx = -1, bool castShadow = false, bool isDrawSkybox = false) = 0;
        virtual void DrawSkybox(Frame& frame) = 0;
        virtual void DrawGui(Frame& frame) = 0;
        virtual void DrawPresent(Frame& frame) = 0;
        virtual void DrawGaussBlur(Frame& frame) = 0;
        virtual void DrawWaterDrops(Frame& frame) = 0;
        virtual void DrawSea(Frame& frame) = 0;
        virtual void DrawVolumetricLight(Frame& frame) = 0;

        virtual void BeginSubPass(const std::string& PassName) = 0;
        virtual void EndSubPass() = 0;
        virtual void BeginOverlayPass() = 0;
        virtual void EndOverlayPass() = 0;
        virtual void SetPipelineStatus(const std::string& PSOName) = 0;
        virtual void SetBatchResources(Frame& frame) = 0;
        virtual void SetShadowResources(Frame& frame, uint8_t lightIdx) = 0;
        virtual void SetShadowMapDebugPresent(uint8_t lightIdx) = 0;
    public:
        virtual LightManager* GetLightManager() = 0;
        virtual std::vector<std::string> GetSkyboxInfo() = 0;
        virtual size_t GetSkyboxTextureGpuPtr(const std::string skyboxName, uint32_t& width, uint32_t& height) = 0;
        virtual size_t GetTextureGpuPtr(const int& batch_index, int material_index, uint32_t& width, uint32_t& height, uint32_t& size) = 0;
    public:
        virtual ShadowMapPass* GetShadowMapPass() = 0;
        virtual ForwardGeometryPass* GetForwardGeometryPass() = 0;
        virtual OverlayPass* GetOverlayPass() = 0;
        virtual PresentPass* GetPresentPass() = 0;

	};
}