#pragma once

#include <vector>
#include "utils.h"


class Validation
{
public:
    Validation() {}
    bool SnakeSingularityAndCorrectness(const std::vector<Cell>& field, int fieldWidth);
    bool ClosedSpacesExistence(const std::vector<Cell>& field, int fieldWidth);

    int snakesAmount;
    bool snakeIsCorrect;
    bool noSpaceAtAll;

private:
    bool __IsASnake(Cell cell);
    bool __IsPossibleSpace(Cell cell);
    void __GetInfoAboutWholeSnake(std::vector<std::vector<bool>>& fieldInfo, int snakeIndex, const std::vector<Cell>& field);
    bool __SpaceIsLeft(const std::vector<Cell>& field, std::vector<std::vector<bool>>& fieldInfo);
};
