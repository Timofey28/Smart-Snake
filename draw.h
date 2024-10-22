#pragma once

#include <iostream>
#include <vector>
#include "utils.h"


extern int nConsoleWidth, nConsoleHeight;
extern int PointOfNoReturn;
const int TAB_WIDTH = 8;

void setPosition(short, short);
void setColor(Color);
void getPosition(short& x, short& y);

namespace draw
{
    void GameCell(const Cell& cell, int stretch = 1);
    void GameCell(Cell& cell, CellType cellType, int stretch = 1);
    void Field(const std::vector<Cell>& field, int width, bool onlyPerimeter = false);

    void EnterFieldDimensions(int& fieldWidth, int& fieldHeight);
    void __ClearInputAndMoveCursorBack(int phraseLength, int inputLength);

    void smth(std::string s);
//    void GameCell(const Cell& cell, Color color);

    namespace alert
    {
        void MultimpleOrNoneSnakes(int snakesAmount);
        void IncorrectSnake();
        void ClosedSpaces();
        void LoopedSnake();
        void NoPossibleStart();
        void Remove();

    }  // namespace alert

}  // namespace draw
