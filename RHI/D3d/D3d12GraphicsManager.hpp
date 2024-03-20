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

        int Initialize() final;
        void Finalize() final;

        void Clear() final;
        void Draw() final;
        void Resize(uint32_t width, uint32_t height) final;
        void Present() final;

        void UpArrowKeyDown() final;
        void DownArrowKeyDown() final;
        void LeftArrowKeyDown() final;
        void RightArrowKeyDown() final;
        void NumPadKeyDown(int64_t key) final;
        void NumPadKeyUp(int64_t key) final;
        void FunctionKeyDown(int64_t key) final;
        void FunctionKeyUp(int64_t key) final;

        void StartGUIFrame() final;
        void EndGUIFrame() final;
    public:


    private:
        bool LoadScene();
        bool GenerateInputLayoutType(D3dGraphicsCore::PrimitiveObject* _object, const std::string& name);

        int InitializeD3dImGUI();
    };
}