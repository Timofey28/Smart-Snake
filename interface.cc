#include "interface.h"
using namespace std;

int Interface::s_dateTableHeight_, Interface::s_experimentTableHeight_, Interface::s_gameTableHeight_;
int Interface::s_dateTablePileLimit_, Interface::s_experimentTablePileLimit_, Interface::s_gameTablePileLimit_;
int Interface::s_experimentLeftMargin_;
int Interface::s_gameLeftMargin_;
int Interface::s_gamePileContentWidth_;
bool Interface::s_focusOnDatesList_ = true;
unordered_map<time_t, Experiment> Interface::s_experiments_;
vector<Experiment> Interface::s_selectedDateExperiments_;
vector<Game> Interface::s_selectedGames_;
optional<ScrollableList> Interface::s_listDates_, Interface::s_listExperiments_, Interface::s_listGames_;
ClickInfo Interface::clickInfo;
int Interface::s_chosenExperimentIndex_, Interface::s_chosenGameIndex_;
pair<int, int> Interface::s_coordsGameNo_, Interface::s_coordsMovesAmount_, Interface::s_coordsAvgMovesToFood_, Interface::s_coordsSortingOrder_;
Sorting Interface::s_gameListSorting_;
map<Sorting, string> Interface::s_sortingName_ = {
    {Sorting::CHRONOLOGICAL, "хронологический  "},
    {Sorting::ASC,           "возрастание очков"},
    {Sorting::DESC,          "убывание очков   "},
};

void Interface::__InitTableDimensions()
{
    // Фактическая высота таблиц
    int maxTableHeight = Console::s_dimensions.height - s_upperMargin_ * 2;
    int pileHeight = s_datePileContentHeight_ + 1;
    s_dateTableHeight_ = maxTableHeight - (pileHeight + (maxTableHeight % pileHeight) - 1) % pileHeight;
    pileHeight = s_experimentPileContentHeight_ + 1;
    s_experimentTableHeight_ = maxTableHeight - (pileHeight + (maxTableHeight % pileHeight) - 1) % pileHeight;
    pileHeight = s_gamePileContentHeight_ + 1;
    s_gameTableHeight_ = maxTableHeight - (pileHeight + (maxTableHeight % pileHeight) - 1) % pileHeight;
    assert(s_experimentTableHeight_ >= 1);
    assert(s_gameTableHeight_ >= 1);

    // Максимальное число отображаемых в один момент плиток в таблице
    s_dateTablePileLimit_ = s_dateTableHeight_ / (s_datePileContentHeight_ + 1);
    s_experimentTablePileLimit_ = s_experimentTableHeight_ / (s_experimentPileContentHeight_ + 1);
    s_gameTablePileLimit_ = s_gameTableHeight_ / (s_gamePileContentHeight_ + 1);
}

