#pragma once

#include "WindowsApplication.hpp"

namespace D3dGraphicsCore
{
    class D3d12RHI;
}

namespace My {
    

    class D3d12Application : public WindowsApplication {
    public:
        using WindowsApplication::WindowsApplication;
        ~D3d12Application() {}

    public:
        void Finalize() override;
        
        int CreateMainWindow() override;

        void LoadSceneResource(Scene& scene);

        D3dGraphicsCore::D3d12RHI& GetRHI() { return *m_GraphicsRHI; }
    private:
        D3dGraphicsCore::D3d12RHI* m_GraphicsRHI;
    };
}