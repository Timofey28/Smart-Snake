#pragma once

#include <iostream>
#include <vector>
#include <windows.h>
#include <stdexcept>
#include <cassert>
#include <fcntl.h>
#include <iomanip>
#include <sstream>

#include "console.h"
#include "utils.h"
#include "file_handler.h"

const int TAB_WIDTH = 8;

void setPosition(short, short);
void setPosition(const Cell& cell, bool secondPart = false);
void setColor(Color);
void getPosition(short& x, short& y);

namespace draw
{
    void GameCell(const Cell& cell, int stretch = 1);
    void SnakeHead(const Cell& cell, Direction movementDirection);
    void Field(const std::vector<Cell>& field, int width, bool onlyPerimeter = false);
    void Crash(bool paint, const std::vector<Cell>& field, int width, int snakeHeadIndex = -1);

    void EnterFieldDimensions(int& fieldWidth, int& fieldHeight, int captionWidth);
    void EnterGamesAmount(int& gamesAmount);
    void __ClearInputAndMoveCursorBack(int phraseLength, int inputLength);

    void ProgressBar(int done, int total);

//    void smth(std::string s, int lineNo = 0);
//    void GameCell(const Cell& cell, Color color);

    template<typename BoxSymbols>
    void Box(int indentX, int indentY, int width, int height, int pileContentHeight, int pilesAmount, Color focusColor, int activePile = 0);

    template<typename BoxSymbols>
    void BoxPile(int indentX, int indentY, int pileWidth, int pileHeight, Color color, bool isFirst, bool isLast, bool careful = false);

    void TableData(int indentX, int indentY, std::vector<std::string> data, Color color);
    void Symbol(int x, int y, Color color);
    void Symbol(std::vector<std::pair<int, int>> coords, std::vector<Color> colors);
    void Symbol(std::vector<std::pair<int, int>> coords, Color color);

    template<typename... Args>
    void Info(std::pair<int, int> xy, Args... args, Color color = Color::NORMAL);

//    namespace alert
//    {
//        void MultipleOrNoneSnakes(int snakesAmount, const std::vector<Cell>& field, int width, std::function<void()> Callback, std::function<void()> cDrawCaption);
//        void IncorrectSnake();
//        void ClosedSpaces();
//        void LoopedSnake();
//        void NoPossibleStart();
//
//        void Remove();
//
//    }  // namespace alert

}  // namespace draw
