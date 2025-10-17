#include "console.h"
using namespace std;

int Console::s_currentFontSize, Console::s_pointOfNoReturn;
Dimensions Console::s_dimensions(-1, -1);
map<int, Dimensions> Console::s_fontSizeToDimensions;
HANDLE Console::s_handle = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD Console::s_previousMode;

void Console::Initialize()
{
//    auto start = std::chrono::high_resolution_clock::now();

    int minFont = round(s_startingFontSize * 0.75);
    if (minFont % 2 != s_startingFontSize % 2) minFont--;
    __SetOrUnsetFullscreen();
    for (int fontSize = minFont; fontSize <= s_startingFontSize; fontSize += 2) {
        SetFontSize(fontSize, false);
        s_fontSizeToDimensions.emplace(fontSize, s_dimensions);
    }
    __MakeCursorInvisible();
    GetConsoleMode(s_handle, &s_previousMode);

//    for (auto it = s_fontSizeToDimensions.begin(); it != s_fontSizeToDimensions.end(); ++it) {
//        cout << "\n" << it->first << ": " << it->second.width << 'x' << it->second.height << "  (" <<
//        doubleToStr((double) it->second.width / it->second.height) << ")";
//    }
//    auto end = std::chrono::high_resolution_clock::now();
//    std::chrono::duration<double> duration = end - start;
//    cout << "\n" << duration; _getch();
//    _getch();
//
//    SetFontSize(minFont); _getch();
//    SetFontSize(s_startingFontSize); _getch();
}

void Console::SetFontSize(int newFontSize, bool pause)
{
    __SetOrUnsetFullscreen();
    __SetFontSize(newFontSize);
    if (pause) this_thread::sleep_for(1ms);
    __GetConsoleDimensions();
    __CorrectBufferSize();
    __SetOrUnsetFullscreen();
    __GetConsoleDimensions();
    __MakeCursorInvisible();
}

void Console::__GetConsoleDimensions()
{
    HANDLE hWndConsole;
    if (hWndConsole = GetStdHandle(-12)) {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo)) {
            s_dimensions.width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
            s_dimensions.height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
            s_pointOfNoReturn = s_dimensions.width / 2 * s_dimensions.height - 1;
        }
        else throw runtime_error("Unable to get console screen buffer info");
    }
    else throw runtime_error("Unable to initialize HANDLE object");
}

void Console::__CorrectBufferSize()
{
    assert(s_dimensions.width != -1);
    COORD newScreenBufferSize;
    newScreenBufferSize.X = s_dimensions.width;
    newScreenBufferSize.Y = s_dimensions.height;
    if(!SetConsoleScreenBufferSize(s_handle, newScreenBufferSize)) {
        string errorMsg = "Unable to set console screen buffer size: " + to_string(GetLastError());
        throw runtime_error(errorMsg.c_str());
    }
}

void Console::__SetOrUnsetFullscreen()
{
    SendMessage(GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);
}

void Console::__MakeCursorInvisible()
{
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(s_handle, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(s_handle, &structCursorInfo);
}

void Console::__SetFontSize(int newFontSize)
{
    if (newFontSize == s_currentFontSize) return;
    HANDLE s_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof(fontInfo);
    GetCurrentConsoleFontEx(s_handle, TRUE, &fontInfo);
    fontInfo.dwFontSize.Y = newFontSize;
    SetCurrentConsoleFontEx(s_handle, TRUE, &fontInfo);
    s_currentFontSize = newFontSize;
}
