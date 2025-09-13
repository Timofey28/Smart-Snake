#include "utils.h"

#include <map>
#include <unordered_map>
#include <string>
#include <cassert>


static const std::string BASE93_DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!\"#$%&'()*+,-./:;<=>?@[]^_`{|}~";
static std::unordered_map<char, int> BASE93_TO_INT;

std::map<CellType, Color> CELL_COLOR = {
    {CellType::PASS, Color::BLUE},
    {CellType::WALL, Color::CYAN},
    {CellType::PORTAL, Color::BRIGHT_GREEN},
    {CellType::FOOD, Color::RED},
    {CellType::SNAKE_BODY, Color::BRIGHT_MAGENTA},
    {CellType::SNAKE_HEAD, Color::MAGENTA},
};
//std::mt19937 generator(time(nullptr));
std::mt19937 generator(1);
std::uniform_int_distribution<int> uid2{0, 1};

std::string toString(CellType cellType)
{
    switch (cellType) {
        case CellType::PASS: return "Pass";
        case CellType::WALL: return "Wall";
        case CellType::PORTAL: return "Portal";
        case CellType::FOOD: return "Food";
        case CellType::SNAKE_BODY: return "Snake body";
        case CellType::SNAKE_HEAD: return "Snake head";
        default: return "Unknown";
    }
}
std::string toString(Direction direction)
{
    switch (direction) {
        case Direction::LEFT: return "Left";
        case Direction::RIGHT: return "Right";
        case Direction::UP: return "Up";
        case Direction::DOWN: return "Down";
        default: return "Unknown";
    }
}


void initializeBase93Map()
{
    for (int i = 0; i < BASE93_DIGITS.size(); ++i)
        BASE93_TO_INT[BASE93_DIGITS[i]] = i;
}

char toBase93(int num)
{
    assert(num >= 0 && num < BASE93_DIGITS.size());
    return BASE93_DIGITS[num];
}

int fromBase93ToDecimal(char numChar)
{
    return BASE93_TO_INT[numChar];
}


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
    bool& isBorder,
    bool& isCorner,
    bool& isAdjacentToCorner
) {
    isBorder = false;
    isCorner = false;
    isAdjacentToCorner = false;
    if (relX == 0 || relX == fieldWidth - 1 || relY == 0 || relY == fieldHeight - 1) {
        isBorder = true;
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

int getOppositeBorderCellIndex(int relX, int relY, int fieldWidth, int fieldHeight)
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

int randomUnder(int num)
{
    std::uniform_int_distribution<int> uid{0, num - 1};
    return uid(generator);

//    if (num == 2) return uid2(generator);
//    std::uniform_int_distribution<int> uid{0, num - 1};
//    return uid(generator);
}

Direction toLeftFrom(Direction direction)
{
    if (direction == Direction::LEFT) return Direction::DOWN;
    else if (direction == Direction::RIGHT) return Direction::UP;
    else if (direction == Direction::UP) return Direction::LEFT;
    else return Direction::RIGHT;
}

Direction toRightFrom(Direction direction)
{
    if (direction == Direction::LEFT) return Direction::UP;
    else if (direction == Direction::RIGHT) return Direction::DOWN;
    else if (direction == Direction::UP) return Direction::RIGHT;
    else return Direction::LEFT;
}
