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

//    int scrollbarHeight = 23,
//        scrollbarSteps_ = 989;
//    int _listHeight = scrollbarHeight - 2, _steps = scrollbarSteps_ - 2;
//    map<int, int> pileIndexToSliderPos_;
//    int _bigFreqCount = _steps % _listHeight;
//    int _smallFrequency = _steps / _listHeight;
//    int _bigFreqStart = _listHeight / 2 - _bigFreqCount / 2;
//    int _bigFreqEnd = _bigFreqStart + _bigFreqCount - 1;
//    for (int i = 0; i < _bigFreqStart * _smallFrequency; ++i) pileIndexToSliderPos_[i + 1] = i / _smallFrequency + 1;
//    int _offset = _bigFreqStart * _smallFrequency;
//    for (int i = 0; i < (_bigFreqEnd - _bigFreqStart + 1) * (_smallFrequency + 1); ++i)
//        pileIndexToSliderPos_[i + _offset + 1] = i / (_smallFrequency + 1) + _bigFreqStart + 1;
//    _offset += (_bigFreqEnd - _bigFreqStart + 1) * (_smallFrequency + 1);
//    for (int i = 0; i < (_listHeight - 1 - _bigFreqEnd) * _smallFrequency; ++i)
//        pileIndexToSliderPos_[i + _offset + 1] = i / _smallFrequency + (_bigFreqEnd + 1) + 1;
//    cout << "0: 0\n1: 1-";
//    int lastValue = 1;
//    auto it = ++pileIndexToSliderPos_.begin();
//    for (; it != pileIndexToSliderPos_.end(); ++it) {
//        auto prev = it; prev--;
//        if (prev->second != it->second) {
//            cout<<prev->first<<" ("<<it->first-lastValue<< ")\n" << it->second << ": " << it->first << "-";
//            lastValue = it->first;}
//        auto next = it; next++;
//        if (next == pileIndexToSliderPos_.end()) cout << it->first;
//    } cout<<" ("<<(--it)->first-lastValue+1<<")\n"<<scrollbarHeight-1<<": "<<scrollbarSteps_-1<<"\n";

//    int i = 417, j = 848;
//    int w = 50;
//    cout << toBase93(i % w) << toBase93(i / w) << ' ' << toBase93(j % w) << toBase93(j / w);

//    vector<int> v = {6, 7, 8, 7, 9};
//    auto it = find(v.rbegin(), v.rend(), 7);
//    assert(it != v.rend());
//    int i = v.size() - distance(v.rbegin(), it);
//    cout << i;

