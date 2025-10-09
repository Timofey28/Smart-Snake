#include "game.h"
using namespace std;

Game::Game(fs::path gameFilePath, int score, bool loadRightAway)
{
    assert(fs::exists(gameFilePath));
    this->path_ = gameFilePath;
    this->no = stoi(gameFilePath.stem().string());
    this->score = score;
    if (loadRightAway) {
        FileHandler::ReadGame(
            gameFilePath,
            width_, height_,
            indentX_, indentY_,
            field_,
            snakeTurns_,
            startingDirection_, crashDirection_,
            startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_,
            foodIndex_, lastFoodIndex_,
            gameIndexes
        );
        __CalculateMovesInfo();
    }
    else width_ = -1;
}

void Game::Load()
{
    if (IsLoaded()) return;
    FileHandler::ReadGame(
        path_,
        width_, height_,
        indentX_, indentY_,
        field_,
        snakeTurns_,
        startingDirection_, crashDirection_,
        startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_,
        foodIndex_, lastFoodIndex_,
        gameIndexes
    );
    __CalculateMovesInfo();
}

void Game::PrintFirstFrame()
{

}

void Game::PrintNextFrame()
{

}

void Game::PrintPreviousFrame()
{

}

void Game::__CalculateMovesInfo()
{
    movesAmount_ = gameIndexes.size() - (finalSnakeLength_ - startingSnakeLength_);
    auto it = find(gameIndexes.rbegin(), gameIndexes.rend(), lastFoodIndex_);
    assert(it != gameIndexes.rend());
    int _movesToLastFood = gameIndexes.size() - distance(gameIndexes.rbegin(), it);
    avgMovesToFood_ = (double) (_movesToLastFood - (finalSnakeLength_ - startingSnakeLength_ - 1)) / (finalSnakeLength_ - startingSnakeLength_);
}
