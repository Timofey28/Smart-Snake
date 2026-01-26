#pragma once

#include <windows.h>
#include <cassert>

#include "console.h"


enum ButtonPressed
{
    // Mouse
    LEFT_BUTTON,
    RIGHT_BUTTON,
    CTRL_LEFT,
    CTRL_RIGHT,
    WHEEL,
    WHEEL_UP,
    WHEEL_DOWN,

    // Keyboard
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    BACKSPACE,
    ESCAPE,
    ENTER,
    CTRL,
    WHITESPACE,
    DIGIT_0, DIGIT_1, DIGIT_2, DIGIT_3, DIGIT_4,
    DIGIT_5, DIGIT_6, DIGIT_7, DIGIT_8, DIGIT_9,
};


class MouseInput
{
public:
    static void Initialize() { handle_ = GetStdHandle(STD_INPUT_HANDLE); }
    static void GetClickInfo();
    static void WaitForAnyEvent();
    static void GetAnyEventInfo(bool flushBuffer = true);

    static bool isKeyboardEvent;
    static short X, Y;
    static ButtonPressed buttonPressed;

private:
    static HANDLE handle_;
    static INPUT_RECORD inputRecord_;
    static DWORD events_;

    static void __EnableMouseInput();
};
