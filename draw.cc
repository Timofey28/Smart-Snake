#include "draw.h"
using namespace std;

int PointOfNoReturn;
static int previousBarValue = -1;

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

void setPosition(const Cell& cell, bool secondPart)
{
    setPosition(cell.realX + secondPart, cell.realY);
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
    setColor(CELL_COLOR[cell.type]);
    cout << string(max(1, min(stretch, PointOfNoReturn - cell.num)) * 2, ' ');  // (from 1 to whatever is smaller) * 2
}

void draw::SnakeHead(const Cell& cell, Direction movementDirection)
{
    setPosition(cell.realX, cell.realY);
    setColor(CELL_COLOR[cell.type]);
    if (movementDirection == Direction::LEFT) cout << ": ";
    else if (movementDirection == Direction::RIGHT) cout << " :";
    else if (movementDirection == Direction::DOWN) cout << "..";
    else if (movementDirection == Direction::UP) {
        _setmode(_fileno(stdout), _O_U16TEXT);
        wcout << Symbols::DOT_ABOVE << Symbols::DOT_ABOVE;
        _setmode(_fileno(stdout), _O_TEXT);
    }
}

//// функции для отладки
//void draw::GameCell(const Cell& cell, Color color)
//{
//    setPosition(cell.realX, cell.realY);
//    setColor(color);
//    cout << string(2, '#');
//}
//void draw::smth(string s, int lineNo)
//{
//    setPosition(0, lineNo);
//    setColor(Color::NORMAL);
//    cout << s;
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
        for (int i = 0; i < field.size(); ++i) draw::GameCell(field[i]);
    }
}

void draw::Crash(bool paint, const vector<Cell>& field, int width, int snakeHeadIndex)
{
    if (snakeHeadIndex == -1) {
        for (int i = 0; i < field.size(); ++i) {
            if (field[i].type == CellType::SNAKE_HEAD) {
                snakeHeadIndex = i;
                break;
            }
        }
    }
    using BoxSymbols = Symbols::BoxHeavy;
    _setmode(_fileno(stdout), _O_U16TEXT);

    // upper cells
    setPosition(field[snakeHeadIndex - width - 1], true);
    setColor(CELL_COLOR[field[snakeHeadIndex - width - 1].type]);
    if (paint) wcout << BoxSymbols::LEFT_UP_CORNER;
    else wcout << ' ';
    setColor(CELL_COLOR[field[snakeHeadIndex - width].type]);
    if (paint) wcout << wstring(2, BoxSymbols::HORIZONTAL_LINE);
    else wcout << "  ";
    setColor(CELL_COLOR[field[snakeHeadIndex - width + 1].type]);
    if (paint) wcout << BoxSymbols::RIGHT_UP_CORNER;
    else wcout << ' ';

    // lower cells
    setPosition(field[snakeHeadIndex + width - 1], true);
    setColor(CELL_COLOR[field[snakeHeadIndex + width - 1].type]);
    if (paint) wcout << BoxSymbols::LEFT_DOWN_CORNER;
    else wcout << ' ';
    setColor(CELL_COLOR[field[snakeHeadIndex + width].type]);
    if (paint) wcout << wstring(2, BoxSymbols::HORIZONTAL_LINE);
    else wcout << "  ";
    setColor(CELL_COLOR[field[snakeHeadIndex + width + 1].type]);
    if (paint) wcout << BoxSymbols::RIGHT_DOWN_CORNER;
    else wcout << ' ';

    // middle cells
    setPosition(field[snakeHeadIndex - 1], true);
    setColor(CELL_COLOR[field[snakeHeadIndex - 1].type]);
    if (paint) wcout << BoxSymbols::VERTICAL_LINE;
    else wcout << ' ';
    setPosition(field[snakeHeadIndex + 1]);
    setColor(CELL_COLOR[field[snakeHeadIndex + 1].type]);
    if (paint) wcout << BoxSymbols::VERTICAL_LINE;
    else wcout << ' ';

    _setmode(_fileno(stdout), _O_TEXT);
}

