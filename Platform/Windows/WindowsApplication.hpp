#pragma once
// include the basic windows header file
#include <windows.h>
#include <windowsx.h>
#include "BaseApplication.hpp"

#pragma comment(lib, "D3dCore.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")



namespace My {
    class WindowsApplication : public BaseApplication
    {
    public:
        WindowsApplication(GfxConfiguration& config)
            : BaseApplication(config) {};

        virtual int Initialize();
        virtual void Finalize();
        // One cycle of the main loop
        virtual void Tick();

        inline HWND GetMainWindow() const { return m_hWnd; };

    private:
        // the WindowProc function prototype
        static LRESULT CALLBACK WindowProc(HWND hWnd,
            UINT message,
            WPARAM wParam,
            LPARAM lParam);

    private:
        HWND m_hWnd;
    };
}
