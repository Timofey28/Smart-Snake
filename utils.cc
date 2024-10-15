#include "utils.h"

#include <map>
#include <string>


std::map<CellType, Color> CELL_COLOR = {
    {CellType::PASS, Color::BLUE},
    {CellType::WALL, Color::CYAN},
    {CellType::PORTAL, Color::BRIGHT_GREEN},
    {CellType::FOOD, Color::RED},
    {CellType::SNAKE_BODY, Color::BRIGHT_MAGENTA},
    {CellType::SNAKE_HEAD, Color::BLACK},
};

bool canConvertToNumber(std::string str)
{
    try {
        size_t pos;
        std::stoi(str, &pos);
        if (pos != str.length()) {
            return false;
        }
        return true;
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
}

int explainClickInfo(
    int relX,
    int relY,
    int fieldWidth,
    int fieldHeight,
    bool& isBoundary,
    bool& isCorner,
    bool& isAdjacentToCorner
) {
    isBoundary = false;
    isCorner = false;
    isAdjacentToCorner = false;
    if (relX == 0 || relX == fieldWidth - 1 || relY == 0 || relY == fieldHeight - 1) {
        isBoundary = true;
        if (relX == 0 && relY == 0 || relX == 0 && relY == fieldHeight - 1 ||
            relX == fieldWidth - 1 && relY == 0 || relX == fieldWidth - 1 && relY == fieldHeight - 1)
        {
            isCorner = true;
        }
        else if (relX == 0 && relY == 1 || relX == 1 && relY == 0 ||  // левые верхние
                 relX == 0 && relY == fieldHeight - 2 || relX == 1 && relY == fieldHeight - 1 ||  // левые нижние
                 relX == fieldWidth - 2 && relY == fieldHeight - 1 || relX == fieldWidth - 1 && relY == fieldHeight - 2 || // правые нижние
                 relX == fieldWidth - 2 && relY == 0 || relX == fieldWidth - 1 && relY == 1)  // правые верхние
        {
            isAdjacentToCorner = true;
        }
    }
    return relY * fieldWidth + relX;
}

int getOppositeBoundaryCellIndex(int relX, int relY, int fieldWidth, int fieldHeight)
{
    int oppositeX = relX,
        oppositeY = relY;
    if (relX != 0 && relX != fieldWidth - 1) {
        if (relY == 0) oppositeY = fieldHeight - 1;
        else oppositeY = 0;
    }
    else {
        if (relX == 0) oppositeX = fieldWidth - 1;
        else oppositeX = 0;
    }
    return oppositeY * fieldWidth + oppositeX;
}

void getPairedAdjacentCellAndCornerCellIndex(
    int& pairedAdjacentCellIndex,
    int& cornerCellIndex,
    int relX,
    int relY,
    int fieldWidth,
    int fieldHeight
) {
    int cornerX, cornerY, adjX, adjY;
    if (relX == 0 && relY == 1) {
        cornerX = 0;
        cornerY = 0;
        adjX = 1;
        adjY = 0;
    }
    else if (relX == 1 && relY == 0) {
        cornerX = 0;
        cornerY = 0;
        adjX = 0;
        adjY = 1;
    }
    else if (relX == fieldWidth - 2 && relY == 0) {
        cornerX = fieldWidth - 1;
        cornerY = 0;
        adjX = fieldWidth - 1;
        adjY = 1;
    }
    else if (relX == fieldWidth - 1 && relY == 1) {
        cornerX = fieldWidth - 1;
        cornerY = 0;
        adjX = fieldWidth - 2;
        adjY = 0;
    }
    else if (relX == 0 && relY == fieldHeight - 2) {
        cornerX = 0;
        cornerY = fieldHeight - 1;
        adjX = 1;
        adjY = fieldHeight - 1;
    }
    else if (relX == 1 && relY == fieldHeight - 1) {
        cornerX = 0;
        cornerY = fieldHeight - 1;
        adjX = 0;
        adjY = fieldHeight - 2;
    }
    else if (relX == fieldWidth - 2 && relY == fieldHeight - 1) {
        cornerX = fieldWidth - 1;
        cornerY = fieldHeight - 1;
        adjX = fieldWidth - 1;
        adjY = fieldHeight - 2;
    }
    else if (relX == fieldWidth - 1 && relY == fieldHeight - 2) {
        cornerX = fieldWidth - 1;
        cornerY = fieldHeight - 1;
        adjX = fieldWidth - 2;
        adjY = fieldHeight - 1;
    }

    pairedAdjacentCellIndex = adjY * fieldWidth + adjX;
    cornerCellIndex = cornerY * fieldWidth + cornerX;
}