void draw::EnterFieldDimensions(int& fieldWidth, int& fieldHeight)
{
    assert(fieldWidth == 0 && fieldHeight == 0);
    int maxFieldWidth = min(91, nConsoleWidth / 2 - 2);
    int maxFieldHeight = min(91, nConsoleHeight - 2);

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
                else if (tolower(input[0]) == 'q') {
                    fieldWidth = 0;
                    fieldHeight = 0;
                    bChooseWidth = true;
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
                else if (tolower(input[0]) == 'q') {
                    fieldWidth = 0;
                    fieldHeight = 0;
                    bChooseWidth = true;
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
    int gamesLimit = 1000;
    string phrase = "Введи количество игр (1 - " + to_string(gamesLimit) + ") => ";
    string input;
    int number;

    system("cls");
    cout << "\n\t" << phrase;
    do {
        getline(cin, input);
        if (canConvertToNumber(input)) {
            number = stoi(input);
            if (number >= 1 && number <= gamesLimit) {
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

void draw::ProgressBar(int done, int total)
{
    int rowIndex = 3;
    int places = nConsoleWidth - 35;

    if (done == total) {
        if (previousBarValue == -1) {
            setPosition(8, rowIndex);
            setColor(Color::BLACK_ON_GREEN);
            cout << "Progress: [100%]";
            setColor(Color::NORMAL);
            cout << " [" << string(places, '#') << "]";
        }
        else {
            setPosition(19, rowIndex);
            setColor(Color::BLACK_ON_GREEN);
            cout << "100";
            setPosition(26 + previousBarValue, rowIndex);
            setColor(Color::NORMAL);
            cout << string(places - previousBarValue, '#');
            previousBarValue = -1;
        }
        return;
    }

    int hashes = round((float) done * places / total);
    int percent = round((float) done / total * 100);

    if (previousBarValue == -1) {
        setPosition(8, rowIndex);
        setColor(Color::BLACK_ON_GREEN);
        cout << "Progress: [" << setw(3) << percent << "%]";
        setColor(Color::NORMAL);
        cout << " [" << string(hashes, '#') << string(places - hashes, '.') << "]";
    }
    else {
        setPosition(19, rowIndex);
        setColor(Color::BLACK_ON_GREEN);
        cout << setw(3) << percent;
        setPosition(26 + previousBarValue, rowIndex);
        setColor(Color::NORMAL);
        cout << string(hashes - previousBarValue, '#');
    }
    previousBarValue = hashes;
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

void draw::alert::Victory()
{
    string alertMsg = "Победа! Ура, ура урааа!!!";
    setPosition(0, 0);
    setColor(Color::NORMAL);
    cout << alertMsg;
}

template<typename BoxSymbols>
void draw::Box(int indentX, int indentY, int width, int height, int pileContentHeight, int pilesAmount, Color focusColor, int activePile)
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    if (activePile == 0) setColor(focusColor);
    setPosition(indentX, indentY);
    wcout << BoxSymbols::LEFT_UP_CORNER << wstring(width - 2, BoxSymbols::HORIZONTAL_LINE) << BoxSymbols::RIGHT_UP_CORNER;
    int tablePileLimit = height / (pileContentHeight + 1);
    for (int pile = 0; pile < min(pilesAmount, tablePileLimit); ++pile) {
        if (pile != activePile) setColor(Color::NORMAL);
        for (int i = 0; i < pileContentHeight; ++i) {
            setPosition(indentX, indentY + pile * (pileContentHeight + 1) + i + 1);
            wcout << BoxSymbols::VERTICAL_LINE << wstring(width - 2, ' ') << BoxSymbols::VERTICAL_LINE;
        }
        if (pile + 1 == activePile) setColor(focusColor);
        setPosition(indentX, indentY + (pile + 1) * (pileContentHeight + 1));
        if (pile == tablePileLimit - 1) wcout << BoxSymbols::LEFT_DOWN_CORNER << wstring(width - 2, BoxSymbols::HORIZONTAL_LINE) << BoxSymbols::RIGHT_DOWN_CORNER;
        else wcout << BoxSymbols::LEFT_TSHAPE << wstring(width - 2, BoxSymbols::HORIZONTAL_LINE) << BoxSymbols::RIGHT_TSHAPE;
    }
    setColor(Color::NORMAL);
    for (int pile = pilesAmount; pile < tablePileLimit; ++pile) {
        for (int i = 0; i <= pileContentHeight; ++i) {
            setPosition(indentX, indentY + pile * (pileContentHeight + 1) + i + 1);
            wcout << BoxSymbols::VERTICAL_LINE << wstring(width - 2, ' ') << BoxSymbols::VERTICAL_LINE;
        }
        setPosition(indentX, indentY + (pile + 1) * (pileContentHeight + 1));
        if (pile == tablePileLimit - 1) wcout << BoxSymbols::LEFT_DOWN_CORNER << wstring(width - 2, BoxSymbols::HORIZONTAL_LINE) << BoxSymbols::RIGHT_DOWN_CORNER;
    }
    _setmode(_fileno(stdout), _O_TEXT);
}
template void draw::Box<Symbols::BoxLight>(int, int, int, int, int, int, Color, int);
template void draw::Box<Symbols::BoxHeavy>(int, int, int, int, int, int, Color, int);

template<typename BoxSymbols>
void draw::BoxPile(int indentX, int indentY, int pileWidth, int pileHeight, Color color, bool isFirst, bool isLast, bool careful)
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    setColor(color);
    if (!careful) {
        setPosition(indentX, indentY);
        if (isFirst) wcout << BoxSymbols::LEFT_UP_CORNER << wstring(pileWidth - 2, BoxSymbols::HORIZONTAL_LINE) << BoxSymbols::RIGHT_UP_CORNER;
        else wcout << BoxSymbols::LEFT_TSHAPE << wstring(pileWidth - 2, BoxSymbols::HORIZONTAL_LINE) << BoxSymbols::RIGHT_TSHAPE;
        setPosition(indentX, indentY + pileHeight - 1);
        if (isLast) wcout << BoxSymbols::LEFT_DOWN_CORNER << wstring(pileWidth - 2, BoxSymbols::HORIZONTAL_LINE) << BoxSymbols::RIGHT_DOWN_CORNER;
        else wcout << BoxSymbols::LEFT_TSHAPE << wstring(pileWidth - 2, BoxSymbols::HORIZONTAL_LINE) << BoxSymbols::RIGHT_TSHAPE;
    }
    for (int i = 1; i <= pileHeight - 2; ++i) {
        setPosition(indentX, indentY + i);
        wcout << BoxSymbols::VERTICAL_LINE;
        setPosition(indentX + pileWidth - 1, indentY + i);
        wcout << BoxSymbols::VERTICAL_LINE;
    }
    setColor(Color::NORMAL);
    _setmode(_fileno(stdout), _O_TEXT);
}
template void draw::BoxPile<Symbols::BoxLight>(int, int, int, int, Color, bool, bool, bool);
template void draw::BoxPile<Symbols::BoxHeavy>(int, int, int, int, Color, bool, bool, bool);

void draw::TableData(int indentX, int indentY, vector<string> data, Color color)
{
    setColor(color);
    for (int i = 0; i < data.size(); ++i) {
        setPosition(indentX, indentY + i);
        cout << data[i];
    }
    setColor(Color::NORMAL);
}

void draw::Symbol(int x, int y, Color color)
{
    setPosition(x, y);
    setColor(color);
    cout << 's';
    setColor(Color::NORMAL);
}

void draw::Symbol(vector<pair<int, int>> coords, vector<Color> colors)
{
    assert(coords.size() == colors.size());
    for (int i = 0; i < coords.size(); ++i) {
        setPosition(coords[i].first, coords[i].second);
        setColor(colors[i]);
        cout << 's';
    }
    setColor(Color::NORMAL);
}

void draw::Symbol(vector<pair<int, int>> coords, Color color)
{
    setColor(color);
    for (int i = 0; i < coords.size(); ++i) {
        setPosition(coords[i].first, coords[i].second);
        cout << 's';
    }
    setColor(Color::NORMAL);
}

template<typename... Args>
void draw::Info(pair<int, int> xy, Args... args, Color color)
{
    setColor(color);
    setPosition(xy.first, xy.second);
    stringstream ss;
    (ss << ... << args);
    cout << ss.str();
    setColor(Color::NORMAL);
}