bool Interface::DatesAndExperimentsList()
{
    __InitTableDimensions();

    system("cls");
    if (FileHandler::s_experimentAmountsByDates.empty()) {
        cout << "\n\tИгры не найдены.";
        this_thread::sleep_for(1000ms);
        return false;
    }

    bool firstTime = false;
    if (FileHandler::s_currExpAmountsByDatesIter == FileHandler::s_experimentAmountsByDates.end()) {
        firstTime = true;
        FileHandler::s_currExpAmountsByDatesIter = FileHandler::s_experimentAmountsByDates.begin();
        time_t latestDateTT = FileHandler::s_currExpAmountsByDatesIter->first;
        s_selectedDateExperiments_.clear();
        for (int no = 1; no <= FileHandler::s_currExpAmountsByDatesIter->second; ++no) {
            s_selectedDateExperiments_.push_back(__GetExperimentInfo(latestDateTT + no));
        }

        s_listDates_ = ScrollableList(FileHandler::s_experimentAmountsByDates.size(), s_dateTablePileLimit_, s_dateTableHeight_, 0, 0);
        s_listExperiments_ = ScrollableList(s_selectedDateExperiments_.size(), s_experimentTablePileLimit_, s_experimentTableHeight_, 0, 0);
        s_experimentLeftMargin_ = s_dateLeftMargin_ + s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2 + s_listDates_.value().scrollbar + 1;
    }

    Color _highlightColor = firstTime ? s_colorFocused_ : s_colorSelected_;
    draw::Box<BoxStyle>(
        s_dateLeftMargin_, s_upperMargin_,
        s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2, s_dateTableHeight_,
        s_datePileContentHeight_, FileHandler::s_dateFolders.size(), _highlightColor,
        s_listDates_.value().ScreenPileIndex()
    );
    int pilesShown = min(s_dateTablePileLimit_, (int) FileHandler::s_dateFolders.size());
    for (int pile = s_listDates_.value().upperPileIndex; pile < s_listDates_.value().upperPileIndex + pilesShown; ++pile) {
        Color _color = (pile == s_listDates_.value().cursorPileIndex) ? _highlightColor : Color::NORMAL;
        __DrawTableData(Table::DAT, _color, pile - s_listDates_.value().upperPileIndex, pile);
    }

    draw::Box<BoxStyle>(
        s_experimentLeftMargin_, s_upperMargin_,
        s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 2, s_experimentTableHeight_,
        s_experimentPileContentHeight_, s_selectedDateExperiments_.size(), s_colorFocused_,
        firstTime ? -1 : s_listExperiments_.value().ScreenPileIndex()
    );
    pilesShown = min(s_experimentTablePileLimit_, (int) s_selectedDateExperiments_.size());
    for (int pile = s_listExperiments_.value().upperPileIndex; pile < s_listExperiments_.value().upperPileIndex + pilesShown; ++pile) {
        Color _color = (!firstTime && pile == s_listExperiments_.value().cursorPileIndex) ? s_colorFocused_ : Color::NORMAL;
        __DrawTableData(Table::EXPERIMENT, _color, pile - s_listExperiments_.value().upperPileIndex, pile);
    }

    if (s_listDates_.value().scrollbar) __DrawScrollbar(Table::DAT);
    if (s_listExperiments_.value().scrollbar) __DrawScrollbar(Table::EXPERIMENT);

    vector<Action> actions;
    while (true) {
        MouseInput::GetAnyEventInfo();
        ButtonPressed bp = MouseInput::buttonPressed;
        if (MouseInput::isKeyboardEvent) {
            if (bp == ButtonPressed::ARROW_LEFT) {
                if (!s_focusOnDatesList_) {
                    s_focusOnDatesList_ = true;
                    __DrawWholeTablePile(Table::EXPERIMENT, Color::NORMAL);
                    __DrawWholeTablePile(Table::DAT, s_colorFocused_);
                }
            }

            else if (bp == ButtonPressed::ARROW_RIGHT) {
                if (s_focusOnDatesList_) {
                    s_focusOnDatesList_ = false;
                    __DrawWholeTablePile(Table::DAT, s_colorSelected_);
                    __DrawWholeTablePile(Table::EXPERIMENT, s_colorFocused_);
                }
            }

            else if (bp == ButtonPressed::ARROW_UP) {
                if (s_focusOnDatesList_) {
                    actions = s_listDates_.value().ReactTo(Event::EV_ARROW_UP);
                    __Execute(Table::DAT, actions);
                    if (actions.size()) {
                        assert(FileHandler::s_currExpAmountsByDatesIter != FileHandler::s_experimentAmountsByDates.begin());
                        FileHandler::s_currExpAmountsByDatesIter--;
                        __LoadNewExperiments();
                    }
                }
                else {
                    actions = s_listExperiments_.value().ReactTo(Event::EV_ARROW_UP);
                    __Execute(Table::EXPERIMENT, actions);
                }
            }

            else if (bp == ButtonPressed::ARROW_DOWN) {
                if (s_focusOnDatesList_) {
                    actions = s_listDates_.value().ReactTo(Event::EV_ARROW_DOWN);
                    __Execute(Table::DAT, actions);
                    if (actions.size()) {
                        FileHandler::s_currExpAmountsByDatesIter++;
                        assert(FileHandler::s_currExpAmountsByDatesIter != FileHandler::s_experimentAmountsByDates.end());
                        __LoadNewExperiments();
                    }
                }
                else {
                    actions = s_listExperiments_.value().ReactTo(Event::EV_ARROW_DOWN);
                    __Execute(Table::EXPERIMENT, actions);
                }
            }

            else if (bp == ButtonPressed::BACKSPACE || bp == ButtonPressed::ESCAPE) {
                s_selectedDateExperiments_.clear();
                s_listDates_.reset();
                s_listExperiments_.reset();
                FileHandler::s_currExpAmountsByDatesIter = FileHandler::s_experimentAmountsByDates.end();
                return false;
            }

            else if (bp == ButtonPressed::ENTER) {
                if (s_focusOnDatesList_) {
                    s_focusOnDatesList_ = false;
                    __DrawWholeTablePile(Table::DAT, s_colorSelected_);
                    __DrawWholeTablePile(Table::EXPERIMENT, s_colorFocused_);
                }
                else {
                    s_chosenExperimentIndex_ = s_listExperiments_.value().cursorPileIndex;
                    return true;
                }
            }
        }

        else {  // Mouse event
            __GetLastClickInfo(Window::EXPERIMENT_SELECTION_MENU);
            if (!clickInfo.bDatesList && !clickInfo.bExperimentsList && bp != ButtonPressed::WHEEL) continue;

            bool changedList = false;
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
                    if (changedList) {
                        __DrawWholeTablePile(Table::DAT, s_colorFocused_);
                        __DrawWholeTablePile(Table::EXPERIMENT, Color::NORMAL);
                    }
                    actions = s_listDates_.value().ReactTo(Event::EV_WHEEL_UP);
                    __Execute(Table::DAT, actions);
                    if (actions.size()) {
                        assert(FileHandler::s_currExpAmountsByDatesIter != FileHandler::s_experimentAmountsByDates.begin());
                        FileHandler::s_currExpAmountsByDatesIter--;
                        __LoadNewExperiments();
                    }
                }
                else {
                    if (changedList) {
                        __DrawWholeTablePile(Table::DAT, s_colorSelected_);
                        __DrawWholeTablePile(Table::EXPERIMENT, s_colorFocused_);
                    }
                    actions = s_listExperiments_.value().ReactTo(Event::EV_WHEEL_UP);
                    __Execute(Table::EXPERIMENT, actions);
                }
            }

            else if (bp == ButtonPressed::WHEEL_DOWN) {
                if (s_focusOnDatesList_) {
                    if (changedList) {
                        __DrawWholeTablePile(Table::DAT, s_colorFocused_);
                        __DrawWholeTablePile(Table::EXPERIMENT, Color::NORMAL);
                    }
                    actions = s_listDates_.value().ReactTo(Event::EV_WHEEL_DOWN);
                    __Execute(Table::DAT, actions);
                    if (actions.size()) {
                        FileHandler::s_currExpAmountsByDatesIter++;
                        assert(FileHandler::s_currExpAmountsByDatesIter != FileHandler::s_experimentAmountsByDates.end());
                        __LoadNewExperiments();
                    }
                }
                else {
                    if (changedList) {
                        __DrawWholeTablePile(Table::DAT, s_colorSelected_);
                        __DrawWholeTablePile(Table::EXPERIMENT, s_colorFocused_);
                    }
                    actions = s_listExperiments_.value().ReactTo(Event::EV_WHEEL_DOWN);
                    __Execute(Table::EXPERIMENT, actions);
                }
            }

            else if (bp == ButtonPressed::LEFT_BUTTON && !clickInfo.isBorder && !clickInfo.isInnerBorder) {
                if (s_focusOnDatesList_) {
                    if (changedList && clickInfo.screenPileIndex != s_listDates_.value().ScreenPileIndex()) {
                        __DrawBoxPile(Table::EXPERIMENT, Color::NORMAL, s_listExperiments_.value().ScreenPileIndex());
                    }
                    if (s_listDates_.value().ScreenPileIndex() == clickInfo.screenPileIndex) {
                        s_focusOnDatesList_ = false;
                        __DrawWholeTablePile(Table::DAT, s_colorSelected_);
                        __DrawWholeTablePile(Table::EXPERIMENT, s_colorFocused_);
                    }
                    else {
                        actions = s_listDates_.value().ReactTo(Event::EV_CLICK_PILE, clickInfo.screenPileIndex);
                        __Execute(Table::DAT, actions);
                        if (actions.size()) {
                            int shift = clickInfo.screenPileIndex - actions[0].value1;
                            assert(shift != 0);
                            FileHandler::s_currExpAmountsByDatesIter = next(FileHandler::s_currExpAmountsByDatesIter, shift);
                            __LoadNewExperiments();
                        }
                    }
                }
                else if (s_listExperiments_.value().upperPileIndex + clickInfo.screenPileIndex < s_selectedDateExperiments_.size()) {
                    s_listExperiments_.value().cursorPileIndex = s_listExperiments_.value().upperPileIndex + clickInfo.screenPileIndex;
                    s_chosenExperimentIndex_ = s_listExperiments_.value().upperPileIndex + clickInfo.screenPileIndex;
                    return true;
                }
            }

            else if (bp == ButtonPressed::WHEEL) {
                s_selectedDateExperiments_.clear();
                s_listDates_.reset();
                s_listExperiments_.reset();
                FileHandler::s_currExpAmountsByDatesIter = FileHandler::s_experimentAmountsByDates.end();
                return false;
            }
        }
    }
}

