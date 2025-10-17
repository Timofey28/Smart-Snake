#pragma once

#include <vector>
#include <unordered_set>
#include <queue>
#include <cassert>

#include "utils.h"


class Validation
{
public:
    int snakesAmount, snakeAssIndex, startingCellIndex;
    bool snakeIsCorrect, snakeIsLooped;
    Direction startingDirection;

    Validation() {}
    bool IsASnake(const Cell& cell);
    bool IsASnake(CellType cellType);
    bool SnakeSingularityAndCorrectness(const std::vector<Cell>& field, int fieldWidth);
    bool ClosedSpacesExistence(const std::vector<Cell>& field, int fieldWidth);
    bool SnakeHeadIdentification(const std::vector<Cell>& field, int fieldWidth);

private:
    bool __IsPossibleSpace(const Cell& cell);
    bool __IsPossiblePassage(const Cell& cell);
    void __GetInfoAboutWholeSnake(const std::vector<Cell>& field, int fieldWidth, std::vector<bool>& passed, int snakeIndex);
    int __GetFirstSpaceIndex(const std::vector<Cell>& field, const std::vector<bool>& passed);
    void __FindSnakeEnds(const std::vector<Cell>& field, int fieldWidth, int& snakeEnd1, int& snakeEnd2);
    void __GetCellVicinity(
        const std::vector<Cell>& field,
        int fieldWidth,
        int cellIndex,
        Cell& leftCell,
        Cell& rightCell,
        Cell& topCell,
        Cell& bottomCell
    );
    Direction __CalculateStartingDirection(const Cell& leftCell, const Cell& rightCell, const Cell& topCell, const Cell& bottomCell);
};


inline bool Validation::IsASnake(const Cell& cell)
{
    if (cell.type == CellType::SNAKE_BODY || cell.type == CellType::SNAKE_HEAD) return true;
    return false;
}

inline bool Validation::IsASnake(CellType cellType)
{
    if (cellType == CellType::SNAKE_BODY || cellType == CellType::SNAKE_HEAD) return true;
    return false;
}

inline bool Validation::__IsPossibleSpace(const Cell& cell)
{
    static std::unordered_set<CellType> possibleSpace = {
        CellType::PASS,
        CellType::SNAKE_BODY,
        CellType::SNAKE_HEAD
    };
    if (possibleSpace.count(cell.type)) return true;
    return false;
}

inline bool Validation::__IsPossiblePassage(const Cell& cell)
{
    if (cell.type == CellType::PASS || cell.type == CellType::PORTAL) return true;
    return false;
}
