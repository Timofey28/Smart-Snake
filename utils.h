#pragma once

#include <map>
#include <vector>


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
    BB_BLUE = 1,
    BB_GREEN = 2,
    BB_CYAN = 3,
    BB_RED = 4,
    BB_MAGENTA = 5,
    BB_GOLD = 6,
    BB_ALMOST_WHITE = 7,
    BB_GRAY = 8,
    BB_BRIGHT_BLUE = 9,
    BB_BRIGHT_GREEN = 10,
    BB_SEA_WAVE = 11,
    BB_BRIGHT_RED = 12,
    BB_BRIGHT_MAGENTA = 13,
    BB_BEIGE = 14,
    BB_WHITE = 15,
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


struct Cell
{
    Cell() :
        fieldX(-1),
        fieldY(-1),
        realX(-1),
        realY(-1),
        num(-1),
        type(CellType::UNKNOWN),
        color(Color::BLACK) {}

    Cell(int number, int fieldWidth, int indentX, int indentY, CellType cellType = CellType::PASS) :
        fieldX(number % fieldWidth + indentX),
        fieldY(number / fieldWidth + indentY),
        realX(fieldX * 2),
        realY(fieldY),
        num(number),
        type(cellType),
        color(CELL_COLOR[cellType]) {}
    void UpdateColor() { color = CELL_COLOR[type]; }

    short fieldX, fieldY, realX, realY, num;
    CellType type;
    Color color;
};