Result Interface::GamesList()
{
    system("cls");
    assert(Console::s_dimensions.height >= 18);
    Experiment& exp = s_selectedDateExperiments_[s_chosenExperimentIndex_];
    s_gamePileContentWidth_ = exp.CalculateGamePileContentWidth();

    int maxPossibleScore = exp.maxPossibleSnakeLength - exp.initialSnakeLength;
    int maxLength = s_gameMenuLeftMargin_ + 30;  // 30 - длина фразы "Порядок игр: возрастание очков"
    string line;
    cout << "\n" << string(s_gameMenuLeftMargin_, ' ') << "Эксперимент #" << s_chosenExperimentIndex_ + 1;
    cout << "\n\n" << string(s_gameMenuLeftMargin_, ' ') << "Количество игр: " << exp.gamesAmount;
    cout << "\n" << string(s_gameMenuLeftMargin_, ' ') << "Размер поля: " << exp.fieldWidth - 2 << "x" << exp.fieldHeight - 2;
    cout << "\n" << string(s_gameMenuLeftMargin_, ' ') << "Начальная длина: " << exp.initialSnakeLength;
    cout << "\n" << string(s_gameMenuLeftMargin_, ' ') << "Победа при: " << exp.maxPossibleSnakeLength;
    line = makeStr(string(s_gameMenuLeftMargin_, ' '), "Лучший р-т: ", exp.bestScore, "/", maxPossibleScore, " (игра #", exp.bestScoreNo + 1, ")");
    maxLength = max(maxLength, (int) line.size());
    cout << "\n" << line;
    line = makeStr(string(s_gameMenuLeftMargin_, ' '), "Худший р-т: ", exp.worstScore, "/", maxPossibleScore, " (игра #", exp.worstScoreNo + 1, ")");
    maxLength = max(maxLength, (int) line.size());
    cout << "\n" << line;
    line = makeStr(string(s_gameMenuLeftMargin_, ' '), "Средний разброс: ", doubleToStr(exp.avgScore, 1));
    int mixedLineLength = line.size();
    cout << "\n" << line;
    _setmode(_fileno(stdout), _O_U16TEXT);
    wcout << ' ' << Symbols::PLUS_MINUS << ' ';
    mixedLineLength += 3;
    _setmode(_fileno(stdout), _O_TEXT);
    line = doubleToStr(exp.stdScore, 1);
    mixedLineLength += line.size();
    maxLength = max(maxLength, mixedLineLength);
    cout << line;
    s_gameLeftMargin_ = maxLength + 4;

    cout << "\n" << string(s_gameMenuLeftMargin_, ' ');
    _setmode(_fileno(stdout), _O_U16TEXT);
    wcout << wstring(maxLength - s_gameMenuLeftMargin_, Symbols::HORIZONTAL_DOUBLE_LINE);
    _setmode(_fileno(stdout), _O_TEXT);

    if (!s_listGames_) {
        s_listGames_ = ScrollableList(exp.gamesAmount, s_gameTablePileLimit_, s_gameTableHeight_, 0, 0);
        assert(s_gameLeftMargin_ + s_gamePileContentWidth_ + 2 * s_gamePilePadding_ + 2 + 2 * s_listGames_.value().scrollbar <= Console::s_dimensions.width);
        assert(s_selectedGames_.empty());
        s_selectedGames_.reserve(exp.gamesAmount);
        for (int i = 0; i < exp.gamesAmount; ++i) {
            s_selectedGames_.push_back(Game(exp.Path() / (to_string(i + 1) + ".txt"), exp.gameScores[i], false));
        }
        s_selectedGames_[0].Load();
        s_gameListSorting_ = Sorting::CHRONOLOGICAL;
    }

    cout << "\n" << string(s_gameMenuLeftMargin_, ' ') << "Игра #" << s_selectedGames_[s_listGames_.value().cursorPileIndex].no;
    s_coordsGameNo_ = {s_gameMenuLeftMargin_ + 6, 11};
    cout << "\n" << string(s_gameMenuLeftMargin_, ' ') << "Поползновений: " << s_selectedGames_[s_listGames_.value().cursorPileIndex].MovesAmount();
    s_coordsMovesAmount_ = {s_gameMenuLeftMargin_ + 15, 12};
    cout << "\n" << string(s_gameMenuLeftMargin_, ' ') << "В среднем до еды: ";
    if (s_selectedGames_[s_listGames_.value().cursorPileIndex].AvgMovesToFood() == -1) cout << "...";
    else cout << doubleToStr(s_selectedGames_[s_listGames_.value().cursorPileIndex].AvgMovesToFood());
    s_coordsAvgMovesToFood_ = {s_gameMenuLeftMargin_ + 18, 13};

    cout << "\n" << string(s_gameMenuLeftMargin_, ' ');
    _setmode(_fileno(stdout), _O_U16TEXT);
    wcout << wstring(maxLength - s_gameMenuLeftMargin_, Symbols::HORIZONTAL_DOUBLE_LINE);
    _setmode(_fileno(stdout), _O_TEXT);
    cout << "\n" << string(s_gameMenuLeftMargin_, ' ') << "Порядок игр: ";
    s_coordsSortingOrder_ = {s_gameMenuLeftMargin_ + 13, 15};
    setColor(s_colorSortingOrder_);
    cout << s_sortingName_[s_gameListSorting_];
    setColor(Color::GRAY_ON_BLACK);
    cout << "\n" << string(s_gameMenuLeftMargin_, ' ') << "Ctrl, пкм - изменить порядок";
    setColor(Color::NORMAL);

    draw::Box<BoxStyle>(
        s_gameLeftMargin_, s_upperMargin_,
        s_gamePileContentWidth_ + 2 * s_gamePilePadding_ + 2, s_gameTableHeight_,
        s_gamePileContentHeight_, s_selectedGames_.size(), s_colorFocused_,
        s_listGames_.value().ScreenPileIndex()
    );
    int pilesShown = min(s_gameTablePileLimit_, (int) s_selectedGames_.size());
    for (int pile = s_listGames_.value().upperPileIndex; pile < s_listGames_.value().upperPileIndex + pilesShown; ++pile) {
        Color _color = (pile == s_listGames_.value().cursorPileIndex) ? s_colorFocused_ : Color::NORMAL;
        __DrawTableData(Table::GAME, _color, pile - s_listGames_.value().upperPileIndex, pile);
    }
    if (s_listGames_.value().scrollbar) __DrawScrollbar(Table::GAME);

    vector<Action> actions;
    while (true) {
        MouseInput::GetAnyEventInfo();
        ButtonPressed bp = MouseInput::buttonPressed;
        if (MouseInput::isKeyboardEvent) {
            if (bp == ButtonPressed::ARROW_UP) {
                actions = s_listGames_.value().ReactTo(Event::EV_ARROW_UP);
                __Execute(Table::GAME, actions);
                if (actions.size()) {
                    s_selectedGames_[s_listGames_.value().cursorPileIndex].Load();
                    int _gameNo = s_selectedGames_[s_listGames_.value().cursorPileIndex].no,
                        _movesAmount = s_selectedGames_[s_listGames_.value().cursorPileIndex].MovesAmount();
                    double _avgMovesToFood = s_selectedGames_[s_listGames_.value().cursorPileIndex].AvgMovesToFood();
                    __SetDetailedGameInfo(_gameNo, _movesAmount, _avgMovesToFood);
                }
            }

            else if (bp == ButtonPressed::ARROW_DOWN) {
                actions = s_listGames_.value().ReactTo(Event::EV_ARROW_DOWN);
                __Execute(Table::GAME, actions);
                if (actions.size()) {
                    s_selectedGames_[s_listGames_.value().cursorPileIndex].Load();
                    int _gameNo = s_selectedGames_[s_listGames_.value().cursorPileIndex].no,
                        _movesAmount = s_selectedGames_[s_listGames_.value().cursorPileIndex].MovesAmount();
                    double _avgMovesToFood = s_selectedGames_[s_listGames_.value().cursorPileIndex].AvgMovesToFood();
                    __SetDetailedGameInfo(_gameNo, _movesAmount, _avgMovesToFood);
                }
            }

            else if (bp == ButtonPressed::CTRL) {
                __SetNextSortingOrder();
                actions = s_listGames_.value().ReactTo(Event::EV_CTRL_OR_RMB);
                __Execute(Table::GAME, actions);
                if (actions.size()) {
                    s_selectedGames_[s_listGames_.value().cursorPileIndex].Load();
                    int _gameNo = s_selectedGames_[s_listGames_.value().cursorPileIndex].no,
                        _movesAmount = s_selectedGames_[s_listGames_.value().cursorPileIndex].MovesAmount();
                    double _avgMovesToFood = s_selectedGames_[s_listGames_.value().cursorPileIndex].AvgMovesToFood();
                    __SetDetailedGameInfo(_gameNo, _movesAmount, _avgMovesToFood);
                }
            }

            else if (bp == ButtonPressed::ENTER) {
                s_chosenGameIndex_ = s_listGames_.value().cursorPileIndex;
                return Result::GAME_CHOSEN;
            }

            else if (bp == ButtonPressed::BACKSPACE || bp == ButtonPressed::ARROW_LEFT) {
                s_listGames_.reset();
                s_selectedGames_.clear();
                return Result::BACK;
            }

            else if (bp == ButtonPressed::ESCAPE) {
                s_selectedDateExperiments_.clear();
                s_selectedGames_.clear();
                s_listDates_.reset();
                s_listExperiments_.reset();
                s_listGames_.reset();
                FileHandler::s_currExpAmountsByDatesIter = FileHandler::s_experimentAmountsByDates.end();
                return Result::EXIT;
            }
        }

        else {  // Mouse event
            __GetLastClickInfo(Window::GAME_SELECTION_MENU);
            if (!clickInfo.bGamesList && bp != ButtonPressed::WHEEL) continue;

            if (bp == ButtonPressed::WHEEL_UP) {
                actions = s_listGames_.value().ReactTo(Event::EV_WHEEL_UP);
                __Execute(Table::GAME, actions);
                if (actions.size()) {
                    s_selectedGames_[s_listGames_.value().cursorPileIndex].Load();
                    int _gameNo = s_selectedGames_[s_listGames_.value().cursorPileIndex].no,
                        _movesAmount = s_selectedGames_[s_listGames_.value().cursorPileIndex].MovesAmount();
                    double _avgMovesToFood = s_selectedGames_[s_listGames_.value().cursorPileIndex].AvgMovesToFood();
                    __SetDetailedGameInfo(_gameNo, _movesAmount, _avgMovesToFood);
                }
            }

            else if (bp == ButtonPressed::WHEEL_DOWN) {
                actions = s_listGames_.value().ReactTo(Event::EV_WHEEL_DOWN);
                __Execute(Table::GAME, actions);
                if (actions.size()) {
                    s_selectedGames_[s_listGames_.value().cursorPileIndex].Load();
                    int _gameNo = s_selectedGames_[s_listGames_.value().cursorPileIndex].no,
                        _movesAmount = s_selectedGames_[s_listGames_.value().cursorPileIndex].MovesAmount();
                    double _avgMovesToFood = s_selectedGames_[s_listGames_.value().cursorPileIndex].AvgMovesToFood();
                    __SetDetailedGameInfo(_gameNo, _movesAmount, _avgMovesToFood);
                }
            }

            else if (bp == ButtonPressed::RIGHT_BUTTON) {
                __SetNextSortingOrder();
                actions = s_listGames_.value().ReactTo(Event::EV_CTRL_OR_RMB);
                __Execute(Table::GAME, actions);
                if (actions.size()) {
                    s_selectedGames_[s_listGames_.value().cursorPileIndex].Load();
                    int _gameNo = s_selectedGames_[s_listGames_.value().cursorPileIndex].no,
                        _movesAmount = s_selectedGames_[s_listGames_.value().cursorPileIndex].MovesAmount();
                    double _avgMovesToFood = s_selectedGames_[s_listGames_.value().cursorPileIndex].AvgMovesToFood();
                    __SetDetailedGameInfo(_gameNo, _movesAmount, _avgMovesToFood);
                }
            }

            else if (bp == ButtonPressed::LEFT_BUTTON && !clickInfo.isBorder && !clickInfo.isInnerBorder) {
                s_listGames_.value().cursorPileIndex = s_listGames_.value().upperPileIndex + clickInfo.screenPileIndex;
                s_chosenGameIndex_ = s_listGames_.value().upperPileIndex + clickInfo.screenPileIndex;
                return Result::GAME_CHOSEN;
            }

            else if (bp == ButtonPressed::WHEEL) {
                s_listGames_.reset();
                s_selectedGames_.clear();
                return Result::BACK;
            }
        }
    }
}

