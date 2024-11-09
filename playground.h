#pragma once

#include <vector>
#include <queue>
#include <unordered_set>
#include <stack>

#include "utils.h"
#include "mouse_input.h"
#include "draw.h"
#include "validation.h"
#include "file_handler.h"
#include "algorithm.h"

#include <sstream>
#include <iostream>


extern int nConsoleWidth, nConsoleHeight;
const int SCREEN_WIDTH = nConsoleWidth / 2;


class Playground
{
public:
    Playground();
    void FieldParametersInputForm();
    void SaveInitialData();

    void ReinitializeStartingData();
    void CalculateNextIteration();
    bool GameOn() { return gameOn_; }
    void SaveLastGame();

private:
    void __InitializeFieldFromDimensions();
    void __ArrangeFieldElements();
    void __AdjustPortals();
    void __CalculatePortalEntries(Orientation orientation, int axisValue);
    bool __WholeAxisIsAWall(Orientation orientation, int axisValue);
    void __MovePortalsBackToBorder();
    void __RepaintSnakeCells();

    void __InitializePlayground();
    void __FillAdjacencyList();
    int __GetPortalExitIndex(int portalEnterIndex, Direction movementDirection);
    void __FillSnakeTurnsQueue();

    Direction __FindMovementDirection(int fromIndex, int toIndex);
    int __FindCellFromMovementDirection(int fromIndex, Direction movementDirection);
    std::vector<int> __GetCellVicinityByIndexes(int cellIndex);


    int width_, height_;  // considering whole field with boundaries
    int indentX_, indentY_;
    std::vector<Cell> field_, initialField_;
    std::vector<std::vector<int>> nodes_, initialNodes_;
    std::vector<int> currentPassCells_, initialCurrentPassCells_;  // to choose a random cell for food
    Direction currentDirection_, initialCurrentDirection_;
    std::queue<Direction> snakeTurns_, initialSnakeTurns_;
    bool gameOn_;
    int foodIndex_, snakeHeadIndex_, snakeAssIndex_, initialSnakeHeadIndex_, initialSnakeAssIndex_;

    Validation validation;
    FileHandler fileHandler;

    // data to record the game
    int firstFoodIndex_;
    std::vector<int> headAndFoodIndexes_;
    Direction crashDirection_;
};
