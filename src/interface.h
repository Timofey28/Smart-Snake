#pragma once

#include <unordered_map>
#include <filesystem>
#include <string>
#include <iomanip>
#include <ctime>
#include <thread>
#include <cassert>
#include <optional>
#include <memory>
#include <chrono>

#include "console.h"
#include "file_handler.h"
#include "utils.h"
#include "draw.h"
#include "experiment.h"
#include "scrollable_list.h"
#include "mouse_input.h"
#include "game.h"

//#include <conio.h>


enum Result
{
    EXIT,
    BACK,
    GAME_CHOSEN,
};

enum TableType
{
    DAT,
    EXPERIMENT,
    GAME,
};

enum Window
{
    EXPERIMENT_SELECTION_MENU,
    GAME_SELECTION_MENU,
};

enum Sorting
{
    CHRONOLOGICAL,
    DESC,
    // Last one must be ASC
    ASC,
};


struct ClickInfo
{
    bool bDatesList, bExperimentsList, bGamesList;
    bool isBorder, isInnerBorder;
    int screenPileIndex;

    ClickInfo() {}
    inline void reset() {
        bDatesList = false;
        bExperimentsList = false;
        bGamesList = false;
        isBorder = false;
        isInnerBorder = false;
        screenPileIndex = -1;
    }
};


class Interface
{
public:
    static bool DatesAndExperimentsList();
    static Result GamesList();
    static void GamePlayback();

private:
    static constexpr Color s_colorScrollbar_ = Color::ALMOST_WHITE, s_colorSlider_ = Color::GRAY, s_colorSortingOrder_ = Color::GOLD_ON_BLACK;
    static constexpr Color s_colorFocused_ = Color::BRIGHT_GREEN_ON_BLACK, s_colorSelected_ = Color::GREEN_ON_BLACK;
    static constexpr int s_datePileContentWidth_ = 22, s_experimentPileContentWidth_ = 24;
    static constexpr int s_datePileContentHeight_ = 2, s_experimentPileContentHeight_ = 6, s_gamePileContentHeight_ = 1;
    static constexpr int s_datePilePadding_ = 2, s_experimentPilePadding_ = 2, s_gamePilePadding_ = 1;  // внутренний отступ по горизонтали
    static constexpr int s_upperMargin_ = 1; // внешний отступ сверху
    static constexpr int s_dateLeftMargin_ = 4;  // внешний отступ слева списка дат
    static constexpr int s_gameMenuLeftMargin_ = 4;
    static int s_experimentLeftMargin_;  // внешний отступ слева списка экспериментов
    static int s_gameLeftMargin_;
    static int s_gamePileContentWidth_;
    static int s_dateTableHeight_, s_experimentTableHeight_, s_gameTableHeight_;
    static int s_dateTablePileLimit_, s_experimentTablePileLimit_, s_gameTablePileLimit_;
    static std::unordered_map<time_t, Experiment> s_experiments_;
    static std::vector<Experiment> s_selectedDateExperiments_;
    static std::vector<std::unique_ptr<Game>> s_selectedGames_;
    static bool s_focusOnDatesList_;
    static std::optional<ScrollableList> s_listDates_, s_listExperiments_, s_listGames_;
    static ClickInfo clickInfo;
    static int s_chosenExperimentIndex_, s_chosenGameIndex_;
    static std::pair<int, int> s_coordsGameNo_, s_coordsMovesAmount_, s_coordsAvgMovesToFood_, s_coordsSortingOrder_;
    static Sorting s_gameListSorting_;
    static std::map<Sorting, std::string> s_sortingName_;

    static void __InitTableDimensions();
    static Experiment __GetExperimentInfo(fs::path experimentFolderPath);
    static Experiment __GetExperimentInfo(time_t experimentDateNo);
    static std::vector<std::string> __MakePileData(
        std::map<time_t, int, std::greater<time_t>>::iterator experimentAmountByDateIterator,
        int cursorPileIndex
    );
    static std::vector<std::string> __MakePileData(Experiment e, int cursorPileIndex);
    static std::vector<std::string> __MakePileData(int gameCursorIndex, int score, int maxPossibleScore);
    static void __DrawTableData(
        TableType tableType,
        Color color,
        std::optional<int> screenPileIndex = std::nullopt,
        std::optional<int> cursorPileIndex = std::nullopt
    );
    static void __DrawBoxPile(
        TableType tableType,
        Color color,
        std::optional<int> screenPileIndex = std::nullopt,
        bool careful = false
    );
    static void __DrawWholeTablePile(
        TableType tableType,
        Color color,
        std::optional<int> screenPileIndex = std::nullopt,
        std::optional<int> cursorPileIndex = std::nullopt,
        bool careful = false
    );
    static void __DrawVerticalLine(int x, int y, int length, Color color);
    static void __DrawScrollbar(TableType tableType);
    static void __RemoveExperimentTableScrollbar();
    static void __ClearPile(TableType tableType, int screenPileIndex);
    static void __ClearLowerPileBoxLine(int screenPileIndex);  // always experiments' table
    static void __Execute(TableType tableType, const std::vector<Action>& actions);
    static void __LoadNewExperiments();
    static void __GetLastClickInfo(Window window);
    static void __SetNextSortingOrder();
    static void __SetDetailedGameInfo(int gameNo, int movesAmount, double avgMovesToFood);
    static void __ConfigureFontSize(Game& game);
    static void __RunGame(Game& game);
};
