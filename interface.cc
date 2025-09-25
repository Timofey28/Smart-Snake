#include "interface.h"
using namespace std;

int Interface::s_dateTableHeight_, Interface::s_experimentTableHeight_;
int Interface::s_dateTablePileLimit_, Interface::s_experimentTablePileLimit_;
int Interface::s_experimentLeftMargin_;
bool Interface::s_focusOnDatesList_;
unordered_map<time_t, Experiment> Interface::s_experiments_;
vector<Experiment> Interface::s_selectedDateExperiments_;
optional<ScrollableList> Interface::s_listDates_, Interface::s_listExperiments_;
ClickInfo Interface::clickInfo;

void Interface::__InitTableDimensions()
{
    // Фактическая высота таблиц
    int maxTableHeight = nConsoleHeight - s_upperMargin_ * 2;
    int pileHeight = s_datePileContentHeight_ + 1;
    s_dateTableHeight_ = maxTableHeight - (pileHeight + (maxTableHeight % pileHeight) - 1) % pileHeight;
    pileHeight = s_experimentPileContentHeight_ + 1;
    s_experimentTableHeight_ = maxTableHeight - (pileHeight + (maxTableHeight % pileHeight) - 1) % pileHeight;
    assert(s_experimentTableHeight_ >= 1);

    // Максимальное число отображаемых в один момент плиток в таблице
    s_dateTablePileLimit_ = s_dateTableHeight_ / (s_datePileContentHeight_ + 1);
    s_experimentTablePileLimit_ = s_experimentTableHeight_ / (s_experimentPileContentHeight_ + 1);
}