void Interface::RunGame()
{
    system("cls");
    Game& game = s_selectedGames_[s_chosenGameIndex_];
    game.Load();
    game.PrintFirstFrame();

    while (true) {
        MouseInput::GetAnyEventInfo();
        ButtonPressed bp = MouseInput::buttonPressed;
        if (MouseInput::isKeyboardEvent) {
            if (bp == ButtonPressed::ARROW_LEFT) {
                if (!game.PrintPreviousFrame());
            }
            else if (bp == ButtonPressed::ARROW_RIGHT) {
                if (!game.PrintNextFrame());
            }
            else if (bp == ButtonPressed::ESCAPE) {
                setColor(Color::NORMAL);
                return;
            }
        }
    }

    this_thread::sleep_for(100ms);
    MouseInput::WaitForAnyEvent();
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
    int maxPossibleGain = e.maxPossibleSnakeLength - e.initialSnakeLength;
    double bestToMaxPossiblePercent = (double) e.bestScore * 100 / maxPossibleGain;
    vector<string> result = {
//                to_string(cursorPileIndex + 1) + ". Создан в " + timestampToHourMinuteStr(e.creationTime),
        to_string(cursorPileIndex + 1) + ". Поле " + to_string(e.fieldWidth - 2) + "x" + to_string(e.fieldHeight - 2) + ", " + timestampToHourMinuteStr(e.creationTime),
//                to_string(cursorPileIndex + 1) + ". " + timestampToHourMinuteStr(e.creationTime) + ", поле " + to_string(e.fieldWidth - 2) + "x" + to_string(e.fieldHeight - 2),
        "  Игр: " + to_string(e.gamesAmount),
        "  Нач. длина змеи: " + to_string(e.initialSnakeLength),
        "  Макс. длина змеи: " + to_string(e.maxPossibleSnakeLength),
        "  Лучшая: " + to_string(e.bestScore) + "/" + to_string(maxPossibleGain) + " (" + doubleToStr(bestToMaxPossiblePercent, 0) + "%)",
        "  В среднем: " + doubleToStr(e.avgScore),
    };
    for (int i = 0; i < result.size(); ++i) {
        assert(result[i].size() <= s_experimentPileContentWidth_);
        result[i] += string(s_experimentPileContentWidth_ - result[i].size(), ' ');
    }
    return result;
}

