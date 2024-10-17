#pragma once

#include <vector>
#include <queue>

#include "utils.h"
#include "mouse_input.h"
#include "algorithm.h"
#include "draw.h"
#include "validation.h"


extern int nConsoleWidth, nConsoleHeight;
const int SCREEN_WIDTH = nConsoleWidth / 2;


class Playground
{
public:
    Playground();
    void FieldParametersInputForm();
    int GetPortalExitIndex(int portalEnterIndex);

private:
    void __InitializeFieldFromDimensions();
    void __ArrangeFieldElements();
    void __IdentifySnakeHead();
    void __AdjustPortals();
    void __CalculatePortalEntries(Orientation orientation, int axisValue);
    bool __WholeAxisIsAWall(Orientation orientation, int axisValue);
    void __MovePortalsBackToBorder();

    int width_, height_;  // considering whole field with boundaries
    int indentX_, indentY_;
    std::vector<Cell> field_;
    std::vector<std::vector<int>> nodes_;
    std::vector<int> currentPassCells_;  // to choose a random cell for food
    Direction currentDirection_;
    std::queue<Direction> snakeTurns_;

    Validation validation;
};
