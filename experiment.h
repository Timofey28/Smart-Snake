#pragma once

#include <iostream>
#include <filesystem>
#include <vector>
#include <cmath>

#include "utils.h"
#include "file_handler.h"

namespace fs = std::filesystem;


class Experiment
{
public:
    time_t creationTime;
    int gamesAmount;
    int fieldWidth, fieldHeight;
    int initialSnakeLength, maxPossibleSnakeLength;
    std::vector<int> gameScores;  // gained length only
    int bestScore, worstScore;
    int bestScoreNo, worstScoreNo;
    double avgScore, stdScore;

    Experiment(fs::path experimentFolderPath);
    int CalculateGamePileContentWidth() { return maxGameNoLength_ + maxScoreLength_ + maxPossibleScoreLength_ + maxGainPercentLength_ + 6; }
    fs::path Path() { return path_; }

private:
    fs::path path_;
    int maxGameNoLength_, maxScoreLength_, maxPossibleScoreLength_, maxGainPercentLength_;
};
