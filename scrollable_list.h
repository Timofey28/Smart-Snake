#pragma once

#include <vector>

#include "draw.h"
#include <conio.h>


enum Operation
{
    TO_NORMAL,  // value1: screenPileIndex, value2: cursorPileIndex
    TO_FOCUSED,  // value1: screenPileIndex, value2: cursorPileIndex
    SHIFT_PILES,  // value1: upperPileIndex, value2: cursorPileIndex
    ADD_SLIDER_PART,  // value1: sliderPos (index)
    REMOVE_SLIDER_PART,  // value1: sliderPos (index)
    RESET_SCROLLBAR,
};


struct Action
{
    Action(Operation _operation, int _value1 = -1, int _value2 = -1) : operation(_operation), value1(_value1), value2(_value2) {}
    Operation operation;
    int value1, value2;
};


enum Event
{
    EV_WHEEL_UP,
    EV_WHEEL_DOWN,
    EV_ARROW_UP,
    EV_ARROW_DOWN,
    EV_CLICK_PILE,
    EV_CTRL_OR_RMB,  // Right Mouse Button
};


class ScrollableList
{
public:
    int upperPileIndex, cursorPileIndex;
    bool scrollbar;
    int scrollbarHeight, sliderHeight, sliderPos;

    ScrollableList(int pilesAmount, int maxVisiblePiles, int tableHeight, int upperPileIndex, int cursorPileIndex);
    std::vector<Action> ReactTo(Event event, int pileIndex = -1);
    int ScreenPileIndex() const { return cursorPileIndex - upperPileIndex; }

private:
    int pilesAmount_, maxVisiblePiles_;
    int scrollbarSteps_;
    bool specialSlider_;
    std::map<int, int> pileIndexToSliderPos_;

    void __InitPileIndexConverter();
};
