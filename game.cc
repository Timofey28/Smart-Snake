#include "game.h"
using namespace std;

Game::Game()
{

}

void Game::Initialize(fs::path gameFilePath)
{
    fileHandler.ReadGame(
        gameFilePath,
        width_, height_,
        indentX_, indentY_,
        field_,
        snakeTurns_,
        startingDirection_, crashDirection_,
        startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_,
        foodIndex_,
        gameIndexes
    );
}
