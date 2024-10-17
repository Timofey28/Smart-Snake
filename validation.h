#pragma once

#include <vector>
#include "utils.h"


class Validation
{
public:
    Validation() {}
    bool SnakeSingularityAndCorrectness(const std::vector<Cell>& field, int fieldWidth);
    bool ClosedSpacesExistence(const std::vector<Cell>& field, int fieldWidth);
//    bool SnakeHeadIdentification(const std::vector<Cell>& field, int fieldWidth);

    int snakesAmount;
    bool snakeIsCorrect;
    bool noSpaceAtAll;
    bool snakeIsLooped;

private:
    bool __IsASnake(Cell cell);
    bool __IsPossibleSpace(Cell cell);
    void __GetInfoAboutWholeSnake(std::vector<std::vector<bool>>& fieldInfo, int snakeIndex, const std::vector<Cell>& field);
    int __GetFirstSpaceIndex(const std::vector<Cell>& field, std::vector<std::vector<bool>>& fieldInfo);
    bool __SpaceIsLeft(const std::vector<Cell>& field, std::vector<std::vector<bool>>& fieldInfo);
    void __FindSnakeEnds(const std::vector<Cell>& field, int fieldWidth, int& snakeEnd1, int& snakeEnd2);
    void __GetCellVicinity(
        const std::vector<Cell>& field,
        int fieldWidth,
        int cellIndex,
        CellType& leftCellType,
        CellType& rightCellType,
        CellType& topCellType,
        CellType& bottomCellType
    );
};
