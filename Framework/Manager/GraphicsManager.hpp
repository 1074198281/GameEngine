#pragma once
#include "IGraphicsManager.hpp"
#include "Scene.hpp"
#include "FrameStructure.hpp"

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


    protected:
        virtual void BeginScene(const Scene& scene);
        virtual void EndScene();

        virtual void BeginFrame(Frame& frame) {}
        virtual void EndFrame(Frame& frame) {}
        virtual void StartGUIFrame() {}
        virtual void EndGUIFrame() {}

    private:
        std::vector<Frame> m_Frames;
        uint64_t m_nSceneRevision;
        uint32_t m_nFrameIndex;
    };
}