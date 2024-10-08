#include <iostream>
#include <windows.h>
#include "application.h"

using namespace std;

int nConsoleWidth, nConsoleHeight;
DWORD prev_mode;

void configureConsole();
void getConsoleWH();

void setColor(int color)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD) color);
}

int main()
{
    setlocale(0, "");

    configureConsole();
    getConsoleWH();

    Application application;
    application.Run();

//    for (int i = 0; i < 16; ++i) {
//        setColor(15);
//        cout << "\n\t" << i << " --> ";
//        if (i < 10) cout << " ";
//        setColor(17 * i);
//        cout << ".....";
//    }
//    cout << "\n";
//    setColor(15);
}


void configureConsole()
{
    int suitableFontSize = 40;

    // ��������� ������� ������ � �������
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof(fontInfo);
    GetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo);
    fontInfo.dwFontSize.Y = suitableFontSize;
    SetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo);

    // ��������� ������� ������ ������ ������ �������� ������� ����
    getConsoleWH();
    COORD newScreenBufferSize;
    newScreenBufferSize.X = ::nConsoleWidth;
    newScreenBufferSize.Y = ::nConsoleHeight;
    if(!SetConsoleScreenBufferSize(hConsole, newScreenBufferSize)) {
        cout << "\nError in " << to_string(__LINE__) << " line: " << GetLastError();
        exit(-1);
    }

    // �������� ������� �� ���� �����
    ::SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);

    // ������� ������ ���������
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(hConsole, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &structCursorInfo);

    // ������ �������� ������ �������
    GetConsoleMode(hConsole, &::prev_mode);
}

void getConsoleWH()
{
    HANDLE hWndConsole;
    if (hWndConsole = GetStdHandle(-12))
    {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo))
        {
            ::nConsoleWidth = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
            ::nConsoleHeight = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
        }
        else exit(-1);
    }
    else exit(-1);
}
