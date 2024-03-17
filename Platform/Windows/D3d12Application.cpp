#include "D3d12Application.hpp"
#include <tchar.h>


void My::D3d12Application::Finalize()
{

}

int My::D3d12Application::CreateMainWindow()
{
	WindowsApplication::CreateMainWindow();


    auto getFramebufferSize = [this](uint32_t& width, uint32_t& height) {
        GetFrameBufferSize(width, height);
    };

    auto getWindowHandler = [this]() -> HWND {
        return (HWND)GetMainWindow();
    };

    auto getGfxConfigHandler = [this]() { return GetConfiguration(); };

    // 设置回调函数
    m_GraphicsRHI.SetQueryFrameBufferSize(getFramebufferSize);
    m_GraphicsRHI.SetGetWindowHandleProc(getWindowHandler);
    m_GraphicsRHI.SetGetGfxconfiguration(getGfxConfigHandler);

    m_GraphicsRHI.StartUp();

	return 0;
}
