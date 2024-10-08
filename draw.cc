#include "draw.h"

#include <iostream>
#include <windows.h>
#include <stdexcept>

#include <conio.h>

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
        throw runtime_error("Failed to set cursor position " + posStr +
                            ". Error code: " + to_string(errorCode));
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

void draw::FieldParametersInputForm(Playground& playground)
{
    // Ввод размеров игрового поля
    __EnterFieldDimensions(playground.width, playground.height);
    playground.InitializeFieldFromDimensions();

    // Расположение стен, порталов и самой змейки
    __ArrangeFieldElements(playground);

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
    cell.type = cellType;
    cell.UpdateColor();
    setPosition(cell.realX, cell.realY);
    setColor(cell.color);
    cout << string(min(stretch, PointOfNoReturn - cell.num) * 2, ' ');
}

void draw::__EnterFieldDimensions(int& fieldWidth, int& fieldHeight)
{
    int maxFieldWidth = nConsoleWidth / 2 - 2;
    int maxFieldHeight = nConsoleHeight - 2;
    PointOfNoReturn = (maxFieldWidth + 2) * (maxFieldHeight + 2) - 1;
    fieldWidth = 22;
    fieldHeight = 22;
    return;

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

void draw::__ClearInputAndMoveCursorBack(int phraseLength, int inputLength)
{
    short posX, posY;
    getPosition(posX, posY);
    setPosition(phraseLength, posY - 1);
    cout << string(inputLength, ' ');
    setPosition(phraseLength, posY - 1);
}

void draw::__ArrangeFieldElements(Playground& playground)
{
    vector<Cell> field = playground.GetField();
    int fieldIndentX, fieldIndentY;
    playground.GetFieldIndents(fieldIndentX, fieldIndentY);

    // Отрисовка периметра
    system("cls");
    int width = playground.width;
    int height = playground.height;
    draw::GameCell(field[0], width);
    draw::GameCell(field[field.size() - width], width);
    draw::GameCell(field[width]);
    draw::GameCell(field[width * (height - 1) - 1]);
    for (int i = 2; i < height - 1; ++i) {
        draw::GameCell(field[i * width - 1]);
        draw::GameCell(field[i * width]);
    }
    for (int i = 1; i < height - 1; ++i) draw::GameCell(field[i * width + 1], width - 2);

    // Расстановка остальных элементов
    MouseInput mouseInput;
    bool isBoundary, isCorner, isAdjacentToCorner;
    int cellIndex;
    while (true) {
        mouseInput.GetClickInfo();
        int clickedX = mouseInput.X / 2;
        int clickedY = mouseInput.Y;

        if (mouseInput.buttonPressed == ButtonPressed::WHEEL ||
            clickedX == nConsoleWidth / 2 - 1 && clickedY == nConsoleHeight - 1)  // cheatcode to continue
        {
            setColor(Color::NORMAL);
            break;
        }

        if (clickedX >= fieldIndentX && clickedX <= fieldIndentX + width - 1 &&
            clickedY >= fieldIndentY && clickedY <= fieldIndentY + height - 1)
        {
            cellIndex = explainClickInfo(
                clickedX - fieldIndentX,
                clickedY - fieldIndentY,
                width,
                height,
                isBoundary,
                isCorner,
                isAdjacentToCorner
            );

            if (isCorner) continue;
            if (isBoundary) {
                CellType typeToChangeTo = CellType::UNKNOWN;
                if (mouseInput.buttonPressed == ButtonPressed::LEFT && field[cellIndex].type != CellType::PORTAL) {
                    field[cellIndex].type = CellType::PORTAL;
                    field[cellIndex].UpdateColor();
                    draw::GameCell(field[cellIndex]);
                    typeToChangeTo = CellType::PORTAL;
                }
                else if (mouseInput.buttonPressed == ButtonPressed::RIGHT && field[cellIndex].type != CellType::WALL) {
                    field[cellIndex].type = CellType::WALL;
                    field[cellIndex].UpdateColor();
                    draw::GameCell(field[cellIndex]);
                    typeToChangeTo = CellType::WALL;
                }

                if (typeToChangeTo != CellType::UNKNOWN) {
                    int oppositeCellIndex = getOppositeBoundaryCellIndex(clickedX - fieldIndentX, clickedY - fieldIndentY, width, height);
                    field[oppositeCellIndex].type = typeToChangeTo;
                    field[oppositeCellIndex].UpdateColor();
                    draw::GameCell(field[oppositeCellIndex]);

                    if (isAdjacentToCorner) {  // updating corner
                        int pairedAdjacentCellIndex, cornerCellIndex;
                        getPairedAdjacentCellAndCornerCellIndex(
                            pairedAdjacentCellIndex,
                            cornerCellIndex,
                            clickedX - fieldIndentX,
                            clickedY - fieldIndentY,
                            width,
                            height
                        );
                        if (field[cellIndex].type == CellType::PORTAL && field[pairedAdjacentCellIndex].type == CellType::PORTAL) {
                            field[cornerCellIndex].type = CellType::PORTAL;
                        }
                        else {
                            field[cornerCellIndex].type = CellType::WALL;
                        }
                        field[cornerCellIndex].UpdateColor();
                        draw::GameCell(field[cornerCellIndex]);

                        getPairedAdjacentCellAndCornerCellIndex(
                            pairedAdjacentCellIndex,
                            cornerCellIndex,
                            oppositeCellIndex % width,
                            oppositeCellIndex / width,
                            width,
                            height
                        );
                        if (field[oppositeCellIndex].type == CellType::PORTAL && field[pairedAdjacentCellIndex].type == CellType::PORTAL) {
                            field[cornerCellIndex].type = CellType::PORTAL;
                        }
                        else {
                            field[cornerCellIndex].type = CellType::WALL;
                        }
                        field[cornerCellIndex].UpdateColor();
                        draw::GameCell(field[cornerCellIndex]);
                    }
                }
            }

            else {  // if cell is inside field
                if (mouseInput.buttonPressed == ButtonPressed::LEFT) field[cellIndex].type = CellType::WALL;
                else if (mouseInput.buttonPressed == ButtonPressed::RIGHT) field[cellIndex].type = CellType::PASS;
                else if (mouseInput.buttonPressed == ButtonPressed::CTRL_LEFT) field[cellIndex].type = CellType::SNAKE_BODY;
                else if (mouseInput.buttonPressed == ButtonPressed::CTRL_RIGHT) {
                    if (field[cellIndex].type == CellType::SNAKE_BODY)
                        field[cellIndex].type = CellType::SNAKE_HEAD;
                }
                field[cellIndex].UpdateColor();
                draw::GameCell(field[cellIndex]);
            }
        }
    }

    // Проверка на валидность


    // Инициализация объекта playground


//    _getch();
    exit(0);
}