vector<string> Interface::__MakePileData(int gameNo, int score, int maxPossibleScore)
{
    int percent = score * 100 / maxPossibleScore;
    string result = format("{}. {}/{}, {}%", gameNo, score, maxPossibleScore, percent);
    assert(s_gamePileContentWidth_ >= result.size());
    result += string(s_gamePileContentWidth_ - result.size(), ' ');
    return {result};
}

void Interface::__DrawTableData(Table table, Color color, optional<int> screenPileIndex, optional<int> cursorPileIndex)
{
    int _leftMargin, _pileContentHeight, _pilePadding, _screenPileIndex, _cursorPileIndex;
    vector<string> data;
    if (table == Table::DAT) {
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
    else if (table == Table::EXPERIMENT) {
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
    else if (table == Table::GAME) {
        _leftMargin = s_gameLeftMargin_;
        _pileContentHeight = s_gamePileContentHeight_;
        _pilePadding = s_gamePilePadding_;
        if (screenPileIndex) {
            _screenPileIndex = screenPileIndex.value();
            _cursorPileIndex = cursorPileIndex.value_or(s_listGames_.value().cursorPileIndex);
            data = __MakePileData(
                s_selectedGames_[_cursorPileIndex].no,
                s_selectedGames_[_cursorPileIndex].score,
                s_selectedDateExperiments_[s_chosenExperimentIndex_].maxPossibleSnakeLength -
                    s_selectedDateExperiments_[s_chosenExperimentIndex_].initialSnakeLength
            );
        }
        else {
            _screenPileIndex = s_listGames_.value().ScreenPileIndex();
            _cursorPileIndex = s_listGames_.value().cursorPileIndex;
            data = __MakePileData(
                s_selectedGames_[_cursorPileIndex].no,
                s_selectedGames_[_cursorPileIndex].score,
                s_selectedDateExperiments_[s_chosenExperimentIndex_].maxPossibleSnakeLength -
                    s_selectedDateExperiments_[s_chosenExperimentIndex_].initialSnakeLength
            );
        }
    }

    draw::TableData(
        _leftMargin + 1 + _pilePadding,
        s_upperMargin_ + 1 + _screenPileIndex * (_pileContentHeight + 1),
        data,
        color
    );
}

void Interface::__DrawBoxPile(Table table, Color color, optional<int> screenPileIndex, bool careful)
{
    int _screenPileIndex;
    if (table == Table::DAT) {
        _screenPileIndex = screenPileIndex.value_or(s_listDates_.value().ScreenPileIndex());
        draw::BoxPile<BoxStyle>(
            s_dateLeftMargin_,
            s_upperMargin_ + _screenPileIndex * (s_datePileContentHeight_ + 1),
            s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2,
            s_datePileContentHeight_ + 2,
            color,
            _screenPileIndex == 0,
            _screenPileIndex == s_dateTablePileLimit_ - 1,
            careful
        );
    }
    else if (table == Table::EXPERIMENT) {
        _screenPileIndex = screenPileIndex.value_or(s_listExperiments_.value().ScreenPileIndex());
        draw::BoxPile<BoxStyle>(
            s_experimentLeftMargin_,
            s_upperMargin_ + _screenPileIndex * (s_experimentPileContentHeight_ + 1),
            s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 2,
            s_experimentPileContentHeight_ + 2,
            color,
            _screenPileIndex == 0,
            _screenPileIndex == s_experimentTablePileLimit_ - 1,
            careful
        );
    }
    else if (table == Table::GAME) {
        _screenPileIndex = screenPileIndex.value_or(s_listGames_.value().ScreenPileIndex());
        draw::BoxPile<BoxStyle>(
            s_gameLeftMargin_,
            s_upperMargin_ + _screenPileIndex * (s_gamePileContentHeight_ + 1),
            s_gamePileContentWidth_ + 2 * s_gamePilePadding_ + 2,
            s_gamePileContentHeight_ + 2,
            color,
            _screenPileIndex == 0,
            _screenPileIndex == s_gameTablePileLimit_ - 1,
            careful
        );
    }
}

void Interface::__DrawWholeTablePile(Table table, Color color, optional<int> screenPileIndex, optional<int> cursorPileIndex, bool careful)
{
    __DrawBoxPile(table, color, screenPileIndex, careful);
    __DrawTableData(table, color, screenPileIndex, cursorPileIndex);
}

void Interface::__DrawVerticalLine(int x, int y, int length, Color color)
{
    if (length <= 0) return;
    vector<pair<int, int>> coords;
    coords.reserve(length);
    for (int i = y; i < y + length; ++i) coords.push_back({x, i});
    draw::Symbol(coords, color);
}

void Interface::__DrawScrollbar(Table table)
{
    int _x, _scrollbarHeight, _sliderHeight, _upperPileIndex;
    if (table == Table::DAT) {
        _x = s_dateLeftMargin_ + s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2;
        _scrollbarHeight = s_listDates_.value().scrollbarHeight;
        _sliderHeight = s_listDates_.value().sliderHeight;
        _upperPileIndex = s_listDates_.value().upperPileIndex;
    }
    else if (table == Table::EXPERIMENT) {
        _x = s_experimentLeftMargin_ + s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 2;
        _scrollbarHeight = s_listExperiments_.value().scrollbarHeight;
        _sliderHeight = s_listExperiments_.value().sliderHeight;
        _upperPileIndex = s_listExperiments_.value().upperPileIndex;
    }
    else if (table == Table::GAME) {
        _x = s_gameLeftMargin_ + s_gamePileContentWidth_ + 2 * s_gamePilePadding_ + 2;
        _scrollbarHeight = s_listGames_.value().scrollbarHeight;
        _sliderHeight = s_listGames_.value().sliderHeight;
        _upperPileIndex = s_listGames_.value().upperPileIndex;
    }
    __DrawVerticalLine(_x, s_upperMargin_ + 1, _upperPileIndex, s_colorScrollbar_);
    __DrawVerticalLine(_x, s_upperMargin_ + 1 + _upperPileIndex, _sliderHeight, s_colorSlider_);
    __DrawVerticalLine(_x, s_upperMargin_ + 1 + _upperPileIndex + _sliderHeight, _scrollbarHeight - _upperPileIndex - _sliderHeight, s_colorScrollbar_);
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

void Interface::__ClearPile(Table table, int screenPileIndex)
{
    if (table == Table::DAT) {
        draw::TableData(
            s_dateLeftMargin_+ 1 + s_datePilePadding_,
            s_upperMargin_ + 1 + screenPileIndex * (s_datePileContentHeight_ + 1),
            vector<string>(s_datePileContentHeight_, string(s_datePileContentWidth_, ' ')),
            Color::NORMAL
        );
    }
    else if (table == Table::EXPERIMENT) {
        draw::TableData(
            s_experimentLeftMargin_ + 1 + s_experimentPilePadding_,
            s_upperMargin_ + 1 + screenPileIndex * (s_experimentPileContentHeight_ + 1),
            vector<string>(s_experimentPileContentHeight_, string(s_experimentPileContentWidth_, ' ')),
            Color::NORMAL
        );
    }
    else if (table == Table::GAME) {
        draw::TableData(
            s_gameLeftMargin_ + 1 + s_gamePilePadding_,
            s_upperMargin_ + 1 + screenPileIndex * (s_gamePileContentHeight_ + 1),
            vector<string>(s_gamePileContentHeight_, string(s_gamePileContentWidth_, ' ')),
            Color::NORMAL
        );
    }
}

void Interface::__ClearLowerPileBoxLine(int screenPileIndex)
{
    _setmode(_fileno(stdout), _O_U16TEXT);
    setPosition(s_experimentLeftMargin_, s_upperMargin_ + (screenPileIndex + 1) * (s_experimentPileContentHeight_ + 1));
    wcout << BoxStyle::VERTICAL_LINE << wstring(s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_, ' ') << BoxStyle::VERTICAL_LINE;
    _setmode(_fileno(stdout), _O_TEXT);
}

void Interface::__Execute(Table table, const vector<Action>& actions)
{
    Operation _op;
    for (const auto& action : actions) {
        _op = action.operation;
        if (_op == Operation::TO_NORMAL) __DrawWholeTablePile(table, Color::NORMAL, action.value1, action.value2);
        else if (_op == Operation::TO_FOCUSED) __DrawWholeTablePile(table, s_colorFocused_, action.value1, action.value2);
        else if (_op == Operation::SHIFT_PILES) {
            int _upperPileIndex = action.value1,
                _cursorPileIndex = action.value2,
                _tablePileLimit,
                _pilesAmount;
            Color _color;
            if (table == Table::DAT) {
                _tablePileLimit = s_dateTablePileLimit_;
                _pilesAmount = FileHandler::s_experimentAmountsByDates.size();
            }
            else if (table == Table::EXPERIMENT) {
                _tablePileLimit = s_experimentTablePileLimit_;
                _pilesAmount = FileHandler::s_currExpAmountsByDatesIter->second;
            }
            else if (table == Table::GAME) {
                _tablePileLimit = s_gameTablePileLimit_;
                _pilesAmount = s_selectedGames_.size();
            }
            for (int i = 0; i < min(_tablePileLimit, _pilesAmount); ++i) {
               _color = (_upperPileIndex + i == _cursorPileIndex) ? s_colorFocused_ : Color::NORMAL;
                __DrawWholeTablePile(table, _color, i, _upperPileIndex + i, (_upperPileIndex + i - 1 == _cursorPileIndex));
            }
        }
        else if (_op == Operation::ADD_SLIDER_PART || _op == Operation::REMOVE_SLIDER_PART) {
            int _x, _y = s_upperMargin_ + 1 + action.value1;
            if (table == Table::DAT) _x = s_dateLeftMargin_ + s_datePileContentWidth_ + 2 * s_datePilePadding_ + 2;
            else if (table == Table::EXPERIMENT) _x = s_experimentLeftMargin_ + s_experimentPileContentWidth_ + 2 * s_experimentPilePadding_ + 2;
            else if (table == Table::GAME) _x = s_gameLeftMargin_ + s_gamePileContentWidth_ + 2 * s_gamePilePadding_ + 2;
            switch (_op) {
                case Operation::ADD_SLIDER_PART: __DrawVerticalLine(_x, _y, 1, s_colorSlider_); break;
                case Operation::REMOVE_SLIDER_PART: __DrawVerticalLine(_x, _y, 1, s_colorScrollbar_); break;
            }
        }
        else if (_op == Operation::RESET_SCROLLBAR) __DrawScrollbar(table);
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
        __DrawWholeTablePile(Table::EXPERIMENT, Color::NORMAL, pile, pile);
    }
    for (int pile = min(s_experimentTablePileLimit_, (int) s_selectedDateExperiments_.size()); pile < s_experimentTablePileLimit_; ++pile) {
        if (pile != s_experimentTablePileLimit_ - 1) __ClearLowerPileBoxLine(pile);
        __ClearPile(Table::EXPERIMENT, pile);
    }
    if (s_listExperiments_.value().scrollbar) __DrawScrollbar(Table::EXPERIMENT);
    else if (prevTableScrollbar) __RemoveExperimentTableScrollbar();
}

void Interface::__GetLastClickInfo(Window window)
{
    clickInfo.reset();
    int x = MouseInput::X,
        y = MouseInput::Y;

    if (window == Window::EXPERIMENT_SELECTION_MENU) {
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

    else if (window == Window::GAME_SELECTION_MENU) {
        if (s_gameLeftMargin_ <= x && x <= s_gameLeftMargin_ + 1 + s_gamePileContentWidth_ + 2 * s_gamePilePadding_ &&
            s_upperMargin_ <= y && y <= s_upperMargin_ + s_gameTableHeight_ - 1)
        {
            clickInfo.bGamesList = true;
            if (x == s_gameLeftMargin_ || x == s_gameLeftMargin_ + 1 + s_gamePileContentWidth_ + 2 * s_gamePilePadding_ ||
                y == s_upperMargin_ || y == s_upperMargin_ + s_gameTableHeight_ - 1)
            {
                clickInfo.isBorder = true;
            }
            else if ((y - s_upperMargin_) % (s_gamePileContentHeight_ + 1))
                clickInfo.screenPileIndex = (y - s_upperMargin_) / (s_gamePileContentHeight_ + 1);
            else clickInfo.isInnerBorder = true;
        }
    }
}

void Interface::__SetNextSortingOrder()
{
    if (s_gameListSorting_ == Sorting::ASC) s_gameListSorting_ = Sorting::CHRONOLOGICAL;
    else s_gameListSorting_ = static_cast<Sorting>(static_cast<int>(s_gameListSorting_) + 1);

    switch (s_gameListSorting_)
    {
        case Sorting::ASC:
            sort(s_selectedGames_.begin(), s_selectedGames_.end(), [](const Game& g1, const Game& g2) {
                 if (g1.score == g2.score) return g1.no < g2.no;
                 return g1.score < g2.score;
            });
            break;

        case Sorting::DESC:
            sort(s_selectedGames_.begin(), s_selectedGames_.end(), [](const Game& g1, const Game& g2) {
                 if (g1.score == g2.score) return g1.no < g2.no;
                 return g1.score > g2.score;
            });
            break;

        case Sorting::CHRONOLOGICAL:
        default:
            sort(s_selectedGames_.begin(), s_selectedGames_.end(), [](const Game& g1, const Game& g2) { return g1.no < g2.no; });
            break;
    }

    setColor(s_colorSortingOrder_);
    setPosition(s_coordsSortingOrder_.first, s_coordsSortingOrder_.second);
    cout << s_sortingName_[s_gameListSorting_];
    setColor(Color::NORMAL);
}

void Interface::__SetDetailedGameInfo(int gameNo, int movesAmount, double avgMovesToFood)
{
    setPosition(s_coordsGameNo_.first, s_coordsGameNo_.second);
    cout << gameNo << "   ";
    setPosition(s_coordsMovesAmount_.first, s_coordsMovesAmount_.second);
    cout << movesAmount << "      ";
    setPosition(s_coordsAvgMovesToFood_.first, s_coordsAvgMovesToFood_.second);
    if (avgMovesToFood == -1) cout << "...      ";
    else cout << doubleToStr(avgMovesToFood) << "      ";
}
