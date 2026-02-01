#pragma once

#include <iostream>
#include <vector>
#include <windows.h>
#include <stdexcept>
#include <cassert>
#include <fcntl.h>
#include <iomanip>
#include <sstream>
#include <unordered_set>

#include "utils.h"
#include "console.h"

constexpr int TAB_WIDTH = 8;

void setPosition(int, int);
void setPosition(std::pair<int, int>);
void setPosition(const Cell& cell, bool secondPart = false);
void setColor(Color);
void getPosition(int& x, int& y);


namespace draw
{
    template <class Mtx = NullMutex>
    void GameCell(const Cell& cell, Mtx& m = noLock, int stretch = 1);

    void SnakeHead(const Cell& cell, Direction movementDirection);

    template <class Mtx = NullMutex>
    void Field(const std::vector<Cell>& field, int width, Mtx& m = noLock, bool onlyPerimeter = false);

    void ClearInputAndMoveCursorBack(int phraseLength, int inputLength);
    void ProgressBar(int done, int total);

    template <class Mtx = NullMutex>
    void Crash(bool paint, const std::vector<Cell>& field, int width, int snakeHeadIndex = -1, Mtx& m = noLock);

//    void smth(std::string s, int lineNo = 0);
//    void GameCell(const Cell& cell, Color color);

    template <typename BoxSymbols>
    void Box(int indentX, int indentY, int width, int height, int pileContentHeight, int pilesAmount, Color focusColor, int activePile = 0);

    template <typename BoxSymbols>
    void BoxPile(int indentX, int indentY, int pileWidth, int pileHeight, Color color, bool isFirst, bool isLast, bool careful = false);

    void TableData(int indentX, int indentY, std::vector<std::string> data, Color color);
    void Symbol(int x, int y, Color color);
    void Symbol(std::vector<std::pair<int, int>> coords, std::vector<Color> colors);
    void Symbol(std::vector<std::pair<int, int>> coords, Color color);

    template <typename... Args>
    void Info(std::pair<int, int> xy, Args... args, Color color = Color::NORMAL);

}  // namespace draw
