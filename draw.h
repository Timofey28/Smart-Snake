#pragma once

#include <iostream>
#include <vector>
#include <windows.h>
#include <stdexcept>
#include <cassert>
#include <fcntl.h>
#include <iomanip>

#include "utils.h"
#include "file_handler.h"


extern int nConsoleWidth, nConsoleHeight;
extern int PointOfNoReturn;
const int TAB_WIDTH = 8;

void setPosition(short, short);
void setColor(Color);
void getPosition(short& x, short& y);

namespace draw
{
    void GameCell(const Cell& cell, int stretch = 1);
    void SnakeHead(Cell& cell, Direction movementDirection);
    void Field(const std::vector<Cell>& field, int width, bool onlyPerimeter = false);

    void EnterFieldDimensions(int& fieldWidth, int& fieldHeight);
    void EnterGamesAmount(int& gamesAmount);
    void __ClearInputAndMoveCursorBack(int phraseLength, int inputLength);

    void ProgressBar(int done, int total);

//    void smth(std::string s, int lineNo = 0);
//    void GameCell(const Cell& cell, Color color);

    template<typename BoxSymbols>
    void Box(int indentX, int indentY, int width, int height, int pileContentHeight, int pilesAmount, Color focusColor, int activePile = 0);

    template<typename BoxSymbols>
    void BoxPile(int indentX, int indentY, int pileWidth, int pileHeight, Color color, bool isFirst, bool isLast);

    void TableData(int indentX, int indentY, std::vector<std::string> data, Color color);
    void Symbol(int x, int y, Color color);
    void Symbol(std::vector<std::pair<int, int>> coords, std::vector<Color> colors);
    void Symbol(std::vector<std::pair<int, int>> coords, Color color);

    namespace alert
    {
        void MultimpleOrNoneSnakes(int snakesAmount);
        void IncorrectSnake();
        void ClosedSpaces();
        void LoopedSnake();
        void NoPossibleStart();

        void Victory();

        void Remove();

    }  // namespace alert

}  // namespace draw
