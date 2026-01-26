#pragma once

#include <filesystem>
#include <vector>
#include <deque>
#include <cassert>
#include <algorithm>
#include <map>
#include <iterator>
#include <atomic>
#include <mutex>

#include "utils.h"
#include "draw.h"
#include "table.h"
#include "file_handler.h"
#include "mouse_input.h"

//#include <conio.h>

namespace fs = std::filesystem;


enum GameAction
{
    NONE_GA,
    FRAME_FORWARD,
    FRAME_BACKWARD,
    START_PAUSE,
    SET_SPEED,
    SET_FRAME,
    CHANGE_PLAYBACK_MODE,
    EXIT_GA,
};


class GameScrollbar
{
public:
    int sliderPos;

    GameScrollbar(int _frames, int _width);
    int Frames() { return frames_; }
    int SliderPosToFrame(int _sliderPos) { return sliderPosToFrame_[_sliderPos]; }
    int FrameToSliderPos(int _frame) { return frameToSliderPos_[_frame]; }

private:
    int frames_, length_;
    std::map<int, int> frameToSliderPos_, sliderPosToFrame_;
    // frame with key -1 -- initial frame
    // frame with key (frames_ - 2) -- last frame with outlined head
};


class Game
{
public:
    int no, score;
    std::atomic<int> currentSpeedNo;
    std::atomic<bool> paused, forwardPlayback, finished;
    int newSpeedNo, newFrame;

    Game(fs::path gameFilePath, int score, bool loadRightAway = true);
    bool IsLoaded() { return width_ != -1; }
    int MovesAmount() { return IsLoaded() ? movesAmount_ : -1; }
    double AvgMovesToFood() { return IsLoaded() ? avgMovesToFood_ : -1; }
    int MaxPossibleScore() { return IsLoaded() ? maxPossibleSnakeLength_ - startingSnakeLength_ : -1; }
    int ContentWidth() { return contentWidth_; }
    int ContentHeight() { return contentHeight_; }

    template <class Mtx = NullMutex>
    bool PrintNextFrame(Mtx& m = noLock);

    template <class Mtx = NullMutex>
    bool PrintPreviousFrame(Mtx& m = noLock);

    template <class Mtx = NullMutex>
    void PrintFrameByIndex(Mtx& m = noLock);

    GameAction ProcessUserInput();
    void WriteReproductionMode(bool fwd);
    void SetAndPaintNewSpeedNo();
    void CorrectScrollbarSlider();
    void CorrectStateInfo();
    void SetAndPaintPause();

    void Load();
    void InitAndPrintInterface();
    void SetFieldIndents(int indentX, int indentY);
    void DrawScrollbar();
    int SpeedNo2FrameTime(int speedNo) { return s_speedNoToFrameTime_[speedNo]; }

private:
    fs::path path_;
    int width_, height_;
    int fieldIndentX_, fieldIndentY_;
    std::vector<Cell> field_, initialField_;
    std::deque<Direction> snakeTurns_, initialSnakeTurns_;
    Direction startingDirection_, crashDirection_;
    int startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_;
    int foodIndex_, lastFoodIndex_;
    std::vector<int> gameIndexes_, initialGameIndexes_;
    int movesAmount_;
    double avgMovesToFood_;

    std::vector<int> foodIndexes_;
    int currentFrame_, currentFoodIndex_, snakeLength_, snakeAssIndex_, startingHeadIndex_;
    int upperMargin_, leftPartWidth_, contentWidth_, contentHeight_;
    int maxFrameNoLength_, maxSnakeNoLength_, maxAppleNoLength_;
    std::unique_ptr<GameScrollbar> scrollbar_;
    std::unique_ptr<Table<BoxStyle>> speedsTable_;
    bool frameChanged_, snakeLengthChanged_;

    static constexpr Color s_colorScrollbar_ = Color::BLACK_ON_ALMOST_WHITE, s_colorSlider_ = Color::BLACK_ON_GRAY;
    static constexpr Color s_colorChosenSpeed_ = Color::BRIGHT_MAGENTA_ON_BLACK, s_colorArrows_ = Color::GRAY_ON_BLACK;
    static constexpr std::string_view s_fwdModeCaption_ = "Прямое воспроизведение", s_revModeCaption_ = "Обратное воспроизведение";
    static constexpr std::string_view s_fwdModeCaptionShort_ = "Пр. воспр.", s_revModeCaptionShort_ = "Обр.воспр.";
    static constexpr int s_spaceBetween_ = 2;
    // upper indents
    static constexpr int s_indentFrameNo = 1, s_indentSnakeLength_ = 2, s_indentApplesEaten_ = 3;
    static constexpr int s_indentSpeed_ = 5, s_indentSpeedsTable_ = 6;

    inline static std::map<int, int> s_speedNoToFrameTime_ = {
        {1, 200},
        {2, 100},
        {3, 50},
        {4, 30},
        {5, 20},
    };

    void __InitDimensions();
    void __CalculateMovesInfo();
    void __FindSnakeAssIndex();
    Direction __FindMovementDirection(int fromIndex, int toIndex) {
        return findMovementDirection(fromIndex, toIndex, width_);
    }
    int __FindCellFromMovementDirection(int fromIndex, Direction movementDirection) {
        return findCellFromMovementDirection(fromIndex, movementDirection, field_, width_, height_);
    }
    int __GetPortalExitIndex(int portalEnterIndex, Direction movementDirection) {
        return getPortalExitIndex(portalEnterIndex, movementDirection, field_, width_, height_);
    }
    void __DrawCrash(int snakeHeadIndex) { draw::Crash(true, field_, width_, snakeHeadIndex); }
    void __RemoveCrash(int snakeHeadIndex) { draw::Crash(false, field_, width_, snakeHeadIndex); }
};
