#pragma once

#include <filesystem>
#include <vector>
#include <queue>
#include "utils.h"
#include "file_handler.h"

namespace fs = std::filesystem;


class Game
{
public:
    Game();
    void Initialize(fs::path gameFilePath);
    void PrintFirstFrame();
    void PrintNextIteration();
    void PrintPreviousIteration();

private:
    int width_, height_;
    int indentX_, indentY_;
    std::vector<Cell> field_;
    std::queue<Direction> snakeTurns_;
    Direction startingDirection_, crashDirection_;
    int startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_;
    int foodIndex_;
    std::vector<int> gameIndexes;

    std::stack<Direction> previousSnakeTurns_;
    int currentIndex_;

    FileHandler fileHandler;
};
