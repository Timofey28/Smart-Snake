#pragma once

#include <vector>
#include <queue>
#include <unordered_set>
#include <stack>
#include <stdexcept>
#include <cassert>
#include <thread>
#include <algorithm>

#include "console.h"
#include "utils.h"
#include "draw.h"
#include "mouse_input.h"
#include "validation.h"
#include "file_handler.h"
#include "algorithm.h"
#include "caption_init.h"
#include "alert.h"


class Playground
{
public:
    int gamesAmount;

    Playground() :
        captionFieldDimensions(CAPTION_FIELD_DIMENSIONS_INPUT),
        captionFieldElements(CAPTION_FIELD_ELEMENTS_ARRANGEMENT) { __Init(); }
    bool FieldParametersInputForm();
    void EnterGamesAmount();
    void SaveInitialData();
    void SaveLastGame();
    void SaveGamesSummary();
    void ReinitializeStartingData();
    void CalculateNextIteration();
    bool GameOn() { return gameOn_; }
    bool Victory() { return victory_; }

private:
    int width_, height_;  // considering whole field with boundaries
    int indentX_, indentY_;
    std::vector<Cell> field_, initialField_;
    std::vector<std::vector<int>> nodes_, initialNodes_;
    std::vector<int> currentPassCells_, initialCurrentPassCells_;  // to choose a random cell for food
    Direction currentDirection_, initialCurrentDirection_;
    std::queue<Direction> snakeTurns_, initialSnakeTurns_;
    bool gameOn_, victory_;
    int foodIndex_, snakeHeadIndex_, snakeAssIndex_, initialSnakeHeadIndex_, initialSnakeAssIndex_;
    const Caption& captionFieldDimensions, captionFieldElements;
    std::unordered_set<int> borderPortalIndexes_, fieldPortalIndexes_;

    Validation validation;
    Alert alert;

    // variables to record the game
    int firstFoodIndex_, lastFoodIndex_;
    std::vector<int> headAndFoodIndexes_;
    Direction crashDirection_;
    float averageMovesToFood_;
    std::vector<int> gameScores_;

    void __Init();
    void __EnterFieldDimensions();
    void __InitializeFieldFromDimensions();
    void __ArrangeFieldElements();
    void __AdjustPortals();
    void __CalculatePortalEntries(Orientation orientation, int axisValue);
    bool __WholeAxisIsAWall(Orientation orientation, int axisValue);
    void __MovePortalsBackToBorder();
    void __RepaintSnakeCells();
    void __ShowAlert(AlertType alertType);
    void __GetBorderPortals();

    void __InitializePlayground();
    void __FillAdjacencyList();
    void __FillSnakeTurnsQueue();

    Direction __FindMovementDirection(int fromIndex, int toIndex) {
        return findMovementDirection(fromIndex, toIndex, width_);
    }
    int __FindCellFromMovementDirection(int fromIndex, Direction movementDirection) {
        return findCellFromMovementDirection(fromIndex, movementDirection, field_, width_, height_);
    }
    int __GetPortalExitIndex(int portalEnterIndex, Direction movementDirection) {
        return getPortalExitIndex(portalEnterIndex, movementDirection, field_, width_, height_);
    }
    std::vector<int> __GetCellVicinityByIndexes(int cellIndex);
};


inline void Playground::__ShowAlert(AlertType alertType)
{
    auto cMovePortalsBackToBorder = std::bind(&Playground::__MovePortalsBackToBorder, this);
//    auto cDrawCaption = std::bind(&Caption::Draw<std::recursive_mutex>, &this->captionFieldElements, std::ref(::rmtx));  // std::bind is outdated
    auto cDrawCaption = [this]() { this->captionFieldElements.Draw(::rmtx); };
    alert.Show(alertType, &field_, width_, cMovePortalsBackToBorder, cDrawCaption);
}
