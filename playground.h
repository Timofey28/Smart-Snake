#pragma once

#include <vector>
#include <queue>
#include <unordered_set>
#include <stack>

#include "utils.h"
#include "mouse_input.h"
#include "algorithm.h"
#include "draw.h"
#include "validation.h"
#include "file_handler.h"


extern int nConsoleWidth, nConsoleHeight;
const int SCREEN_WIDTH = nConsoleWidth / 2;


class Playground
{
public:
    Playground();
    void FieldParametersInputForm();
    void SaveInitialData();
    int GetPortalExitIndex(int portalEnterIndex, Direction movementDirection);
    bool GameOn() { return gameOn_; }

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
    void __FillSnakeTurnsQueue();

    int width_, height_;  // considering whole field with boundaries
    int indentX_, indentY_;
    std::vector<Cell> field_, initialField_;
    std::vector<std::vector<int>> nodes_, initialNodes_;
    std::vector<int> currentPassCells_, initialCurrentPassCells_;  // to choose a random cell for food
    Direction currentDirection_, initialCurrentDirection_;
    std::queue<Direction> snakeTurns_, initialSnakeTurns_;
    int gamesAmount_;
    bool gameOn_;

    Validation validation;
    FileHandler fileHandler;
};
