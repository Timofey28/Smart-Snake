#pragma once

#include <iostream>
#include <filesystem>
#include <vector>

#include "file_handler.h"

namespace fs = std::filesystem;


class Experiment
{
public:
    time_t creationTime;
    int gamesAmount;
    int fieldWidth, fieldHeight;
    int initialSnakeLength, maxPossibleSnakeLength;
    std::vector<int> gameScores, avgPathsToFood;
    int bestScore;
    double avgScore;

    Experiment(fs::path experimentFolderPath);

private:
    fs::path path_;
};
