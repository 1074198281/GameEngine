#pragma once
// include the basic windows header file

#include <windows.h>
#include <windowsx.h>
#include "BaseApplication.hpp"

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

        int CreateMainWindow() override;

        void* GetMainWindow() override;

        inline HWND GetMainWindowHWND() const { return m_hWnd; };

        void GetFrameBufferSize(uint32_t& width, uint32_t& height);
    private:
        // the WindowProc function prototype
        static LRESULT CALLBACK WindowProc(HWND hWnd,
            UINT message,
            WPARAM wParam,
            LPARAM lParam);

        int InitializeImGUI();

        void FinalizeImGUI();

    private:
        HWND m_hWnd;
    };
}
