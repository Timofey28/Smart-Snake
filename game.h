#pragma once

#include <filesystem>
#include <vector>
#include <queue>
#include <cassert>
#include <algorithm>

#include "utils.h"
#include "file_handler.h"

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
    void PrintNextFrame();
    void PrintPreviousFrame();

private:
    fs::path path_;
    int width_, height_;
    int indentX_, indentY_;
    std::vector<Cell> field_;
    std::queue<Direction> snakeTurns_;
    Direction startingDirection_, crashDirection_;
    int startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_;
    int foodIndex_, lastFoodIndex_;
    std::vector<int> gameIndexes;

    int movesAmount_;
    double avgMovesToFood_;
    std::stack<Direction> previousSnakeTurns_;
    int currentIndex_;

    void __CalculateMovesInfo();
};