Experiment* Interface::DatesAndExperimentsList()
{
    __InitTableDimensions();
    s_focusOnDatesList_ = true;

    system("cls");
    if (FileHandler::s_experimentAmountsByDates.empty()) {
        cout << "\n\tИгры не найдены.";
        this_thread::sleep_for(1000ms);
        return nullptr;
    }
    FileHandler::s_currExpAmountsByDatesIter = FileHandler::s_experimentAmountsByDates.begin();
    time_t latestDateTT = FileHandler::s_currExpAmountsByDatesIter->first;
    s_selectedDateExperiments_.clear();
    for (int no = 1; no <= FileHandler::s_currExpAmountsByDatesIter->second; ++no) {
        s_selectedDateExperiments_.push_back(__GetExperimentInfo(latestDateTT + no));
    }

    s_listDates_ = ScrollableList(FileHandler::s_experimentAmountsByDates.size(), s_dateTablePileLimit_, s_dateTableHeight_, 0, 0);
    s_listExperiments_ = ScrollableList(s_selectedDateExperiments_.size(), s_experimentTablePileLimit_, s_experimentTableHeight_, 0, 0);

    draw::Box<BoxStyle>(
        s_dateLeftMargin_, s_upperMargin_,
        s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2, s_dateTableHeight_,
        s_datePileContentHeight_, FileHandler::s_dateFolders.size(), s_colorFocused_, 0
    );
    int pilesShown = min(s_dateTablePileLimit_, (int) FileHandler::s_dateFolders.size());
    for (int pile = 0; pile < pilesShown; ++pile) __DrawTableData(true, (pile == 0) ? s_colorFocused_ : Color::NORMAL, pile, pile);

    s_experimentLeftMargin_ = s_dateLeftMargin_ + s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2 + s_listDates_.value().scrollbar + 1;
    draw::Box<BoxStyle>(
        s_experimentLeftMargin_, s_upperMargin_,
        s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 2, s_experimentTableHeight_,
        s_experimentPileContentHeight_, FileHandler::s_dateFolders.size(), s_colorFocused_, -1
    );
    pilesShown = min(s_experimentTablePileLimit_, (int) s_selectedDateExperiments_.size());
    for (int pile = 0; pile < pilesShown; ++pile) __DrawTableData(false, Color::NORMAL, pile, pile);

    if (s_listDates_.value().scrollbar) __DrawScrollbar(true);
    if (s_listExperiments_.value().scrollbar) __DrawScrollbar(false);

    vector<Action> actions;
    while (true) {
        MouseInput::GetAnyEventInfo();
        ButtonPressed bp = MouseInput::buttonPressed;
        if (MouseInput::isKeyboardEvent) {
            if (bp == ButtonPressed::ARROW_LEFT) {
                if (!s_focusOnDatesList_) {
                    s_focusOnDatesList_ = true;
                    __DrawWholeTablePile(false, Color::NORMAL);
                    __DrawWholeTablePile(true, s_colorFocused_);
                }
            }

            else if (bp == ButtonPressed::ARROW_RIGHT) {
                if (s_focusOnDatesList_) {
                    s_focusOnDatesList_ = false;
                    __DrawWholeTablePile(true, s_colorSelected_);
                    __DrawWholeTablePile(false, s_colorFocused_);
                }
            }

            else if (bp == ButtonPressed::ARROW_UP) {
                if (s_focusOnDatesList_) {
                    actions = s_listDates_.value().ReactTo(Event::EV_ARROW_UP);
                    __Execute(true, actions);
                    if (actions.size()) {
                        assert(FileHandler::s_currExpAmountsByDatesIter != FileHandler::s_experimentAmountsByDates.begin());
                        FileHandler::s_currExpAmountsByDatesIter--;
                        __LoadNewExperiments();
                    }
                }
                else {
                    actions = s_listExperiments_.value().ReactTo(Event::EV_ARROW_UP);
                    __Execute(false, actions);
                }
            }

            else if (bp == ButtonPressed::ARROW_DOWN) {
                if (s_focusOnDatesList_) {
                    actions = s_listDates_.value().ReactTo(Event::EV_ARROW_DOWN);
                    __Execute(true, actions);
                    if (actions.size()) {
                        FileHandler::s_currExpAmountsByDatesIter++;
                        assert(FileHandler::s_currExpAmountsByDatesIter != FileHandler::s_experimentAmountsByDates.end());
                        __LoadNewExperiments();
                    }
                }
                else {
                    actions = s_listExperiments_.value().ReactTo(Event::EV_ARROW_DOWN);
                    __Execute(false, actions);
                }
            }

            else if (bp == ButtonPressed::BACKSPACE || bp == ButtonPressed::ESCAPE) {
                s_selectedDateExperiments_.clear();
                s_listDates_.reset();
                s_listExperiments_.reset();
                return nullptr;
            }

            else if (bp == ButtonPressed::ENTER) {
                if (s_focusOnDatesList_) {
                    s_focusOnDatesList_ = false;
                    __DrawWholeTablePile(true, s_colorSelected_);
                    __DrawWholeTablePile(false, s_colorFocused_);
                }
                else return &s_selectedDateExperiments_[s_listExperiments_.value().cursorPileIndex];
            }
        }

        else {  // Mouse event
            __GetLastClickInfo();
            bool changedList = false;
            if (!clickInfo.bDatesList && !clickInfo.bExperimentsList) continue;
            if (clickInfo.bDatesList) {
                if (!s_focusOnDatesList_) changedList = true;
                s_focusOnDatesList_ = true;
            }
            else {  // clickInfo.bExperimentsList == true
                if (s_focusOnDatesList_) changedList = true;
                s_focusOnDatesList_ = false;
            }

            if (bp == ButtonPressed::WHEEL_UP) {
                if (s_focusOnDatesList_) {
                    if (changedList) __DrawBoxPile(false, Color::NORMAL, 0);
                    actions = s_listDates_.value().ReactTo(Event::EV_ARROW_UP);
                    __Execute(true, actions);
                    if (actions.size()) {
                        assert(FileHandler::s_currExpAmountsByDatesIter != FileHandler::s_experimentAmountsByDates.begin());
                        FileHandler::s_currExpAmountsByDatesIter--;
                        __LoadNewExperiments();
                    }
                }
                else {
                    if (changedList) {
                        __DrawWholeTablePile(true, s_colorSelected_);
                        __DrawWholeTablePile(false, s_colorFocused_);
                    }
                    actions = s_listExperiments_.value().ReactTo(Event::EV_ARROW_UP);
                    __Execute(false, actions);
                }
            }

            else if (bp == ButtonPressed::WHEEL_DOWN) {
                if (s_focusOnDatesList_) {
                    if (changedList) __DrawBoxPile(false, Color::NORMAL, 0);
                    actions = s_listDates_.value().ReactTo(Event::EV_ARROW_DOWN);
                    __Execute(true, actions);
                    if (actions.size()) {
                        FileHandler::s_currExpAmountsByDatesIter++;
                        assert(FileHandler::s_currExpAmountsByDatesIter != FileHandler::s_experimentAmountsByDates.end());
                        __LoadNewExperiments();
                    }
                }
                else {
                    if (changedList) __DrawWholeTablePile(true, s_colorSelected_);
                    actions = s_listExperiments_.value().ReactTo(Event::EV_ARROW_DOWN);
                    __Execute(false, actions);
                }
            }

            else if (bp == ButtonPressed::LEFT_BUTTON && !clickInfo.isBorder && !clickInfo.isInnerBorder) {
                if (s_focusOnDatesList_) {
                    if (changedList && clickInfo.screenPileIndex != s_listDates_.value().ScreenPileIndex()) {
                        __DrawBoxPile(false, Color::NORMAL, 0);
                    }
                    if (s_listDates_.value().ScreenPileIndex() == clickInfo.screenPileIndex) {
                        s_focusOnDatesList_ = false;
                        __DrawWholeTablePile(true, s_colorSelected_);
                        __DrawWholeTablePile(false, s_colorFocused_);
                    }
                    else {
                        actions = s_listDates_.value().ReactTo(Event::EV_CLICK_PILE, clickInfo.screenPileIndex);
                        __Execute(true, actions);
                        if (actions.size()) {
                            int shift = clickInfo.screenPileIndex - actions[0].value1;
                            assert(shift != 0);
                            FileHandler::s_currExpAmountsByDatesIter = next(FileHandler::s_currExpAmountsByDatesIter, shift);
                            __LoadNewExperiments();
                        }
                    }
                }
                else return &s_selectedDateExperiments_[s_listExperiments_.value().cursorPileIndex];
            }
        }
    }
}

