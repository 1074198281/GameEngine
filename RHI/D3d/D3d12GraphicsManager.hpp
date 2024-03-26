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
#include "GraphicsCore.h"
#include "RenderObject.h"


namespace My {
    class D3d12GraphicsManager : public GraphicsManager
    {
    public:
        //~D3d12GraphicsManager() final {}

        int Initialize() override;
        void Finalize() override;

        void Clear() override;
        void Draw() override;
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

        void StartGUIFrame() override;
        void EndGUIFrame() override;
        void BeginFrame(Frame& frame) override;
        void EndFrame(Frame& frame) override;
    public:


    private:
        void initializeGeometries(const Scene& scene) override;
        void initializeSkybox(const Scene& scene) override;
        bool GenerateInputLayoutType(uint32_t& InputLayout, const std::string& name);

        int InitializeD3dImGUI();

        struct D3dDrawBatchContext : DrawBatchContext {
            uint32_t m_vertex_buffer_index;
            uint32_t m_indice_buffer_index;
            PrimitiveType m_PrimitiveType;
            uint32_t m_index_count_per_instance;
            uint32_t m_inputlayout;
        };
    };
}