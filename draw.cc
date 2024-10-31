#include "draw.h"

#include <iostream>
#include <windows.h>
#include <stdexcept>

using namespace std;


int PointOfNoReturn;

void setPosition(short x, short y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!SetConsoleCursorPosition(handle, coord)) {
        DWORD errorCode = GetLastError();
        string posStr = "(" + to_string(x) + ", " + to_string(y) + ")";
        throw runtime_error("Failed to set cursor position " + posStr + ". Error code: " + to_string(errorCode));
    }
}

void setColor(Color color)
{
    static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, (WORD) color);
}

void getPosition(short& x, short& y)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
        COORD cursorPosition = consoleInfo.dwCursorPosition;
        x = cursorPosition.X;
        y = cursorPosition.Y;
    }
    else throw runtime_error("Failed to get current cursor position.");
}

void draw::GameCell(const Cell& cell, int stretch)
{
    if (cell.num == PointOfNoReturn) return;
    setPosition(cell.realX, cell.realY);
    setColor(cell.color);
    cout << string(min(stretch, PointOfNoReturn - cell.num) * 2, ' ');
}

void draw::GameCell(Cell& cell, CellType cellType, int stretch)
{
    if (cell.num == PointOfNoReturn) return;
    if (cell.type != cellType) {
        cell.type = cellType;
        cell.UpdateColor();
        setPosition(cell.realX, cell.realY);
        setColor(cell.color);
        cout << string(min(stretch, PointOfNoReturn - cell.num) * 2, ' ');
    }
}

//void draw::GameCell(const Cell& cell, Color color)
//{
//    setPosition(cell.realX, cell.realY);
//    setColor(color);
//    cout << string(2, ' ');
//}

void draw::Field(const vector<Cell>& field, int width, bool onlyPerimeter)
{
    int height = field.size() / width;
    if (onlyPerimeter) {
        system("cls");
        draw::GameCell(field[0], width);
        draw::GameCell(field[field.size() - width], width);
        draw::GameCell(field[width]);
        draw::GameCell(field[width * (height - 1) - 1]);
        for (int i = 2; i < height - 1; ++i) {
            draw::GameCell(field[i * width - 1]);
            draw::GameCell(field[i * width]);
        }
        for (int i = 1; i < height - 1; ++i) draw::GameCell(field[i * width + 1], width - 2);
    } else {
        system("cls");
        for (int i = 0; i < field.size(); ++i) draw::GameCell(field[i]);
    }
}

void draw::EnterFieldDimensions(int& fieldWidth, int& fieldHeight)
{
    int maxFieldWidth = nConsoleWidth / 2 - 2;
    int maxFieldHeight = nConsoleHeight - 2;
    PointOfNoReturn = nConsoleWidth / 2 * nConsoleHeight - 1;
//    fieldWidth = 25;
//    fieldHeight = 24;
//    return;

    string phraseChooseWidth = "Выбери ширину поля (3 - " + to_string(maxFieldWidth) + ") => ";
    string phraseChooseHeight = "Выбери высоту поля (3 - " + to_string(maxFieldHeight) + ") => ";
    string input;
    int number;
    bool bChooseWidth = true;
    do {
        if (!fieldWidth && !fieldHeight) system("cls");
        if (bChooseWidth) {
            if (!fieldWidth && !fieldHeight) cout << "\n\t" << phraseChooseWidth;
            getline(cin, input);
            if (canConvertToNumber(input)) {
                number = stoi(input);
                if (number >= 3 && number <= maxFieldWidth) {
                    fieldWidth = number;
                    bChooseWidth = false;
                    if (!fieldHeight) cout << "\t" << phraseChooseHeight;
                }
                else {
                    cout << '\a';
                    __ClearInputAndMoveCursorBack(TAB_WIDTH + phraseChooseWidth.length(), input.length());
                }
            }
            else {
                if (input.empty()) {
                    if (fieldHeight) setPosition(TAB_WIDTH + phraseChooseWidth.length(), 2);
                    else bChooseWidth = false;
                }
                else __ClearInputAndMoveCursorBack(TAB_WIDTH + phraseChooseWidth.length(), input.length());
            }
        }
        else {
            if (!fieldWidth && !fieldHeight) cout << "\n\t" << phraseChooseHeight;
            getline(cin, input);
            if (canConvertToNumber(input)) {
                number = stoi(input);
                if (number >= 3 && number <= maxFieldHeight) {
                    fieldHeight = number;
                    bChooseWidth = true;
                    if (!fieldWidth) cout << "\t" << phraseChooseWidth;
                }
                else {
                    cout << '\a';
                    __ClearInputAndMoveCursorBack(TAB_WIDTH + phraseChooseHeight.length(), input.length());
                }
            }
            else {
                if (input.empty()) {
                    if (fieldWidth) setPosition(TAB_WIDTH + phraseChooseHeight.length(), 2);
                    else bChooseWidth = true;
                }
                else __ClearInputAndMoveCursorBack(TAB_WIDTH + phraseChooseHeight.length(), input.length());
            }
        }
    } while (!fieldWidth || !fieldHeight);
    fieldWidth += 2;
    fieldHeight += 2;
}

