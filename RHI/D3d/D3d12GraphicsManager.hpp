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
        virtual int Initialize();
        virtual void Finalize();

        virtual void Tick();

        virtual void Clear();
        virtual void Draw();
        virtual void Resize(uint32_t width, uint32_t height);

        virtual void UpArrowKeyDown();
        virtual void DownArrowKeyDown();
        virtual void LeftArrowKeyDown();
        virtual void RightArrowKeyDown();
        virtual void NumPadKeyDown(int64_t key);
        virtual void NumPadKeyUp(int64_t key);
        virtual void FunctionKeyDown(int64_t key);
        virtual void FunctionKeyUp(int64_t key);

        virtual void StartGUIFrame();
        virtual void EndGUIFrame();
    public:


    private:
        bool LoadScene();
        bool GenerateInputLayoutType(D3dGraphicsCore::PrimitiveObject* _object, const std::string& name);

        int InitializeD3dImGUI();
    private:
        std::unique_ptr<D3dGraphicsCore::CD3dGraphicsCore> m_pGraphics;
    };
}