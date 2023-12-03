#include "XMInput.h"
#include "D3d/Core/D3dGraphicsCoreManager.h"
#include "D3d/Core/Utility.h"


namespace XM_Input {
    bool s_Buttons[2][kNumDigitalInputs];
    float s_HoldDuration[kNumDigitalInputs] = { 0.0f };
    float s_Analogs[kNumAnalogInputs];
    float s_AnalogsTC[kNumAnalogInputs];

#ifdef USE_KEYBOARD_MOUSE
    IDirectInput8A* s_DI;
    IDirectInputDevice8A* s_Keyboard;
    IDirectInputDevice8A* s_Mouse;

    DIMOUSESTATE2 s_MouseState;
    unsigned char s_Keybuffer[256];
    unsigned char s_DXKeyMapping[kNumKeys]; // map DigitalInput enum to DX key codes 
#endif

    inline float FilterAnalogInput(int val, int deadZone)
    {
        if (val < 0)
        {
            if (val > -deadZone)
                return 0.0f;
            else
                return (val + deadZone) / (32768.0f - deadZone);
        }
        else
        {
            if (val < deadZone)
                return 0.0f;
            else
                return (val - deadZone) / (32767.0f - deadZone);
        }
    }

#ifdef USE_KEYBOARD_MOUSE
    void KbmBuildKeyMapping()
    {
        s_DXKeyMapping[kKey_escape] = 1;
        s_DXKeyMapping[kKey_1] = 2;
        s_DXKeyMapping[kKey_2] = 3;
        s_DXKeyMapping[kKey_3] = 4;
        s_DXKeyMapping[kKey_4] = 5;
        s_DXKeyMapping[kKey_5] = 6;
        s_DXKeyMapping[kKey_6] = 7;
        s_DXKeyMapping[kKey_7] = 8;
        s_DXKeyMapping[kKey_8] = 9;
        s_DXKeyMapping[kKey_9] = 10;
        s_DXKeyMapping[kKey_0] = 11;
        s_DXKeyMapping[kKey_minus] = 12;
        s_DXKeyMapping[kKey_equals] = 13;
        s_DXKeyMapping[kKey_back] = 14;
        s_DXKeyMapping[kKey_tab] = 15;
        s_DXKeyMapping[kKey_q] = 16;
        s_DXKeyMapping[kKey_w] = 17;
        s_DXKeyMapping[kKey_e] = 18;
        s_DXKeyMapping[kKey_r] = 19;
        s_DXKeyMapping[kKey_t] = 20;
        s_DXKeyMapping[kKey_y] = 21;
        s_DXKeyMapping[kKey_u] = 22;
        s_DXKeyMapping[kKey_i] = 23;
        s_DXKeyMapping[kKey_o] = 24;
        s_DXKeyMapping[kKey_p] = 25;
        s_DXKeyMapping[kKey_lbracket] = 26;
        s_DXKeyMapping[kKey_rbracket] = 27;
        s_DXKeyMapping[kKey_return] = 28;
        s_DXKeyMapping[kKey_lcontrol] = 29;
        s_DXKeyMapping[kKey_a] = 30;
        s_DXKeyMapping[kKey_s] = 31;
        s_DXKeyMapping[kKey_d] = 32;
        s_DXKeyMapping[kKey_f] = 33;
        s_DXKeyMapping[kKey_g] = 34;
        s_DXKeyMapping[kKey_h] = 35;
        s_DXKeyMapping[kKey_j] = 36;
        s_DXKeyMapping[kKey_k] = 37;
        s_DXKeyMapping[kKey_l] = 38;
        s_DXKeyMapping[kKey_semicolon] = 39;
        s_DXKeyMapping[kKey_apostrophe] = 40;
        s_DXKeyMapping[kKey_grave] = 41;
        s_DXKeyMapping[kKey_lshift] = 42;
        s_DXKeyMapping[kKey_backslash] = 43;
        s_DXKeyMapping[kKey_z] = 44;
        s_DXKeyMapping[kKey_x] = 45;
        s_DXKeyMapping[kKey_c] = 46;
        s_DXKeyMapping[kKey_v] = 47;
        s_DXKeyMapping[kKey_b] = 48;
        s_DXKeyMapping[kKey_n] = 49;
        s_DXKeyMapping[kKey_m] = 50;
        s_DXKeyMapping[kKey_comma] = 51;
        s_DXKeyMapping[kKey_period] = 52;
        s_DXKeyMapping[kKey_slash] = 53;
        s_DXKeyMapping[kKey_rshift] = 54;
        s_DXKeyMapping[kKey_multiply] = 55;
        s_DXKeyMapping[kKey_lalt] = 56;
        s_DXKeyMapping[kKey_space] = 57;
        s_DXKeyMapping[kKey_capital] = 58;
        s_DXKeyMapping[kKey_f1] = 59;
        s_DXKeyMapping[kKey_f2] = 60;
        s_DXKeyMapping[kKey_f3] = 61;
        s_DXKeyMapping[kKey_f4] = 62;
        s_DXKeyMapping[kKey_f5] = 63;
        s_DXKeyMapping[kKey_f6] = 64;
        s_DXKeyMapping[kKey_f7] = 65;
        s_DXKeyMapping[kKey_f8] = 66;
        s_DXKeyMapping[kKey_f9] = 67;
        s_DXKeyMapping[kKey_f10] = 68;
        s_DXKeyMapping[kKey_numlock] = 69;
        s_DXKeyMapping[kKey_scroll] = 70;
        s_DXKeyMapping[kKey_numpad7] = 71;
        s_DXKeyMapping[kKey_numpad8] = 72;
        s_DXKeyMapping[kKey_numpad9] = 73;
        s_DXKeyMapping[kKey_subtract] = 74;
        s_DXKeyMapping[kKey_numpad4] = 75;
        s_DXKeyMapping[kKey_numpad5] = 76;
        s_DXKeyMapping[kKey_numpad6] = 77;
        s_DXKeyMapping[kKey_add] = 78;
        s_DXKeyMapping[kKey_numpad1] = 79;
        s_DXKeyMapping[kKey_numpad2] = 80;
        s_DXKeyMapping[kKey_numpad3] = 81;
        s_DXKeyMapping[kKey_numpad0] = 82;
        s_DXKeyMapping[kKey_decimal] = 83;
        s_DXKeyMapping[kKey_f11] = 87;
        s_DXKeyMapping[kKey_f12] = 88;
        s_DXKeyMapping[kKey_numpadenter] = 156;
        s_DXKeyMapping[kKey_rcontrol] = 157;
        s_DXKeyMapping[kKey_divide] = 181;
        s_DXKeyMapping[kKey_sysrq] = 183;
        s_DXKeyMapping[kKey_ralt] = 184;
        s_DXKeyMapping[kKey_pause] = 197;
        s_DXKeyMapping[kKey_home] = 199;
        s_DXKeyMapping[kKey_up] = 200;
        s_DXKeyMapping[kKey_pgup] = 201;
        s_DXKeyMapping[kKey_left] = 203;
        s_DXKeyMapping[kKey_right] = 205;
        s_DXKeyMapping[kKey_end] = 207;
        s_DXKeyMapping[kKey_down] = 208;
        s_DXKeyMapping[kKey_pgdn] = 209;
        s_DXKeyMapping[kKey_insert] = 210;
        s_DXKeyMapping[kKey_delete] = 211;
        s_DXKeyMapping[kKey_lwin] = 219;
        s_DXKeyMapping[kKey_rwin] = 220;
        s_DXKeyMapping[kKey_apps] = 221;
    }

