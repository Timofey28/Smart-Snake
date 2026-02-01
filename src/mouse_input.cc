#include "mouse_input.h"

bool MouseInput::isKeyboardEvent;
short MouseInput::X;
short MouseInput::Y;
ButtonPressed MouseInput::buttonPressed;
HANDLE MouseInput::handle_;
INPUT_RECORD MouseInput::inputRecord_;
DWORD MouseInput::events_;

void MouseInput::GetAnyEventInfo(bool flushBuffer)
{
    MouseInput::__EnableMouseInput();
    if (flushBuffer) FlushConsoleInputBuffer(handle_);

    while (true) {
        ReadConsoleInput(handle_, &inputRecord_, 1, &events_);
        if (inputRecord_.EventType == MOUSE_EVENT) {
            MouseInput::isKeyboardEvent = false;
            MOUSE_EVENT_RECORD mer = inputRecord_.Event.MouseEvent;
            MouseInput::X = mer.dwMousePosition.X;
            MouseInput::Y = mer.dwMousePosition.Y;

            if (mer.dwEventFlags & MOUSE_WHEELED) {
                short wheelDelta = HIWORD(mer.dwButtonState);
                assert(wheelDelta != 0);

                if (wheelDelta > 0) MouseInput::buttonPressed = ButtonPressed::WHEEL_UP;
                else MouseInput::buttonPressed = ButtonPressed::WHEEL_DOWN;
                return;
            }
            else if (mer.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) {
                if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                    MouseInput::buttonPressed = ButtonPressed::CTRL_LEFT;
                    return;
                }
                else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
                    MouseInput::buttonPressed = ButtonPressed::CTRL_RIGHT;
                    return;
                }
            }
            else if (mer.dwButtonState == MOUSE_WHEELED) {
                MouseInput::buttonPressed = ButtonPressed::WHEEL;
                return;
            }
            else if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                MouseInput::buttonPressed = ButtonPressed::LEFT_BUTTON;
                return;
            }
            else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
                MouseInput::buttonPressed = ButtonPressed::RIGHT_BUTTON;
                return;
            }
        }

        else if (inputRecord_.EventType == KEY_EVENT) {
            KEY_EVENT_RECORD ker = inputRecord_.Event.KeyEvent;

            if (ker.bKeyDown) { // Обрабатываем только нажатия, а не отпускания
                MouseInput::isKeyboardEvent = true;
                switch (ker.wVirtualKeyCode) {
                    case VK_LEFT:
                        MouseInput::buttonPressed = ButtonPressed::ARROW_LEFT;
                        return;

                    case VK_RIGHT:
                        MouseInput::buttonPressed = ButtonPressed::ARROW_RIGHT;
                        return;

                    case VK_UP:
                        MouseInput::buttonPressed = ButtonPressed::ARROW_UP;
                        return;

                    case VK_DOWN:
                        MouseInput::buttonPressed = ButtonPressed::ARROW_DOWN;
                        return;

                    case VK_BACK:
                        MouseInput::buttonPressed = ButtonPressed::BACKSPACE;
                        return;

                    case VK_ESCAPE:
                        MouseInput::buttonPressed = ButtonPressed::ESCAPE;
                        return;

                    case VK_RETURN:
                        MouseInput::buttonPressed = ButtonPressed::ENTER;
                        return;

                    case VK_CONTROL:
                        MouseInput::buttonPressed = ButtonPressed::CTRL;
                        return;

                    case VK_SPACE:
                        MouseInput::buttonPressed = ButtonPressed::WHITESPACE;
                        return;

                    case '0':
                    case VK_NUMPAD0:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_0;
                        return;

                    case '1':
                    case VK_NUMPAD1:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_1;
                        return;

                    case '2':
                    case VK_NUMPAD2:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_2;
                        return;

                    case '3':
                    case VK_NUMPAD3:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_3;
                        return;

                    case '4':
                    case VK_NUMPAD4:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_4;
                        return;

                    case '5':
                    case VK_NUMPAD5:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_5;
                        return;

                    case '6':
                    case VK_NUMPAD6:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_6;
                        return;

                    case '7':
                    case VK_NUMPAD7:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_7;
                        return;

                    case '8':
                    case VK_NUMPAD8:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_8;
                        return;

                    case '9':
                    case VK_NUMPAD9:
                        MouseInput::buttonPressed = ButtonPressed::DIGIT_9;
                        return;
                }
            }
        }
    }
}

void MouseInput::GetClickInfo()
{
    MouseInput::__EnableMouseInput();

    while (true) {
        ReadConsoleInput(handle_, &inputRecord_, 1, &events_);
        if (inputRecord_.EventType == MOUSE_EVENT) {
            MOUSE_EVENT_RECORD mer = inputRecord_.Event.MouseEvent;
            MouseInput::X = mer.dwMousePosition.X;
            MouseInput::Y = mer.dwMousePosition.Y;

            if (mer.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) {
                if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                    MouseInput::buttonPressed = ButtonPressed::CTRL_LEFT;
                    return;
                }
                else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
                    MouseInput::buttonPressed = ButtonPressed::CTRL_RIGHT;
                    return;
                }
            }
            else if (mer.dwButtonState == MOUSE_WHEELED) {
                MouseInput::buttonPressed = ButtonPressed::WHEEL;
                return;
            }
            else if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                MouseInput::buttonPressed = ButtonPressed::LEFT_BUTTON;
                return;
            }
            else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
                MouseInput::buttonPressed = ButtonPressed::RIGHT_BUTTON;
                return;
            }
        }
        else if (inputRecord_.EventType == KEY_EVENT) {
            KEY_EVENT_RECORD ker = inputRecord_.Event.KeyEvent;
            if (ker.bKeyDown && ker.wVirtualKeyCode == VK_ESCAPE) {
                MouseInput::buttonPressed = ButtonPressed::ESCAPE;
                return;
            }
        }
    }
}

void MouseInput::WaitForAnyEvent()
{
    MouseInput::__EnableMouseInput();
    FlushConsoleInputBuffer(handle_);

    while (true) {
        ReadConsoleInput(handle_, &inputRecord_, 1, &events_);
        if (inputRecord_.EventType == KEY_EVENT) return;
        if (inputRecord_.EventType == MOUSE_EVENT) {
            MOUSE_EVENT_RECORD mer = inputRecord_.Event.MouseEvent;
            if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED ||
                mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED ||
                mer.dwButtonState == MOUSE_WHEELED)
            {
                return;
            }
        }
    }
}

void MouseInput::__EnableMouseInput()
{
    SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS | (Console::s_previousMode & ~ENABLE_QUICK_EDIT_MODE));
    SetConsoleMode(handle_, ENABLE_MOUSE_INPUT);
}
