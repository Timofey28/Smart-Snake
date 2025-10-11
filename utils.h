#pragma once

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <cassert>
#include <ctime>
#include <iomanip>
#include <random>
#include <sstream>


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

    // black on colored background
    BLACK_ON_BLUE = 16,
    BLACK_ON_GREEN = 32,
    BLACK_ON_CYAN = 48,
    BLACK_ON_RED = 64,
    BLACK_ON_MAGENTA = 80,
    BLACK_ON_GOLD = 96,
    BLACK_ON_ALMOST_WHITE = 112,
    BLACK_ON_GRAY = 128,
    BLACK_ON_BRIGHT_BLUE = 144,
    BLACK_ON_BRIGHT_GREEN = 160,
    BLACK_ON_SEA_WAVE = 176,
    BLACK_ON_BRIGHT_RED = 192,
    BLACK_ON_BRIGHT_MAGENTA = 208,
    BLACK_ON_BEIGE = 224,
    BLACK_ON_WHITE = 240,

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
    NONE
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
Direction opposite(Direction direction);

bool isToday(time_t timestamp);
bool isYesterday(time_t timestamp);

std::string _timestampToString(time_t timestamp, std::string format);
std::string timestampToDateStr(time_t timestamp);  // -> dd.mm.yyyy
std::string timestampToISOFormatDateStr(time_t timestamp);  // -> yyyy-mm-dd
std::string timestampToHourMinuteStr(time_t timestamp);  // -> HH:MM
time_t dateStrISOFormatToTimestamp(std::string dateStr);
std::string doubleToStr(double value, int precision = 2);

template<typename... Args>
std::string makeStr(Args... args)
{
    std::stringstream ss;
    (ss << ... << args);
    return ss.str();
}

int numberLength(int number);

struct Cell
{
    short fieldX, fieldY, realX, realY, num;
    CellType type;

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
};

Direction findMovementDirection(int fromIndex, int toIndex, int width);
int findCellFromMovementDirection(int cellIndex, Direction movementDirection, const std::vector<Cell>& field, int width, int height);
int getPortalExitIndex(int portalEnterIndex, Direction movementDirection, const std::vector<Cell>& field, int width, int height);

struct Symbols
{
    static constexpr wchar_t PLUS_MINUS = 0x00B1;
    static constexpr wchar_t HORIZONTAL_DOUBLE_LINE = 0x2550;
    static constexpr wchar_t DOT_ABOVE = 0x02D9;

    struct BoxLight
    {
        static constexpr wchar_t HORIZONTAL_LINE = 0x2500;
        static constexpr wchar_t VERTICAL_LINE = 0x2502;
        static constexpr wchar_t LEFT_TSHAPE = 0x251C;
        static constexpr wchar_t LEFT_UP_CORNER = 0x250C;
        static constexpr wchar_t LEFT_DOWN_CORNER = 0x2514;
        static constexpr wchar_t RIGHT_TSHAPE = 0x2524;
        static constexpr wchar_t RIGHT_UP_CORNER = 0x2510;
        static constexpr wchar_t RIGHT_DOWN_CORNER = 0x2518;
    };
    struct BoxHeavy
    {
        static constexpr wchar_t HORIZONTAL_LINE = 0x2501;
        static constexpr wchar_t VERTICAL_LINE = 0x2503;
        static constexpr wchar_t LEFT_TSHAPE = 0x2523;
        static constexpr wchar_t LEFT_UP_CORNER = 0x250F;
        static constexpr wchar_t LEFT_DOWN_CORNER = 0x2517;
        static constexpr wchar_t RIGHT_TSHAPE = 0x252B;
        static constexpr wchar_t RIGHT_UP_CORNER = 0x2513;
        static constexpr wchar_t RIGHT_DOWN_CORNER = 0x251B;
    };
};
