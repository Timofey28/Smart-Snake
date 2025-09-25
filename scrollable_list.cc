#include "scrollable_list.h"
using namespace std;

ScrollableList::ScrollableList(int pilesAmount, int maxVisiblePiles, int tableHeight, int upperPileIndex, int cursorPileIndex)
{
    this->pilesAmount_ = pilesAmount;
    this->maxVisiblePiles_ = maxVisiblePiles;
    this->upperPileIndex = upperPileIndex;
    this->cursorPileIndex = cursorPileIndex;

    this->scrollbar = false;
    if (pilesAmount > maxVisiblePiles) {
        this->scrollbar = true;
        this->scrollbarHeight = tableHeight - 2;
        this->scrollbarSteps_ = pilesAmount - maxVisiblePiles + 1;
        this->sliderHeight = scrollbarHeight - scrollbarSteps_ + 1;
        this->sliderPos = 0;
    }
}

vector<Action> ScrollableList::ReactTo(Event event, int pileIndex)
{
    if (event == Event::EV_WHEEL_UP || event == Event::EV_ARROW_UP) {
        if (cursorPileIndex == 0) return {};
        if (cursorPileIndex != upperPileIndex) {
            cursorPileIndex--;
            return {
                Action(Operation::TO_NORMAL, cursorPileIndex + 1 - upperPileIndex, cursorPileIndex + 1),
                Action(Operation::TO_FOCUSED, cursorPileIndex - upperPileIndex, cursorPileIndex),
            };
        }
        // last case: cursorPileIndex == upperPileIndex
        upperPileIndex--;
        cursorPileIndex--;
        return {
            Action(Operation::SHIFT_PILES, upperPileIndex, cursorPileIndex),
            Action(Operation::ADD_SLIDER_PART, --sliderPos),
            Action(Operation::REMOVE_SLIDER_PART, sliderPos + sliderHeight),
        };
    }

    else if (event == Event::EV_WHEEL_DOWN || event == Event::EV_ARROW_DOWN) {
        if (cursorPileIndex == pilesAmount_ - 1) return {};
        if (cursorPileIndex != upperPileIndex + maxVisiblePiles_ - 1) {
            cursorPileIndex++;
            return {
                Action(Operation::TO_NORMAL, cursorPileIndex - 1 - upperPileIndex, cursorPileIndex - 1),
                Action(Operation::TO_FOCUSED, cursorPileIndex - upperPileIndex, cursorPileIndex),
            };
        }
        // last case: cursorPileIndex == upperPileIndex + maxVisiblePiles_ - 1
        upperPileIndex++;
        cursorPileIndex++;
        return {
            Action(Operation::SHIFT_PILES, upperPileIndex, cursorPileIndex),
            Action(Operation::ADD_SLIDER_PART, sliderPos + sliderHeight),
            Action(Operation::REMOVE_SLIDER_PART, sliderPos++),
        };
    }

    else if (event == Event::EV_CLICK_PILE) {
        assert(pileIndex != -1);
        int currentScreenPileIndex = cursorPileIndex - upperPileIndex;
        if (pileIndex == currentScreenPileIndex) return {};
        cursorPileIndex = upperPileIndex + pileIndex;
        return {
            Action(Operation::TO_NORMAL, currentScreenPileIndex, upperPileIndex + currentScreenPileIndex),
            Action(Operation::TO_FOCUSED, cursorPileIndex - upperPileIndex, cursorPileIndex),
        };
    }
}
