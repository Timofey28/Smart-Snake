#include "experiment.h"
using namespace std;

Experiment::Experiment(fs::path experimentFolderPath)
{
    path_ = experimentFolderPath;
    creationTime = FileHandler::GetLastWriteTime(experimentFolderPath);
    FileHandler::GetExperimentInitialData(path_, fieldWidth, fieldHeight, initialSnakeLength, maxPossibleSnakeLength, gameScores);
    gamesAmount = gameScores.size();
    bestScore = gameScores[0];
    avgScore = 0.0f;
    for (int game = 0; game < gamesAmount; ++game) {
        bestScore = max(bestScore, gameScores[game]);
        avgScore += gameScores[game];
    }
    avgScore /= gamesAmount;
}