Experiment Interface::__GetExperimentInfo(fs::path experimentFolderPath)
{
    int experimentNo;
    time_t timestamp;
    try {
        experimentNo = stoi((*(--experimentFolderPath.end())).string());
        string dateStr = (*(++experimentFolderPath.begin())).string();
        timestamp = dateStrISOFormatToTimestamp(dateStr);
    } catch (const exception& e) {
        throw runtime_error("Unable to parse folder \"" + experimentFolderPath.generic_string() + "\".");
    }

    time_t experimentDateNo = timestamp + experimentDateNo;
    if (s_experiments_.count(experimentDateNo) == 0) {
        s_experiments_.emplace(experimentDateNo, Experiment(experimentFolderPath));
    }
    return s_experiments_.at(experimentDateNo);
}

Experiment Interface::__GetExperimentInfo(time_t experimentDateNo)
{
    if (s_experiments_.count(experimentDateNo) == 0) {
        string dateFolder;
        int experimentNo;
        time_t dateTT;
        try {
            dateFolder = timestampToISOFormatDateStr(experimentDateNo);
            dateTT = dateStrISOFormatToTimestamp(dateFolder);
            experimentNo = (int) experimentDateNo - dateTT;
        } catch (const exception& e) {
            throw runtime_error("Unable to parse timestamp (time_t) " + to_string(experimentDateNo) + ".");
        }

        fs::path experimentFolderPath = FileHandler::GetGamesFolder() / dateFolder / to_string(experimentNo);
        s_experiments_.emplace(experimentDateNo, Experiment(experimentFolderPath));
    }
    return s_experiments_.at(experimentDateNo);
}

vector<string> Interface::__MakePileData(
    map<time_t, int, greater<time_t>>::iterator experimentAmountByDateIterator,
    int cursorPileIndex
) {
    string firstLine = to_string(cursorPileIndex + 1) + ") " + timestampToDateStr(experimentAmountByDateIterator->first);
    if (isToday(experimentAmountByDateIterator->first)) firstLine += ", сегодня";
    else if (isYesterday(experimentAmountByDateIterator->first)) firstLine += ", вчера";
    string secondLine = "Экспериментов: " + to_string(experimentAmountByDateIterator->second);

    assert(firstLine.size() <= s_datePileContentWidth_);
    assert(secondLine.size() <= s_datePileContentWidth_);
    firstLine += string(s_datePileContentWidth_ - firstLine.size(), ' ');
    secondLine += string(s_datePileContentWidth_ - secondLine.size(), ' ');
    return {firstLine, secondLine};
}

