#pragma once

#include <vector>
#include "utils.h"

//#include "draw.h"
//#include <sstream>
//#include <thread>


class Validation
{
public:
    Validation() {}
    bool SnakeSingularityAndCorrectness(const std::vector<Cell>& field, int fieldWidth);
    bool ClosedSpacesExistence(const std::vector<Cell>& field, int fieldWidth);
    bool SnakeHeadIdentification(const std::vector<Cell>& field, int fieldWidth);

    bool IsASnake(const Cell& cell);
    bool IsASnake(CellType cellType);

    int snakesAmount;
    bool snakeIsCorrect;
    bool snakeIsLooped;
    int startingCellIndex;
    Direction startingDirection;

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
