#include "experiment.h"
using namespace std;

Experiment::Experiment(fs::path experimentFolderPath)
{
    path_ = experimentFolderPath;
    creationTime = FileHandler::GetLastWriteTime(experimentFolderPath);
    FileHandler::GetExperimentInitialData(path_, fieldWidth, fieldHeight, initialSnakeLength, maxPossibleSnakeLength, gameScores);
    gamesAmount = gameScores.size();
    avgScore = 0.0f;
    bestScore = gameScores[0];
    worstScore = gameScores[0];
    bestScoreNo = 0;
    worstScoreNo = 0;
    maxScoreLength_ = 0;
    maxPossibleScoreLength_ = numberLength(maxPossibleSnakeLength - initialSnakeLength);
    maxGainPercentLength_ = 0;
    maxGameNoLength_ = numberLength(gamesAmount);
    for (int game = 0; game < gamesAmount; ++game) {
        avgScore += gameScores[game];
        if (gameScores[game] > bestScore) {
            bestScore = gameScores[game];
            bestScoreNo = game;
        }
        if (gameScores[game] < worstScore) {
            worstScore = gameScores[game];
            worstScoreNo = game;
        }

        maxScoreLength_ = max(maxScoreLength_, numberLength(gameScores[game]));
        maxGainPercentLength_ = max(maxGainPercentLength_, numberLength(gameScores[game] * 100 / (maxPossibleSnakeLength - initialSnakeLength)));
    }
    avgScore /= gamesAmount;

    stdScore = 0.0f;
    for (int game = 0; game < gamesAmount; ++game) stdScore += pow(gameScores[game] - avgScore, 2);
    stdScore = sqrt(stdScore / gamesAmount);
}