vector<string> Interface::__MakePileData(Experiment e, int cursorPileIndex)
{
    vector<string> result = {
//                to_string(cursorPileIndex + 1) + ". Создан в " + timestampToHourMinuteStr(e.creationTime),
        to_string(cursorPileIndex + 1) + ". Поле " + to_string(e.fieldWidth - 2) + "x" + to_string(e.fieldHeight - 2),
//                to_string(cursorPileIndex + 1) + ". " + timestampToHourMinuteStr(e.creationTime) + ", поле " + to_string(e.fieldWidth - 2) + "x" + to_string(e.fieldHeight - 2),
        "  Игр: " + to_string(e.gamesAmount),
        "  Нач. длина змеи: " + to_string(e.initialSnakeLength),
        "  Макс. длина змеи: " + to_string(e.maxPossibleSnakeLength),
        "  Лучшая: " + to_string(e.bestScore),
        "  В среднем: " + doubleToStr(e.avgScore),
    };
    for (int i = 0; i < result.size(); ++i) {
        assert(result[i].size() <= s_experimentPileContentWidth_);
        result[i] += string(s_experimentPileContentWidth_ - result[i].size(), ' ');
    }
    return result;
}

void Interface::__DrawTableData(bool isDateTable, Color color, optional<int> screenPileIndex, optional<int> cursorPileIndex)
{
    int _leftMargin, _pileContentHeight, _pilePadding, _screenPileIndex, _cursorPileIndex;
    vector<string> data;
    if (isDateTable) {
        _leftMargin = s_dateLeftMargin_;
        _pileContentHeight = s_datePileContentHeight_;
        _pilePadding = s_datePilePadding_;
        if (screenPileIndex) {
            _screenPileIndex = screenPileIndex.value();
            _cursorPileIndex = cursorPileIndex.value_or(s_listDates_.value().cursorPileIndex);
            auto it = next(FileHandler::s_experimentAmountsByDates.begin(), _cursorPileIndex);
            data = __MakePileData(it, _cursorPileIndex);
        }
        else {
            _screenPileIndex = s_listDates_.value().ScreenPileIndex();
            auto it = next(FileHandler::s_experimentAmountsByDates.begin(), s_listDates_.value().cursorPileIndex);
            data = __MakePileData(it, s_listDates_.value().cursorPileIndex);
        }
    }
    if (!isDateTable) {
        _leftMargin = s_experimentLeftMargin_;
        _pileContentHeight = s_experimentPileContentHeight_;
        _pilePadding = s_experimentPilePadding_;
        if (screenPileIndex) {
            _screenPileIndex = screenPileIndex.value();
            _cursorPileIndex = cursorPileIndex.value_or(s_listExperiments_.value().cursorPileIndex);
            data = __MakePileData(s_selectedDateExperiments_[_cursorPileIndex], _cursorPileIndex);
        }
        else {
            _screenPileIndex = s_listExperiments_.value().ScreenPileIndex();
            _cursorPileIndex = s_listExperiments_.value().cursorPileIndex;
            data = __MakePileData(s_selectedDateExperiments_[_cursorPileIndex], _cursorPileIndex);
        }
    }
    draw::TableData(
        _leftMargin + 1 + _pilePadding,
        s_upperMargin_ + 1 + _screenPileIndex * (_pileContentHeight + 1),
        data,
        color
    );
}

