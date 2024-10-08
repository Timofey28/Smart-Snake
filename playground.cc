#include "playground.h"


Playground::Playground()
{
    width = 0;
    height = 0;
}

void Playground::InitializeFieldFromDimensions()
{
    indentX_ = (nConsoleWidth / 2 - width) / 2;
    indentY_ = (nConsoleHeight - height) / 2;

    // Ставим стены по периметру
    field_.resize(width * height);
    CellType cellType;
    for (int i = 0; i < width * height; ++i) {
        cellType = CellType::PASS;
        if (i % width == 0 || i % width == width - 1 || i / width == 0 || i / width == height - 1) cellType = CellType::WALL;
        field_[i] = Cell(i, width, indentX_, indentY_, cellType);
    }
}

void Playground::GetFieldIndents(int& indentX, int& indentY)
{
    indentX = indentX_;
    indentY = indentY_;
}
