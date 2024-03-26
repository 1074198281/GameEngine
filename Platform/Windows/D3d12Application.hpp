#pragma once

#include "WindowsApplication.hpp"
#include "D3d/GraphicsCore.h"

namespace My {
    class D3d12Application : public WindowsApplication {
    public:
        using WindowsApplication::WindowsApplication;
        ~D3d12Application() {}

    public:
        void Finalize() override;
        
        int CreateMainWindow() override;

        void LoadSceneResource(Scene& scene);

        D3dGraphicsCore::CD3dGraphicsCore& GetRHI() { return m_GraphicsRHI; }
    private:
        D3dGraphicsCore::CD3dGraphicsCore m_GraphicsRHI;
    };
}