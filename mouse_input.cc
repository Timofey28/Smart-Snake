#include "mouse_input.h"


void MouseInput::GetClickInfo()
{
    this->__EnableMouseInput();

    while (true) {
        ReadConsoleInput(handle_, &inputRecord_, 1, &events_);
        if (inputRecord_.EventType == MOUSE_EVENT) {
            MOUSE_EVENT_RECORD mer = inputRecord_.Event.MouseEvent;
            this->X = mer.dwMousePosition.X;
            this->Y = mer.dwMousePosition.Y;

            if (mer.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) {
                if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                    this->buttonPressed = ButtonPressed::CTRL_LEFT;
                    return;
                }
                else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
                    this->buttonPressed = ButtonPressed::CTRL_RIGHT;
                    return;
                }
            }
            else if (mer.dwButtonState == MOUSE_WHEELED) {
                this->buttonPressed = ButtonPressed::WHEEL;
                return;
            }
            else if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                this->buttonPressed = ButtonPressed::LEFT;
                return;
            }
            else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
                this->buttonPressed = ButtonPressed::RIGHT;
                return;
            }
        }
    }
}

void MouseInput::GetAnyClick()
{
    this->__EnableMouseInput();
    ReadConsoleInput(handle_, &inputRecord_, 1, &events_);
}

void MouseInput::__EnableMouseInput()
{
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS | (::prev_mode & ~ENABLE_QUICK_EDIT_MODE));
    SetConsoleMode(handle_, ENABLE_MOUSE_INPUT);
}
