#include <iostream>
#include <windows.h>
#include <stdexcept>
#include "application.h"
#include "file_handler.h"
#include "mouse_input.h"
#include "utils.h"

#include <bits/stdc++.h>
#include "experiment.h"
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


int main()
{
    setlocale(0, "");
    SetConsoleCP(866);
    SetConsoleOutputCP(866);

    FileHandler::Initialize();
    MouseInput::Initialize();
    initializeBase93Map();
    configureConsole();
    getConsoleWH();

    Application application;
    application.Run();

//    Experiment e = Experiment("Games/2025-09-15/1");
//    cout << e.creationTime << ' ' << timestampToHourMinuteStr(e.creationTime) << "\n";
//    cout << "Количество игр: " << e.gamesAmount << "\n";
//    cout << "Поле " << e.fieldWidth << "x" << e.fieldHeight << "\n";
//    cout << "Нач. длина змеи: " << e.initialSnakeLength << "\n";
//    cout << "Макс. длина змеи: " << e.maxPossibleSnakeLength << "\n";
//    cout << "Результаты игр: "; for (int i = 0; i < e.gamesAmount; ++i) cout << e.gameScores[i] << ' '; cout << "\n";
//    cout << "Лучший: " << e.bestScore << "\nСреднее: " << e.avgScore << " (" << doubleToStr(e.avgScore) << ")\n";

//    // Проверка одновременного чтения мыши и клавиатуры
//    MouseInput::Initialize();
//    char left = 27, right = 26, up = 24, down = 25, backspace = 17;
//    while (true) {
//        MouseInput::GetAnyEventInfo();
//        short x = MouseInput::X, y = MouseInput::Y;
//        if (MouseInput::isKeyboardEvent) {
//            switch (MouseInput::buttonPressed) {
//                case ButtonPressed::ARROW_LEFT: cout << left; break;
//                case ButtonPressed::ARROW_RIGHT: cout << right; break;
//                case ButtonPressed::ARROW_UP: cout << up; break;
//                case ButtonPressed::ARROW_DOWN: cout << down; break;
//                case ButtonPressed::BACKSPACE: cout << backspace; break;
//                case ButtonPressed::ESCAPE: cout << "Esc"; break;
//                case ButtonPressed::ENTER: cout << "Enter"; break;
//            }
//            cout << "\n";
//        }
//        else {
//            cout << "X=" << MouseInput::X << " Y=" << MouseInput::Y;
//            switch (MouseInput::buttonPressed) {
//                case ButtonPressed::LEFT_BUTTON: cout << " левая\n"; break;
//                case ButtonPressed::RIGHT_BUTTON: cout << " правая\n"; break;
//                case ButtonPressed::CTRL_LEFT: cout << " ctrl+левая\n"; break;
//                case ButtonPressed::CTRL_RIGHT: cout << " ctrl+правая\n"; break;
//                case ButtonPressed::WHEEL: cout << " колесико\n"; break;
//                case ButtonPressed::WHEEL_UP: cout << " колесико вверх, x=" << x << " y=" << y << "\n"; break;
//                case ButtonPressed::WHEEL_DOWN: cout << " колесико вниз, x=" << x << " y=" << y << "\n"; break;
//            }
//        }
//    }

//    string dateStr = "2025-09-10";
//    tm tm_struct = {};
//    istringstream ss(dateStr);
//    ss >> get_time(&tm_struct, "%Y-%m-%d");
//    time_t timestamp = mktime(&tm_struct);
//    cout << timestamp;

//    std::mt19937 generator(1);
//    uniform_int_distribution<int> uid{0, 5};
//    cout << uid(generator) << "\n";
//    uniform_int_distribution<int> uid2{0, 7};
//    cout << uid2(generator);

//    _setmode(_fileno(stdout), _O_U16TEXT);
////    wcout << (wchar_t) 0x2591 << ' ' << (wchar_t) 0x2592 << ' ' << (wchar_t) 0x2593 << "\n";  // shadows
//    wcout << (wchar_t) 0x250F << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x2513 << "\n";
//    wcout << (wchar_t) 0x2503 << "   " << (wchar_t) 0x2503 << "\n";
//    wcout << (wchar_t) 0x2523 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x252B << "\n";
//    wcout << (wchar_t) 0x2517 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x251B << "\n";
//
//    wcout << (wchar_t) 0x250C << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2510 << "\n";
//    wcout << (wchar_t) 0x2502 << "   " << (wchar_t) 0x2502 << "\n";
//    wcout << (wchar_t) 0x251C << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2524 << "\n";
//    wcout << (wchar_t) 0x2514 << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2518 << "\n";
//
////    wcout << (wchar_t) 0x2015 << (wchar_t) 0x2015 << (wchar_t) 0x2015 << "\n";
////    wcout << (wchar_t) 0x2502 << "   " << (wchar_t) 0x2502 << "\n";
////    wcout << (wchar_t) 0x2502 << "   " << (wchar_t) 0x2502 << "\n";
////    wcout << (wchar_t) 0x2502 << "   " << (wchar_t) 0x2502 << "\n";
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