//    vector<int> v = {23, 26, 15, 21, 25, 30, 23, 8, 8, 22, 28, 23, 12, 30, 29, 14, 5, 15, 19, 27, 19, 24, 29, 30, 18, 26, 14, 34, 20, 13, 13, 26, 19, 16, 26, 28, 19, 12, 3, 36, 27, 27, 19, 15, 14, 10, 28, 18, 20, 21, 1, 20, 15, 26, 22, 22, 15, 39, 23, 24, 17, 31, 15, 17, 14, 2, 9, 14, 28, 6, 41, 26, 17, 17, 34, 5, 30, 24, 20, 10, 22, 10, 7, 17, 28, 35, 15, 28, 11, 40, 26, 26, 6, 13, 36, 23, 17, 31, 18, 25, 26, 19, 29, 30, 23, 36, 13, 15, 17, 24, 28, 17, 18, 26, 6, 33, 24, 15, 20, 30, 30, 26, 16, 16, 14, 30, 23, 3, 18, 12, 25, 23, 11, 33, 23, 14, 19, 9, 38, 29, 2, 16, 17, 18, 16, 3, 14, 27, 20, 37, 8, 23, 19, 25, 18, 7, 28, 25, 19, 8, 11, 32, 25, 10, 27, 26, 16, 18, 14, 31, 25, 20, 18, 23, 1, 5, 15, 20, 7, 27, 20, 31, 11, 8, 5, 17, 12, 27, 28, 18, 10, 29, 12, 5, 14, 13, 19, 24, 29, 16, 1, 20, 14, 18, 21, 23, 6, 21, 30, 28, 13, 29, 26, 30, 25, 26, 4, 7, 25, 4, 20, 7, 21, 30, 5, 18, 2, 5, 27, 38, 40, 13, 21, 18, 10, 15, 21, 26, 6, 22, 11, 27, 11, 23, 18, 25, 27, 15, 18, 25, 24, 11, 13, 30, 20, 29, 32, 21, 18, 31, 18, 6, 33, 35, 5, 25, 3, 20, 23, 18, 25, 43, 19, 27, 26, 15, 17, 6, 22, 35, 18, 19, 6, 21, 13, 20, 2, 1, 28, 22, 31, 21, 30, 26, 10, 16, 20, 11, 19, 28, 23, 15, 15, 26, 17, 24, 22, 16, 14, 14, 9, 15, 16, 16, 12, 7, 7, 28, 31, 17, 19, 12, 32, 16, 28, 14, 32, 39, 42, 14, 29, 15, 31, 27, 24, 29, 20, 19, 19, 16, 4, 20, 23, 13, 11, 16, 19, 30, 27, 32, 27, 31, 20, 23, 18, 25, 29, 24, 16, 24, 27, 9, 24, 19, 26, 12, 12, 33, 13, 22, 1, 31, 3, 23, 12, 13, 6, 31, 14, 26, 22, 14, 28, 8, 22, 4, 22, 15, 5, 32, 22, 30, 29, 16, 25, 30, 14, 33, 46, 15, 14, 17, 24, 7, 24, 10, 14, 21, 16, 5, 14, 19, 9, 10, 17, 19, 38, 12, 20, 21, 20, 13, 21, 9, 37, 29, 10, 25, 7, 28, 27, 26, 25, 8, 4, 33, 31, 27, 1, 31, 27, 26, 22, 34, 13, 21, 24, 22, 1, 10, 30, 25, 33, 12, 15, 11, 10, 15, 6, 14, 16, 25, 7, 4, 14, 27, 16, 29, 24, 16, 5, 19, 18, 21, 28, 19, 38, 18, 19, 30, 24, 22, 29, 21, 6, 8, 9, 21, 22, 29, 31, 25, 21, 8, 13, 18, 30, 12, 12, 26, 6, 25, 11, 18, 22, 24, 22, 6, 16, 22, 28, 28, 18, 14, 31, 11, 21, 30, 24, 13, 14, 13, 24, 9, 12, 37, 38, 20, 22, 20, 13, 21, 14, 12, 18, 3, 21, 21, 17, 26, 24, 18, 29, 24, 21, 26, 8, 20, 22, 28, 22, 17, 22, 30, 28, 22, 20, 18, 18, 16, 18, 15, 22, 26, 25, 11, 27, 19, 4, 34, 9, 15, 31, 17, 17, 14, 25, 10, 28, 25, 20, 7, 13, 4, 18, 16, 18, 24, 7, 21, 13, 26, 13, 21, 11, 32, 13, 6, 18, 30, 22, 35, 31, 23, 14, 23, 18, 39, 42, 18, 25, 11, 26, 5, 16, 18, 29, 9, 31, 23, 25, 23, 18, 23, 25, 14, 8, 17, 14, 29, 16, 19, 27, 25, 30, 23, 14, 32, 19, 15, 25, 14, 31, 24, 24, 13, 19, 15, 32, 10, 23, 25, 7, 31, 30, 37, 17, 20, 21, 20, 19, 28, 16, 26, 13, 21, 17, 19, 6, 28, 32, 32, 27, 19, 11, 21, 26, 27, 22, 15, 25, 13, 21, 11, 27, 24, 23, 17, 22, 28, 24, 22, 20, 22, 9, 17, 15, 7, 33, 13, 1, 14, 37, 19, 30, 25, 3, 3, 15, 22, 35, 31, 11, 25, 10, 21, 4, 22, 10, 16, 21, 28, 17, 13, 23, 9, 12, 22, 26, 13, 24, 17, 24, 8, 22, 20, 29, 19, 30, 19, 21, 18, 23, 11, 32, 21, 11, 29, 3, 11, 22, 36, 13, 4, 28, 19, 35, 20, 15, 27, 26, 36, 16, 25, 31, 16, 26, 22, 10, 22, 1, 25, 21, 3, 32, 24, 14, 32, 37, 12, 23, 15, 17, 20, 25, 9, 20, 8, 22, 18, 24, 16, 27, 23, 14, 16, 27, 27, 37, 29, 13, 23, 21, 19, 24, 16, 28, 23, 23, 20, 17, 28, 32, 17, 34, 23, 32, 15, 19, 38, 25, 21, 25, 15, 18, 27, 28, 1, 17, 30, 16, 3, 29, 39, 27, 21, 5, 27, 21, 12, 16, 13, 14, 34, 7, 21, 20, 13, 13, 12, 19, 20, 32, 14, 32, 4, 17, 10, 21, 28, 19, 10, 11, 27, 3, 20, 2, 22, 30, 23, 20, 15, 33, 29, 17, 29, 38, 17, 31, 18, 28, 7, 14, 15, 19, 21, 30, 9, 32, 30, 19, 17, 14, 12, 14, 19, 30, 16, 12, 28, 25, 17, 14, 38, 17, 23, 18, 16, 26, 23, 12, 10, 8, 17, 28, 9, 4, 20, 19, 22, 15, 32, 3, 25, 18, 1, 16, 25, 28, 13, 24, 19, 27, 16, 24, 22, 2, 36, 13, 11, 30, 37, 35, 28, 8, 24, 36, 14, 22, 20, 34, 24, 14, 19, 19, 15, 28, 4, 23, 26, 37, 13, 12, 19, 23, 6, 18, 15, 19, 29, 24, 4, 20, 16, 3, 17, 23, 41, 10, 16, 16, 16, 23, 38, 6, 27, 21, 19, 21, 21, 36, 15, 14, 8, 23, 20, 18, 14, 21, 24};
////    cout << v.size() << ' ' << nConsoleHeight;
//    sort(v.begin(), v.end());
//    int minv = v[0], maxv = v.back();
//    double median;
//    if (v.size() % 2) median = v[v.size() / 2];
//    else median = (v[v.size() / 2 - 1] + v[v.size() / 2]) / 2.0;
//    double Q1 = v[(v.size() + 1) * 0.25];
//    double Q3 = v[(v.size() + 1) * 0.75];
//    double IQR = Q3 - Q1;
//    double lowerWhisker = Q1 - 1.5 * IQR, upperWhisker = Q3 + 1.5 * IQR;
//
//    cout << "Элементов всего: " << v.size() << endl;
//    cout << "Мин. зн: " << minv << endl << "Макс. зн: " << maxv << endl;
//    std::cout << "Q1: " << Q1 << std::endl;
//    std::cout << "Q2 (медиана): " << median << std::endl;
//    std::cout << "Q3: " << Q3 << std::endl;
//    std::cout << "IQR: " << IQR << std::endl;
//    std::cout << "Нижний ус: " << lowerWhisker << std::endl;
//    std::cout << "Верхний ус: " << upperWhisker << std::endl;

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
//////    shadows
////    wcout << (wchar_t) 0x2591 << ' ' << (wchar_t) 0x2592 << ' ' << (wchar_t) 0x2593 << "\n";
//
//////     boxes
////    wcout << (wchar_t) 0x250F << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x2513 << "\n";
////    wcout << (wchar_t) 0x2503 << "   " << (wchar_t) 0x2503 << "\n";
////    wcout << (wchar_t) 0x2523 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x252B << "\n";
////    wcout << (wchar_t) 0x2517 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x2501 << (wchar_t) 0x251B << "\n";
////    wcout << (wchar_t) 0x250C << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2510 << "\n";
////    wcout << (wchar_t) 0x2502 << "   " << (wchar_t) 0x2502 << "\n";
////    wcout << (wchar_t) 0x251C << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2524 << "\n";
////    wcout << (wchar_t) 0x2514 << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2500 << (wchar_t) 0x2518 << "\n";
//    wchar_t wc = 0x2550;
//    wcout << wstring(3, wc) << "\n";
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