void Interface::__DrawBoxPile(bool isDateTable, Color color, optional<int> screenPileIndex)
{
    int _screenPileIndex;
    if (isDateTable) {
        _screenPileIndex = screenPileIndex.value_or(s_listDates_.value().ScreenPileIndex());
        draw::BoxPile<BoxStyle>(
            s_dateLeftMargin_,
            s_upperMargin_ + _screenPileIndex * (s_datePileContentHeight_ + 1),
            s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2,
            s_datePileContentHeight_ + 2,
            color,
            _screenPileIndex == 0,
            _screenPileIndex == s_dateTablePileLimit_ - 1
        );
    }
    else {
        _screenPileIndex = screenPileIndex.value_or(s_listExperiments_.value().ScreenPileIndex());
        draw::BoxPile<BoxStyle>(
            s_experimentLeftMargin_,
            s_upperMargin_ + _screenPileIndex * (s_experimentPileContentHeight_ + 1),
            s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 2,
            s_experimentPileContentHeight_ + 2,
            color,
            _screenPileIndex == 0,
            _screenPileIndex == s_experimentTablePileLimit_ - 1
        );
    }
}

void Interface::__DrawWholeTablePile(bool isDateTable, Color color, optional<int> screenPileIndex, optional<int> cursorPileIndex)
{
    __DrawBoxPile(isDateTable, color, screenPileIndex);
    __DrawTableData(isDateTable, color, screenPileIndex, cursorPileIndex);
}

void Interface::__DrawVerticalLine(int x, int y, int length, Color color)
{
    vector<pair<int, int>> coords;
    coords.reserve(length);
    for (int i = y; i < y + length; ++i) coords.push_back({x, i});
    draw::Symbol(coords, color);
}

void Interface::__DrawScrollbar(bool dateTable)
{
    int _x, _scrollbarHeight, _sliderHeight;
    if (dateTable) {
        _x = s_dateLeftMargin_ + s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2;
        _scrollbarHeight = s_listDates_.value().scrollbarHeight;
        _sliderHeight = s_listDates_.value().sliderHeight;
    }
    else {
        _x = s_experimentLeftMargin_ + s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 2;
        _scrollbarHeight = s_listExperiments_.value().scrollbarHeight;
        _sliderHeight = s_listExperiments_.value().sliderHeight;
    }
    __DrawVerticalLine(_x, s_upperMargin_ + 1, _sliderHeight, s_colorSlider_);
    __DrawVerticalLine(_x, s_upperMargin_ + 1 + _sliderHeight, _scrollbarHeight - _sliderHeight, s_colorScrollbar_);
}

void Interface::__RemoveExperimentTableScrollbar()
{
    __DrawVerticalLine(
        s_experimentLeftMargin_ + s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 2,
        s_upperMargin_ + 1,
        s_experimentTableHeight_ - 2,
        Color::BLACK
    );
}

void Interface::__ClearPile(bool dateTable, int screenPileIndex)
{
    if (dateTable) {
        draw::TableData(
            s_dateLeftMargin_+ 1 + s_datePilePadding_,
            s_upperMargin_ + 1 + screenPileIndex * (s_datePileContentHeight_ + 1),
            vector<string>(s_datePileContentHeight_, string(s_datePileContentWidth_, ' ')),
            Color::NORMAL
        );
    }
    else {
        draw::TableData(
            s_experimentLeftMargin_ + 1 + s_experimentPilePadding_,
            s_upperMargin_ + 1 + screenPileIndex * (s_experimentPileContentHeight_ + 1),
            vector<string>(s_experimentPileContentHeight_, string(s_experimentPileContentWidth_, ' ')),
            Color::NORMAL
        );
    }
}

