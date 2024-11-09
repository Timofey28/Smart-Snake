#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <ctime>
#include <random>


enum Color {
    NORMAL = 15,

    // same color background & text
    BLACK = 0,
    BLUE = 17,
    GREEN = 34,
    CYAN = 51,
    RED = 68,
    MAGENTA = 85,
    GOLD = 102,
    ALMOST_WHITE = 119,
    GRAY = 136,
    BRIGHT_BLUE = 153,
    BRIGHT_GREEN = 170,
    SEA_WAVE = 187,
    BRIGHT_RED = 204,
    BRIGHT_MAGENTA = 221,
    BEIGE = 238,
    WHITE = 255,

    // on black background
    BLUE_ON_BLACK = 1,
    GREEN_ON_BLACK = 2,
    CYAN_ON_BLACK = 3,
    RED_ON_BLACK = 4,
    MAGENTA_ON_BLACK = 5,
    GOLD_ON_BLACK = 6,
    ALMOST_WHITE_ON_BLACK = 7,
    GRAY_ON_BLACK = 8,
    BRIGHT_BLUE_ON_BLACK = 9,
    BRIGHT_GREEN_ON_BLACK = 10,
    SEA_WAVE_ON_BLACK = 11,
    BRIGHT_RED_ON_BLACK = 12,
    BRIGHT_MAGENTA_ON_BLACK = 13,
    BEIGE_ON_BLACK = 14,
    WHITE_ON_BLACK = 15,

    SNAKE_EYES = 80,
    BEIGE_ON_BLUE = 19,
};

enum CellType
{
    UNKNOWN,
    PASS,
    WALL,
    PORTAL,
    FOOD,
    SNAKE_BODY,
    SNAKE_HEAD,
};

enum Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

enum Orientation
{
    VERTICAL,
    HORIZONTAL
};


extern std::map<CellType, Color> CELL_COLOR;
std::string toString(CellType cellType);
std::string toString(Direction direction);

char toBase93(int num);
int fromBase93ToDecimal(char numChar);

bool canConvertToNumber(std::string str);
int explainClickInfo(
    int relX,
    int relY,
    int fieldWidth,
    int fieldHeight,
    bool& isBorder,
    bool& isCorner,
    bool& isAdjacentToCorner
);
int getOppositeBorderCellIndex(int relX, int relY, int fieldWidth, int fieldHeight);
void getPairedAdjacentCellAndCornerCellIndex(
    int& pairedAdjacentCellIndex,
    int& cornerCellIndex,
    int relX,
    int relY,
    int fieldWidth,
    int fieldHeight
);
int randomUnder(int num);
Direction toLeftFrom(Direction direction);
Direction toRightFrom(Direction direction);


struct Cell
{
    Cell() :
        fieldX(-1),
        fieldY(-1),
        realX(-1),
        realY(-1),
        num(-1),
        type(CellType::UNKNOWN) {}

    Cell(int number, int fieldWidth, int indentX, int indentY, CellType cellType = CellType::PASS) :
        fieldX(number % fieldWidth + indentX),
        fieldY(number / fieldWidth + indentY),
        realX(fieldX * 2),
        realY(fieldY),
        num(number),
        type(cellType) {}
    bool isNone() { return type == CellType::UNKNOWN; }

    short fieldX, fieldY, realX, realY, num;
    CellType type;
};
