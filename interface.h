#pragma once

#include <unordered_map>
#include <filesystem>
#include <string>
#include <iomanip>
#include <ctime>
#include <thread>
#include <cassert>
#include <optional>

#include "file_handler.h"
#include "utils.h"
#include "draw.h"
#include "experiment.h"
#include "scrollable_list.h"
#include "mouse_input.h"

#include <conio.h>

extern int nConsoleWidth, nConsoleHeight;
using BoxStyle = Symbols::BoxLight;


struct ClickInfo
{
    bool bDatesList, bExperimentsList;
    bool isBorder, isInnerBorder;
    int screenPileIndex;

    ClickInfo() {}
    inline void reset() {
        bDatesList = false;
        bExperimentsList = false;
        isBorder = false;
        isInnerBorder = false;
        screenPileIndex = -1;
    }
};


class Interface
{
public:
    static Experiment* DatesAndExperimentsList();

private:
    static constexpr Color s_colorScrollbar_ = Color::ALMOST_WHITE, s_colorSlider_ = Color::GRAY;
    static constexpr Color s_colorFocused_ = Color::BRIGHT_GREEN_ON_BLACK, s_colorSelected_ = Color::GREEN_ON_BLACK;
    static constexpr int s_datePileContentWidth_ = 22, s_experimentPileContentWidth_ = 24;
    static constexpr int s_datePileContentHeight_ = 2, s_experimentPileContentHeight_ = 6;
    static constexpr int s_datePilePadding_ = 2, s_experimentPilePadding_ = 2;  // внутренний отступ по горизонтали
    static constexpr int s_upperMargin_ = 1; // внешний отступ сверху
    static constexpr int s_dateLeftMargin_ = 4;  // внешний отступ слева списка дат
    static int s_experimentLeftMargin_;  // внешний отступ слева списка экспериментов
    static int s_dateTableHeight_, s_experimentTableHeight_;
    static int s_dateTablePileLimit_, s_experimentTablePileLimit_;
    static std::unordered_map<time_t, Experiment> s_experiments_;
    static std::vector<Experiment> s_selectedDateExperiments_;
    static bool s_focusOnDatesList_;
    static std::optional<ScrollableList> s_listDates_, s_listExperiments_;
    static ClickInfo clickInfo;

    static void __InitTableDimensions();
    static Experiment __GetExperimentInfo(fs::path experimentFolderPath);
    static Experiment __GetExperimentInfo(time_t experimentDateNo);
    static std::vector<std::string> __MakePileData(
        std::map<time_t, int, std::greater<time_t>>::iterator experimentAmountByDateIterator,
        int cursorPileIndex
    );
    static std::vector<std::string> __MakePileData(Experiment e, int cursorPileIndex);
    static void __DrawTableData(
        bool isDateTable,
        Color color,
        std::optional<int> screenPileIndex = std::nullopt,
        std::optional<int> cursorPileIndex = std::nullopt
    );
    static void __DrawBoxPile(bool isDateTable, Color color, std::optional<int> screenPileIndex = std::nullopt);
    static void __DrawWholeTablePile(
        bool isDateTable,
        Color color,
        std::optional<int> screenPileIndex = std::nullopt,
        std::optional<int> cursorPileIndex = std::nullopt
    );
    static void __DrawVerticalLine(int x, int y, int length, Color color);
    static void __DrawScrollbar(bool dateTable);
    static void __RemoveExperimentTableScrollbar();
    static void __ClearPile(bool dateTable, int screenPileIndex);
    static void __Execute(bool onDateTable, const std::vector<Action>& actions);
    static void __LoadNewExperiments();
    static void __GetLastClickInfo();
};