void Interface::__Execute(bool onDateTable, const vector<Action>& actions)
{
    Operation _op;
    for (const auto& action : actions) {
        _op = action.operation;
        if (_op == Operation::TO_NORMAL) __DrawWholeTablePile(onDateTable, Color::NORMAL, action.value1, action.value2);
        else if (_op == Operation::TO_FOCUSED) __DrawWholeTablePile(onDateTable, s_colorFocused_, action.value1, action.value2);
        else if (_op == Operation::SHIFT_PILES) {
            int _upperPileIndex = action.value1,
                _cursorPileIndex = action.value2,
                _tablePileLimit;
            Color _color;
            if (onDateTable) _tablePileLimit = s_dateTablePileLimit_;
            else _tablePileLimit = s_experimentTablePileLimit_;
            for (int i = 0; i < _tablePileLimit; ++i) {
               _color = (_upperPileIndex + i == _cursorPileIndex) ? s_colorFocused_ : Color::NORMAL;
                __DrawWholeTablePile(onDateTable, _color, i, _upperPileIndex + i);
            }
        }
        else if (_op == Operation::ADD_SLIDER_PART) {
            int _x = s_dateLeftMargin_ + s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2,
                _y = s_upperMargin_ + 1 + action.value1;
            if (!onDateTable) _x += s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 4;
            __DrawVerticalLine(_x, _y, 1, s_colorSlider_);
        }
        else if (_op == Operation::REMOVE_SLIDER_PART) {
            int _x = s_dateLeftMargin_ + s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2,
                _y = s_upperMargin_ + 1 + action.value1;
            if (!onDateTable) _x += s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 4;
            __DrawVerticalLine(_x, _y, 1, s_colorScrollbar_);
        }
    }
}

void Interface::__LoadNewExperiments()
{
    time_t currentDateTT = FileHandler::s_currExpAmountsByDatesIter->first;
    s_selectedDateExperiments_.clear();
    for (int no = 1; no <= FileHandler::s_currExpAmountsByDatesIter->second; ++no) {
        s_selectedDateExperiments_.push_back(__GetExperimentInfo(currentDateTT + no));
    }
    bool prevTableScrollbar = s_listExperiments_.value().scrollbar;
    s_listExperiments_ = ScrollableList(s_selectedDateExperiments_.size(), s_experimentTablePileLimit_, s_experimentTableHeight_, 0, 0);

    for (int pile = 0; pile < min(s_experimentTablePileLimit_, (int) s_selectedDateExperiments_.size()); ++pile) {
        __DrawTableData(false, Color::NORMAL, pile, pile);
    }
    for (int pile = min(s_experimentTablePileLimit_, (int) s_selectedDateExperiments_.size()); pile < s_experimentTablePileLimit_; ++pile) {
        __ClearPile(false, pile);
    }
    if (s_listExperiments_.value().scrollbar) __DrawScrollbar(false);
    else if (prevTableScrollbar) __RemoveExperimentTableScrollbar();
}

void Interface::__GetLastClickInfo()
{
    // > 0: номер плитки в списке дат, начиная с единицы
    // < 0: модуль номера плитки в списке экспериментов, начииная с единицы
    // == 0: вне списков
    clickInfo.reset();
    int x = MouseInput::X,
        y = MouseInput::Y;
    if (s_dateLeftMargin_ <= x && x <= s_dateLeftMargin_ + 1 + s_datePileContentWidth_ + 2 * s_datePilePadding_ &&
        s_upperMargin_ <= y && y <= s_upperMargin_ + s_dateTableHeight_ - 1)
    {
        clickInfo.bDatesList = true;
        if (x == s_dateLeftMargin_ || x == s_dateLeftMargin_ + 1 + s_datePileContentWidth_ + 2 * s_datePilePadding_ ||
            y == s_upperMargin_ || y == s_upperMargin_ + s_dateTableHeight_ - 1)
        {
            clickInfo.isBorder = true;
        }
        else if ((y - s_upperMargin_) % (s_datePileContentHeight_ + 1))
            clickInfo.screenPileIndex = (y - s_upperMargin_) / (s_datePileContentHeight_ + 1);
        else clickInfo.isInnerBorder = true;
    }
    else if (s_experimentLeftMargin_ <= x && x <= s_experimentLeftMargin_ + 1 + s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ &&
             s_upperMargin_ <= y && y <= s_upperMargin_ + s_experimentTableHeight_ - 1)
    {
        clickInfo.bExperimentsList = true;
        if (x == s_experimentLeftMargin_ || x == s_experimentLeftMargin_ + 1 + s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ ||
            y == s_upperMargin_ || y == s_upperMargin_ + s_experimentTableHeight_ - 1)
        {
            clickInfo.isBorder = true;
        }
        else if ((y - s_upperMargin_) % (s_experimentPileContentHeight_ + 1))
            clickInfo.screenPileIndex = (y - s_upperMargin_) / (s_experimentPileContentHeight_ + 1);
        else clickInfo.isInnerBorder = true;
    }
}
