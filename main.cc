#include <iostream>
#include <windows.h>
#include <stdexcept>
#include "application.h"
#include "utils.h"

#include <bits/stdc++.h>
namespace fs = std::filesystem;

using namespace std;

int nConsoleWidth, nConsoleHeight;
DWORD prev_mode;

void initializeBase93Map();
void configureConsole();
void getConsoleWH();

void setColor(int color)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hStdOut, (WORD) color);
}

/*
1) эксперимент
2) сессия
3) тест
4) серия
5) симуляция
*/

int main()
{
    setlocale(0, "");
    SetConsoleCP(866);
    SetConsoleOutputCP(866);

    initializeBase93Map();
    configureConsole();
    getConsoleWH();

    Application application;
    application.Run();

//    std::mt19937 generator(1);
//    uniform_int_distribution<int> uid{0, 5};
//    cout << uid(generator) << "\n";
//    uniform_int_distribution<int> uid2{0, 7};
//    cout << uid2(generator);

//    _setmode(_fileno(stdout), _O_U16TEXT);
//    wcout << (wchar_t) 0x2591 << ' ' << (wchar_t) 0x2592 << ' ' << (wchar_t) 0x2593 << "\n";
//    wcout << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2500 << "\n";
//    wcout << (wchar_t) 0x2502 << "\n" << (wchar_t) 0x2502 << "\n" << (wchar_t) 0x2502 << "\n";
//    wcout << (wchar_t) 0x250C << (wchar_t) 0x2510 << "\n";
//    wcout << (wchar_t) 0x2514 << (wchar_t) 0x2518 << "\n";
//    _setmode(_fileno(stdout), _O_TEXT);

//    auto start = chrono::high_resolution_clock::now();
//    this_thread::sleep_for(1528ms);
//    auto end = chrono::high_resolution_clock::now();
//    chrono::duration<double> duration = end - start;
//    cout << "Время выполнения: " << round(duration.count() * 100) / 100 << " секунд" << std::endl;

//    fs::path CONSTANT_PATH = "Constant-Path";
//    int filesAmount = 1;
//    fs::path p = CONSTANT_PATH / (to_string(filesAmount) + ".txt");
//    cout << p;

//    for (int i = 100; ~i; --i) {
//        string s = to_string(i);
//        cout << "\rпривет: " << i << flush;
//        this_thread::sleep_for(50ms);
//    }

//    auto now = chrono::system_clock::now();
//    time_t now_time = chrono::system_clock::to_time_t(now);
//    cout << now_time;
//    ostringstream oss;
//    oss << put_time(localtime(&now_time), "%Y-%m-%d %H:%M:%S");
//    string humanTime = oss.str();
//    cout << "\nТекущая дата и время: " << humanTime << std::endl;

//    fs::path a = "a";
//    fs::path b = "b";
//    int i = 1;
//    fs::create_directories(a / b / to_string(i));

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

    // установка размера шрифта в консоли
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof(fontInfo);
    GetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo);
    fontInfo.dwFontSize.Y = suitableFontSize;
    SetCurrentConsoleFontEx(hConsole, TRUE, &fontInfo);

    // установка размера буфера экрана равным текущему размеру окна
    getConsoleWH();
    COORD newScreenBufferSize;
    newScreenBufferSize.X = ::nConsoleWidth;
    newScreenBufferSize.Y = ::nConsoleHeight;
    if(!SetConsoleScreenBufferSize(hConsole, newScreenBufferSize)) {
        string errorMsg = "Unable to set console screen buffer size: " + to_string(GetLastError());
        throw runtime_error(errorMsg.c_str());
    }

    // открытие консоли во весь экран
    ::SendMessage(::GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);

    // сделать курсор невидимым
    CONSOLE_CURSOR_INFO structCursorInfo;
    GetConsoleCursorInfo(hConsole, &structCursorInfo);
    structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &structCursorInfo);

    // чтение текущего режима консоли
    GetConsoleMode(hConsole, &::prev_mode);
}

void getConsoleWH()
{
    HANDLE hWndConsole;
    if (hWndConsole = GetStdHandle(-12)) {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo)) {
            ::nConsoleWidth = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
            ::nConsoleHeight = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
        }
        else throw runtime_error("Unable to get console screen buffer info");
    }
    else throw runtime_error("Unable to initialize HANDLE object");
}
