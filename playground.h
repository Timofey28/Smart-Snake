#pragma once

#include <vector>
#include "utils.h"
#include "algorithm.h"

extern int nConsoleWidth, nConsoleHeight;
const int SCREEN_WIDTH = nConsoleWidth / 2;


class Playground
{
public:
    Playground();
    void InitializeFieldFromDimensions();
    std::vector<Cell> GetField() { return field_; }
    void GetFieldIndents(int&, int&);

    int width, height;  // considering whole field with boundaries

private:
    int indentX_, indentY_;
    std::vector<Cell> field_;
    std::vector<std::vector<int>> nodes_;
    std::vector<int> currentPassCells_;  // to choose a random cell for food
};
