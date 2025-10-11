#pragma once

#include <filesystem>
#include <vector>
#include <deque>
#include <cassert>
#include <algorithm>

#include "utils.h"
#include "file_handler.h"
#include "draw.h"

namespace fs = std::filesystem;


class Game
{
public:
    int no, score;

    Game(fs::path gameFilePath, int score, bool loadRightAway = true);
    bool IsLoaded() { return width_ != -1; }
    int MovesAmount() { return IsLoaded() ? movesAmount_ : -1; }
    double AvgMovesToFood() { return IsLoaded() ? avgMovesToFood_ : -1; }
    int MaxPossibleScore() { return IsLoaded() ? maxPossibleSnakeLength_ - startingSnakeLength_ : -1; }

    void Load();
    void PrintFirstFrame();
    bool PrintNextFrame();
    bool PrintPreviousFrame();

private:
    fs::path path_;
    int width_, height_;
    int indentX_, indentY_;
    std::vector<Cell> field_, initialField_;
    std::deque<Direction> snakeTurns_, initialSnakeTurns_;
    Direction startingDirection_, crashDirection_;
    int startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_;
    int foodIndex_, lastFoodIndex_;
    std::vector<int> gameIndexes_;
    int movesAmount_;
    double avgMovesToFood_;

    std::vector<int> foodIndexes_;
    int currentGameIndex_, currentFoodIndex_, snakeLength_, snakeAssIndex_, startingHeadIndex_;

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