    void KbmZeroInputs()
    {
        memset(&s_MouseState, 0, sizeof(DIMOUSESTATE2));
        memset(s_Keybuffer, 0, sizeof(s_Keybuffer));
    }

    void KbmInitialize()
    {
        KbmBuildKeyMapping();

        if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&s_DI, nullptr)))
            ASSERT(false, "DirectInput8 initialization failed.");

        if (FAILED(s_DI->CreateDevice(GUID_SysKeyboard, &s_Keyboard, nullptr)))
            ASSERT(false, "Keyboard CreateDevice failed.");
        if (FAILED(s_Keyboard->SetDataFormat(&c_dfDIKeyboard)))
            ASSERT(false, "Keyboard SetDataFormat failed.");
        if (FAILED(s_Keyboard->SetCooperativeLevel(D3dGraphicsCore::g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
            ASSERT(false, "Keyboard SetCooperativeLevel failed.");

        DIPROPDWORD dipdw;
        dipdw.diph.dwSize = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj = 0;
        dipdw.diph.dwHow = DIPH_DEVICE;
        dipdw.dwData = 10;
        if (FAILED(s_Keyboard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
            ASSERT(false, "Keyboard set buffer size failed.");

        if (FAILED(s_DI->CreateDevice(GUID_SysMouse, &s_Mouse, nullptr)))
            ASSERT(false, "Mouse CreateDevice failed.");
        if (FAILED(s_Mouse->SetDataFormat(&c_dfDIMouse2)))
            ASSERT(false, "Mouse SetDataFormat failed.");
        if (FAILED(s_Mouse->SetCooperativeLevel(D3dGraphicsCore::g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
            ASSERT(false, "Mouse SetCooperativeLevel failed.");

        KbmZeroInputs();
    }

    void KbmShutdown()
    {
        if (s_Keyboard)
        {
            s_Keyboard->Unacquire();
            s_Keyboard->Release();
            s_Keyboard = nullptr;
        }
        if (s_Mouse)
        {
            s_Mouse->Unacquire();
            s_Mouse->Release();
            s_Mouse = nullptr;
        }
        if (s_DI)
        {
            s_DI->Release();
            s_DI = nullptr;
        }
    }

    void KbmUpdate()
    {
        HWND foreground = GetForegroundWindow();
        bool visible = IsWindowVisible(foreground) != 0;

        if (foreground != D3dGraphicsCore::g_hWnd // wouldn't be able to acquire
            || !visible)
        {
            KbmZeroInputs();
        }
        else
        {
            s_Mouse->Acquire();
            s_Mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &s_MouseState);
            s_Keyboard->Acquire();
            s_Keyboard->GetDeviceState(sizeof(s_Keybuffer), s_Keybuffer);
        }
    }

#endif

}

void XM_Input::Initialize()
{
    ZeroMemory(s_Buttons, sizeof(s_Buttons));
    ZeroMemory(s_Analogs, sizeof(s_Analogs));

#ifdef USE_KEYBOARD_MOUSE
    KbmInitialize();
#endif
}

void XM_Input::Shutdown()
{
#ifdef USE_KEYBOARD_MOUSE
    KbmShutdown();
#endif
}

void XM_Input::Update(float frameDelta)
{
    memcpy(s_Buttons[1], s_Buttons[0], sizeof(s_Buttons[0]));
    memset(s_Buttons[0], 0, sizeof(s_Buttons[0]));
    memset(s_Analogs, 0, sizeof(s_Analogs));

#ifdef _GAMING_DESKTOP

#define SET_BUTTON_VALUE(InputEnum, GameInputMask) \
        s_Buttons[0][InputEnum] = !!(newInputState.Gamepad.wButtons & GameInputMask);

    XINPUT_STATE newInputState;
    if (ERROR_SUCCESS == XInputGetState(0, &newInputState))
    {
        SET_BUTTON_VALUE(kDPadUp, XINPUT_GAMEPAD_DPAD_UP);
        SET_BUTTON_VALUE(kDPadDown, XINPUT_GAMEPAD_DPAD_DOWN);
        SET_BUTTON_VALUE(kDPadLeft, XINPUT_GAMEPAD_DPAD_LEFT);
        SET_BUTTON_VALUE(kDPadRight, XINPUT_GAMEPAD_DPAD_RIGHT);
        SET_BUTTON_VALUE(kStartButton, XINPUT_GAMEPAD_START);
        SET_BUTTON_VALUE(kBackButton, XINPUT_GAMEPAD_BACK);
        SET_BUTTON_VALUE(kLThumbClick, XINPUT_GAMEPAD_LEFT_THUMB);
        SET_BUTTON_VALUE(kRThumbClick, XINPUT_GAMEPAD_RIGHT_THUMB);
        SET_BUTTON_VALUE(kLShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER);
        SET_BUTTON_VALUE(kRShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER);
        SET_BUTTON_VALUE(kAButton, XINPUT_GAMEPAD_A);
        SET_BUTTON_VALUE(kBButton, XINPUT_GAMEPAD_B);
        SET_BUTTON_VALUE(kXButton, XINPUT_GAMEPAD_X);
        SET_BUTTON_VALUE(kYButton, XINPUT_GAMEPAD_Y);

        s_Analogs[kAnalogLeftTrigger] = newInputState.Gamepad.bLeftTrigger / 255.0f;
        s_Analogs[kAnalogRightTrigger] = newInputState.Gamepad.bRightTrigger / 255.0f;
        s_Analogs[kAnalogLeftStickX] = FilterAnalogInput(newInputState.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        s_Analogs[kAnalogLeftStickY] = FilterAnalogInput(newInputState.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        s_Analogs[kAnalogRightStickX] = FilterAnalogInput(newInputState.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        s_Analogs[kAnalogRightStickY] = FilterAnalogInput(newInputState.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    }
#else
    IGameInputReading* pGIR = nullptr;
    if (s_pGameInput != nullptr)
        s_pGameInput->GetCurrentReading(GameInputKindGamepad, nullptr, &pGIR);
    bool IsGamepadPresent = (pGIR != nullptr);

    if (IsGamepadPresent)
    {
        GameInputGamepadState newInputState;
        pGIR->GetGamepadState(&newInputState);
        pGIR->Release();

#define SET_BUTTON_VALUE(InputEnum, GameInputMask) \
        s_Buttons[0][InputEnum] = !!(newInputState.buttons & GameInputMask);

        SET_BUTTON_VALUE(kDPadUp, GameInputGamepadDPadUp);
        SET_BUTTON_VALUE(kDPadDown, GameInputGamepadDPadDown);
        SET_BUTTON_VALUE(kDPadLeft, GameInputGamepadDPadLeft);
        SET_BUTTON_VALUE(kDPadRight, GameInputGamepadDPadRight);
        SET_BUTTON_VALUE(kStartButton, GameInputGamepadMenu);
        SET_BUTTON_VALUE(kBackButton, GameInputGamepadView);
        SET_BUTTON_VALUE(kLThumbClick, GameInputGamepadLeftThumbstick);
        SET_BUTTON_VALUE(kRThumbClick, GameInputGamepadRightThumbstick);
        SET_BUTTON_VALUE(kLShoulder, GameInputGamepadLeftShoulder);
        SET_BUTTON_VALUE(kRShoulder, GameInputGamepadRightShoulder);
        SET_BUTTON_VALUE(kAButton, GameInputGamepadA);
        SET_BUTTON_VALUE(kBButton, GameInputGamepadB);
        SET_BUTTON_VALUE(kXButton, GameInputGamepadX);
        SET_BUTTON_VALUE(kYButton, GameInputGamepadY);

        s_Analogs[kAnalogLeftTrigger] = newInputState.leftTrigger;
        s_Analogs[kAnalogRightTrigger] = newInputState.rightTrigger;
        s_Analogs[kAnalogLeftStickX] = FilterAnalogInput(newInputState.leftThumbstickX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        s_Analogs[kAnalogLeftStickY] = FilterAnalogInput(newInputState.leftThumbstickY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        s_Analogs[kAnalogRightStickX] = FilterAnalogInput(newInputState.rightThumbstickX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        s_Analogs[kAnalogRightStickY] = FilterAnalogInput(newInputState.rightThumbstickY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
    }
#endif

#ifdef USE_KEYBOARD_MOUSE
    KbmUpdate();

    for (uint32_t i = 0; i < kNumKeys; ++i)
    {
        s_Buttons[0][i] = (s_Keybuffer[s_DXKeyMapping[i]] & 0x80) != 0;
    }

    for (uint32_t i = 0; i < 8; ++i)
    {
        if (s_MouseState.rgbButtons[i] > 0) s_Buttons[0][kMouse0 + i] = true;
    }

    s_Analogs[kAnalogMouseX] = (float)s_MouseState.lX * .0018f;
    s_Analogs[kAnalogMouseY] = (float)s_MouseState.lY * -.0018f;

    if (s_MouseState.lZ > 0)
        s_Analogs[kAnalogMouseScroll] = 1.0f;
    else if (s_MouseState.lZ < 0)
        s_Analogs[kAnalogMouseScroll] = -1.0f;
#endif

    // Update time duration for buttons pressed
    for (uint32_t i = 0; i < kNumDigitalInputs; ++i)
    {
        if (s_Buttons[0][i])
        {
            if (!s_Buttons[1][i])
                s_HoldDuration[i] = 0.0f;
            else
                s_HoldDuration[i] += frameDelta;
        }
    }

    for (uint32_t i = 0; i < kNumAnalogInputs; ++i)
    {
        s_AnalogsTC[i] = s_Analogs[i] * frameDelta;
    }

}

bool XM_Input::IsAnyPressed(void)
{
    return s_Buttons[0] != 0;
}

bool XM_Input::IsPressed(DigitalInput di)
{
    return s_Buttons[0][di];
}

bool XM_Input::IsFirstPressed(DigitalInput di)
{
    return s_Buttons[0][di] && !s_Buttons[1][di];
}

bool XM_Input::IsReleased(DigitalInput di)
{
    return !s_Buttons[0][di];
}

bool XM_Input::IsFirstReleased(DigitalInput di)
{
    return !s_Buttons[0][di] && s_Buttons[1][di];
}

float XM_Input::GetDurationPressed(DigitalInput di)
{
    return s_HoldDuration[di];
}

float XM_Input::GetAnalogInput(AnalogInput ai)
{
    return s_Analogs[ai];
}

float XM_Input::GetTimeCorrectedAnalogInput(AnalogInput ai)
{
    return s_AnalogsTC[ai];
}