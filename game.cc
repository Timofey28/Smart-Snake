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
            initialField_,
            initialSnakeTurns_,
            startingDirection_, crashDirection_,
            startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_,
            foodIndex_, lastFoodIndex_,
            gameIndexes_
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
        initialField_,
        initialSnakeTurns_,
        startingDirection_, crashDirection_,
        startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_,
        foodIndex_, lastFoodIndex_,
        gameIndexes_
    );
    __CalculateMovesInfo();
}

void Game::PrintFirstFrame()
{
    assert(IsLoaded());
    field_ = initialField_;
    snakeTurns_ = initialSnakeTurns_;
    currentGameIndex_ = -1;
    currentFoodIndex_ = 0;
    snakeLength_ = startingSnakeLength_;
    field_[foodIndex_].type = CellType::FOOD;
    foodIndexes_ = {foodIndex_};
    vector<int> _indicesToRemove;
    _indicesToRemove.reserve(score);
    for (int i = 0; i < gameIndexes_.size(); ++i) {
        if (gameIndexes_[i] == foodIndexes_.back() && foodIndexes_.size() < MaxPossibleScore()) {
            assert(i < gameIndexes_.size() - 1);
            foodIndexes_.push_back(gameIndexes_[++i]);
            _indicesToRemove.push_back(i);
        }
    }
    for (int i = _indicesToRemove.size() - 1; i >= 0; --i) {
        gameIndexes_.erase(gameIndexes_.begin() + _indicesToRemove[i]);
    }

    draw::Field(field_, width_);
    for (int i = 0; i < field_.size(); ++i) {
        if (field_[i].type == CellType::SNAKE_HEAD) {
            if (snakeLength_ == 1) startingDirection_ = __FindMovementDirection(i, gameIndexes_[0]);
            draw::SnakeHead(field_[i], startingDirection_);
            startingHeadIndex_ = i;
            break;
        }
    }
    __FindSnakeAssIndex();
    setColor(Color::NORMAL);
}

bool Game::PrintNextFrame()
{
    if (currentGameIndex_ == gameIndexes_.size()) return false;
    if (currentGameIndex_ == gameIndexes_.size() - 1) __DrawCrash(gameIndexes_[currentGameIndex_++]);
    else {
        int _prevCellIndex = (currentGameIndex_ == -1) ? startingHeadIndex_ : gameIndexes_[currentGameIndex_];
        snakeTurns_.push_front(__FindMovementDirection(_prevCellIndex, gameIndexes_[++currentGameIndex_]));
        field_[gameIndexes_[currentGameIndex_]].type = CellType::SNAKE_HEAD;
        draw::SnakeHead(field_[gameIndexes_[currentGameIndex_]], snakeTurns_[0]);
        if (snakeLength_ >= 2 || gameIndexes_[currentGameIndex_] == foodIndexes_[currentFoodIndex_]) {
            field_[_prevCellIndex].type = CellType::SNAKE_BODY;
            draw::GameCell(field_[_prevCellIndex]);
        }
        if (gameIndexes_[currentGameIndex_] == foodIndexes_[currentFoodIndex_]) {
            if (++snakeLength_ < maxPossibleSnakeLength_) {
                assert(currentFoodIndex_ < foodIndexes_.size() - 1);
                field_[foodIndexes_[++currentFoodIndex_]].type = CellType::FOOD;
                draw::GameCell(field_[foodIndexes_[currentFoodIndex_]]);
            }
        }
        else {
            if (gameIndexes_[currentGameIndex_] != snakeAssIndex_) {
                field_[snakeAssIndex_].type = CellType::PASS;
                draw::GameCell(field_[snakeAssIndex_]);
            }
            snakeAssIndex_ = __FindCellFromMovementDirection(snakeAssIndex_, snakeTurns_[snakeLength_ - 1]);
        }
    }

    return true;
}

bool Game::PrintPreviousFrame()
{
    if (currentGameIndex_ == -1) return false;
    if (currentGameIndex_ == gameIndexes_.size()) __RemoveCrash(gameIndexes_[--currentGameIndex_]);
    else {
        int _prevCellIndex = gameIndexes_[currentGameIndex_];
        if (--currentGameIndex_ == -1) {
            field_[startingHeadIndex_].type = CellType::SNAKE_HEAD;
            draw::SnakeHead(field_[startingHeadIndex_], startingDirection_);
        }
        else {
            field_[gameIndexes_[currentGameIndex_]].type = CellType::SNAKE_HEAD;
            if (snakeTurns_.size() >= 2) draw::SnakeHead(field_[gameIndexes_[currentGameIndex_]], snakeTurns_[1]);
            else draw::SnakeHead(field_[gameIndexes_[currentGameIndex_]], snakeTurns_[0]);
        }
        if (currentFoodIndex_ > 0 && _prevCellIndex == foodIndexes_[currentFoodIndex_ - 1]) {
            snakeLength_--;
            field_[foodIndexes_[currentFoodIndex_]].type = CellType::PASS;
            draw::GameCell(field_[foodIndexes_[currentFoodIndex_--]]);
            field_[_prevCellIndex].type = CellType::FOOD;
            draw::GameCell(field_[_prevCellIndex]);
        }
        else if (snakeLength_ == maxPossibleSnakeLength_ && _prevCellIndex == foodIndexes_.back()) {  // from last victory frame
            snakeLength_--;
            field_[_prevCellIndex].type = CellType::FOOD;
            draw::GameCell(field_[_prevCellIndex]);
        }
        else {
            snakeAssIndex_ = __FindCellFromMovementDirection(snakeAssIndex_, opposite(snakeTurns_[snakeLength_ - 1]));
            if (snakeLength_ >= 2) {
                field_[snakeAssIndex_].type = CellType::SNAKE_BODY;
                draw::GameCell(field_[snakeAssIndex_]);
            }
            if (_prevCellIndex != snakeAssIndex_) {
                field_[_prevCellIndex].type = CellType::PASS;
                draw::GameCell(field_[_prevCellIndex]);
            }
        }
        snakeTurns_.pop_front();
    }

    return true;
}

void Game::__CalculateMovesInfo()
{
    movesAmount_ = gameIndexes_.size() - (finalSnakeLength_ - startingSnakeLength_);
    auto it = find(gameIndexes_.rbegin(), gameIndexes_.rend(), lastFoodIndex_);
    assert(it != gameIndexes_.rend());
    int _movesToLastFood = gameIndexes_.size() - distance(gameIndexes_.rbegin(), it);
    avgMovesToFood_ = (double) (_movesToLastFood - (finalSnakeLength_ - startingSnakeLength_ - 1)) / (finalSnakeLength_ - startingSnakeLength_);
}

void Game::__FindSnakeAssIndex()
{
    snakeAssIndex_ = startingHeadIndex_;
    for (int i = 0; i < snakeTurns_.size(); ++i) snakeAssIndex_ = __FindCellFromMovementDirection(snakeAssIndex_, opposite(snakeTurns_[i]));
}
