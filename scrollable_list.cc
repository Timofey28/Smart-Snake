#include "scrollable_list.h"
using namespace std;

ScrollableList::ScrollableList(int pilesAmount, int maxVisiblePiles, int tableHeight, int upperPileIndex, int cursorPileIndex)
{
    this->pilesAmount_ = pilesAmount;
    this->maxVisiblePiles_ = maxVisiblePiles;
    this->upperPileIndex = upperPileIndex;
    this->cursorPileIndex = cursorPileIndex;

    this->scrollbar = false;
    this->specialSlider_ = false;
    if (pilesAmount > maxVisiblePiles) {
        this->scrollbar = true;
        this->scrollbarHeight = tableHeight - 2;
        this->scrollbarSteps_ = pilesAmount - maxVisiblePiles + 1;
        this->sliderPos = 0;
        if (scrollbarHeight - scrollbarSteps_ + 1 < 1) {  // need more steps than scrollbar height
            this->sliderHeight = 1;
            this->specialSlider_ = true;
            this->__InitPileIndexConverter();
        }
        else this->sliderHeight = scrollbarHeight - scrollbarSteps_ + 1;
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
                Action(Operation::TO_FOCUSED, cursorPileIndex - upperPileIndex, cursorPileIndex)
            };
        }
        // last case: cursorPileIndex == upperPileIndex
        upperPileIndex--;
        cursorPileIndex--;
        vector<Action> actions = {Action(Operation::SHIFT_PILES, upperPileIndex, cursorPileIndex)};
        if (!specialSlider_ ||
            cursorPileIndex == 0 ||
            pileIndexToSliderPos_[upperPileIndex] != sliderPos)
        {
            actions.insert(actions.end(), {
                Action(Operation::ADD_SLIDER_PART, --sliderPos),
                Action(Operation::REMOVE_SLIDER_PART, sliderPos + sliderHeight)
            });
        }
        return actions;
    }

    else if (event == Event::EV_WHEEL_DOWN || event == Event::EV_ARROW_DOWN) {
        if (cursorPileIndex == pilesAmount_ - 1) return {};
        if (cursorPileIndex != upperPileIndex + maxVisiblePiles_ - 1) {
            cursorPileIndex++;
            return {
                Action(Operation::TO_NORMAL, cursorPileIndex - 1 - upperPileIndex, cursorPileIndex - 1),
                Action(Operation::TO_FOCUSED, cursorPileIndex - upperPileIndex, cursorPileIndex)
            };
        }
        // last case: cursorPileIndex == upperPileIndex + maxVisiblePiles_ - 1
        upperPileIndex++;
        cursorPileIndex++;
        vector<Action> actions = {Action(Operation::SHIFT_PILES, upperPileIndex, cursorPileIndex)};
        if (!specialSlider_ ||
            cursorPileIndex == pilesAmount_ - 1 ||
            pileIndexToSliderPos_[upperPileIndex] != sliderPos)
        {
            actions.insert(actions.end(), {
                Action(Operation::ADD_SLIDER_PART, sliderPos + sliderHeight),
                Action(Operation::REMOVE_SLIDER_PART, sliderPos++)
            });
        }
        return actions;
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

    else if (event == Event::EV_CTRL_OR_RMB) {
        upperPileIndex = 0;
        cursorPileIndex = 0;
        sliderPos = 0;
        if (scrollbar) return {
            Action(Operation::SHIFT_PILES, 0, 0),
            Action(Operation::RESET_SCROLLBAR)
        };
        else return {Action(Operation::SHIFT_PILES, 0, 0)};
    }
}

void ScrollableList::__InitPileIndexConverter()
{
    assert(specialSlider_);
    int _listHeight = scrollbarHeight - 2,
        _steps = scrollbarSteps_ - 2;
    int _bigFreqCount = _steps % _listHeight;
    int _smallFrequency = _steps / _listHeight;
    int _bigFreqStart = _listHeight / 2 - _bigFreqCount / 2;
    int _bigFreqEnd = _bigFreqStart + _bigFreqCount - 1;
    for (int i = 0; i < _bigFreqStart * _smallFrequency; ++i) pileIndexToSliderPos_[i + 1] = i / _smallFrequency + 1;
    int _offset = _bigFreqStart * _smallFrequency;
    for (int i = 0; i < (_bigFreqEnd - _bigFreqStart + 1) * (_smallFrequency + 1); ++i)
        pileIndexToSliderPos_[i + _offset + 1] = i / (_smallFrequency + 1) + _bigFreqStart + 1;
    _offset += (_bigFreqEnd - _bigFreqStart + 1) * (_smallFrequency + 1);
    for (int i = 0; i < (_listHeight - 1 - _bigFreqEnd) * _smallFrequency; ++i)
        pileIndexToSliderPos_[i + _offset + 1] = i / _smallFrequency + (_bigFreqEnd + 1) + 1;
}
