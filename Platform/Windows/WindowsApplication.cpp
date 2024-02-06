#include "WindowsApplication.hpp"
#include "InputManager.hpp"
#include <tchar.h>

using namespace My;

int My::WindowsApplication::Initialize()
{
    int result;

    // get the HINSTANCE of the Console Program
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // the handle for the window, filled by a function
    HWND hWnd;
    // this struct holds information for the window class
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("GameEngineFromScratch");

    // register the window class
    RegisterClassEx(&wc);

    // create the window and use the result as the handle
    hWnd = CreateWindowEx(0,
        _T("GameEngineFromScratch"),      // name of the window class
        (LPCSTR)m_Config.appName,                 // title of the window
        WS_OVERLAPPEDWINDOW,              // window style
        CW_USEDEFAULT,                    // x-position of the window
        CW_USEDEFAULT,                    // y-position of the window
        m_Config.screenWidth,             // width of the window
        m_Config.screenHeight,            // height of the window
        NULL,                             // we have no parent window, NULL
        NULL,                             // we aren't using menus, NULL
        hInstance,                        // application handle
        this);                            // pass pointer to current object

// display the window on the screen
    ShowWindow(hWnd, SW_SHOW);

    m_hWnd = hWnd;

    // first call base class initialization
    result = BaseApplication::Initialize();

    if (result != 0)
        exit(result);

    return result;
}

void My::WindowsApplication::Finalize()
{
    BaseApplication::Finalize();
}

void My::WindowsApplication::Tick()
{
    BaseApplication::Tick();

    // this struct holds Windows event messages
    MSG msg;

    // we use PeekMessage instead of GetMessage here
    // because we should not block the thread at anywhere
    // except the engine execution driver module 
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        // translate keystroke messages into the right format
        TranslateMessage(&msg);

        // send the message to the WindowProc function
        DispatchMessage(&msg);
    }
}

// this is the main message handler for the program
LRESULT CALLBACK My::WindowsApplication::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowsApplication* pThis;
    if (message == WM_NCCREATE)
    {
        pThis = static_cast<WindowsApplication*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

        SetLastError(0);
        if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
        {
            if (GetLastError() != 0)
                return FALSE;
        }
    }
    else
    {
        pThis = reinterpret_cast<WindowsApplication*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    // sort through and find what code to run for the message given
    switch (message)
    {
    case WM_PAINT:
    {
        pThis->OnDraw();
    }
    break;

    case WM_KEYDOWN:
    {
        // we will replace this with input manager
        switch (wParam)
        {
#ifndef USE_DIRECTX_INPUT
        case VK_LEFT:
            g_pInputManager->LeftArrowKeyDown();
            break;
        case VK_RIGHT:
            g_pInputManager->RightArrowKeyDown();
            break;
        case VK_UP:
            g_pInputManager->UpArrowKeyDown();
            break;
        case VK_DOWN:
            g_pInputManager->DownArrowKeyDown();
            break;
        case VK_NUMPAD0:
        case VK_NUMPAD1:
        case VK_NUMPAD2:
        case VK_NUMPAD3:
        case VK_NUMPAD4:
        case VK_NUMPAD5:
        case VK_NUMPAD6:
        case VK_NUMPAD7:
        case VK_NUMPAD8:
        case VK_NUMPAD9:
            g_pInputManager->NumPadKeyDown(wParam);
            break;
        case VK_PRIOR:
        case VK_NEXT:
            g_pInputManager->FunctionKeyDown(wParam);
            break;
#endif
        case 0x52:
            g_pInputManager->FunctionKeyDown(wParam);
            break;
        case VK_ESCAPE:
            m_bQuit = true;
            break;
        default:
            break;
        }
    }
    break;

    // this message is read when the window is closed
    case WM_DESTROY:
    {
        // close the application entirely
        PostQuitMessage(0);
        m_bQuit = true;
        return 0;
    }
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
}

