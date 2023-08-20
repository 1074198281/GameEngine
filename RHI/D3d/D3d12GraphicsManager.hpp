#pragma once
#include <stdint.h>
#include <d3d12.h>
#include <DXGI1_4.h>
#include <unordered_map>
#include "GraphicsManager.hpp"
#include "Buffer.hpp"
#include "Image.hpp"
#include "SceneObject.hpp"
#include "SceneNode.hpp"
#include "GeometryStructure.h"
#include "Core/GraphicsCore.h"


namespace My {
    class D3d12GraphicsManager : public GraphicsManager
    {
    public:
        virtual int Initialize();
        virtual void Finalize();

        virtual void Tick();

        virtual void Clear();
        virtual void Draw();

        virtual void MoveCameraXPositive();
        virtual void MoveCameraXNegative();
        virtual void MoveCameraYPositive();
        virtual void MoveCameraYNegative();
    public:


    private:
        bool LoadScene();


    private:
        std::unique_ptr<D3dGraphicsCore::CD3dGraphicsCore> m_pGraphics;
        DirectX::XMFLOAT3 m_EyePos;
        

    };
}