void draw::EnterGamesAmount(int& gamesAmount)
{
    string phrase = "Введи количество игр (1 - 10) => ";
    string input;
    int number;

    system("cls");
    cout << "\n\t" << phrase;
    do {
        getline(cin, input);
        if (canConvertToNumber(input)) {
            number = stoi(input);
            if (number >= 1 && number <= 10) {
                gamesAmount = number;
                return;
            }
            cout << '\a';
            __ClearInputAndMoveCursorBack(TAB_WIDTH + phrase.length(), input.length());
        }
        else {
            if (input.empty()) setPosition(TAB_WIDTH + phrase.length(), 1);
            else __ClearInputAndMoveCursorBack(TAB_WIDTH + phrase.length(), input.length());
        }
    } while (true);
}

void draw::__ClearInputAndMoveCursorBack(int phraseLength, int inputLength)
{
    short posX, posY;
    getPosition(posX, posY);
    setPosition(phraseLength, posY - 1);
    cout << string(inputLength, ' ');
    setPosition(phraseLength, posY - 1);
}


// namespace alert

void draw::alert::MultimpleOrNoneSnakes(int snakesAmount)
{
    string alertMsg;
    if (!snakesAmount) alertMsg = "Не найдено ни одной змейки. ИСПРАВИТЬ!!!";
    else alertMsg = "Змейка должна быть только одна. ИСПРАВИТЬ!!!";
    setPosition(0, 0);
    setColor(Color::NORMAL);
    cout << alertMsg;
}

void draw::alert::IncorrectSnake()
{
    string alertMsg = "Змейка некорректна, ее части не могут находиться рядом друг с другом. ИСПРАВИТЬ!!!";
    setPosition(0, 0);
    setColor(Color::NORMAL);
    cout << alertMsg;
}

void draw::alert::ClosedSpaces()
{
    string alertMsg = "На игровом поле имеются комнаты, в которые невозможно попасть. Закрась их или сделай туда проход";
    setPosition(0, 0);
    setColor(Color::NORMAL);
    cout << alertMsg;
}

void draw::alert::LoopedSnake()
{
    string alertMsg = "Змейка зациклена, а так нельзя. ИСПРАВЬ!!!";
    setPosition(0, 0);
    setColor(Color::NORMAL);
    cout << alertMsg;
}

void draw::alert::NoPossibleStart()
{
    string alertMsg = "Змейка не может начать игру, потому что ей некуда идти. ИСПРАВИТЬ!!!";
    setPosition(0, 0);
    setColor(Color::NORMAL);
    cout << alertMsg;
}

void draw::alert::Remove()
{
    setPosition(0, 0);
    setColor(Color::BLACK);
    cout << string(100, ' ');
}

void draw::smth(string s)
{
    setPosition(0, 0);
    setColor(Color::NORMAL);
    std::cout << s;
}
