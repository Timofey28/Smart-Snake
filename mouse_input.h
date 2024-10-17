#pragma once

#include <windows.h>

extern DWORD prev_mode;


enum ButtonPressed
{
    CTRL_LEFT,
    CTRL_RIGHT,
    LEFT_BUTTON,
    RIGHT_BUTTON,
    WHEEL,
};


class MouseInput
{
public:
    MouseInput() : handle_(GetStdHandle(STD_INPUT_HANDLE)) {}
    void GetClickInfo();
    void GetAnyClick();

    short X, Y;
    ButtonPressed buttonPressed;

private:
    void __EnableMouseInput();

    HANDLE handle_;
    INPUT_RECORD inputRecord_;
    DWORD events_;
};
