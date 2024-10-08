#pragma once

#include <iostream>
#include "playground.h"
#include "mouse_input.h"
#include "utils.h"


extern int nConsoleWidth, nConsoleHeight;
extern int PointOfNoReturn;
const int TAB_WIDTH = 8;

void setPosition(short, short);
void setColor(Color);
void getPosition(short& x, short& y);

namespace draw
{
    void FieldParametersInputForm(Playground& playground);
    void GameCell(const Cell& cell, int stretch = 1);
    void GameCell(Cell& cell, CellType cellType, int stretch = 1);

    void __EnterFieldDimensions(int& fieldWidth, int& fieldHeight);
    void __ClearInputAndMoveCursorBack(int phraseLength, int inputLength);
    void __ArrangeFieldElements(Playground& playground);

//    COLOR_CREATING_FIELD_BOUNDARY_CHANGING = Color::CYAN;
//    COLOR_CREATING_FIELD_BOUNDARY_STATIC = Color::BRIGHT_BLUE;
}
