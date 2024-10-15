#pragma once

#include <vector>
#include "utils.h"


class Validation
{
public:
    Validation();
    bool SnakeIsGood(const std::vector<Cell>& field, int fieldWidth);
    bool NoClosedSpaces(const std::vector<Cell>& field, int fieldWidth);

    int snakesAmount;

private:
    bool __IsASnake(Cell cell);
    void __GetInfoAboutWholeSnake(std::vector<std::vector<bool>>& fieldInfo, int snakeIndex, const std::vector<Cell>& field);
};
