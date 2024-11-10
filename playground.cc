#include "playground.h"

#include <stdexcept>
#include <cassert>
#include <thread>
#include <algorithm>

using namespace std;

#include <conio.h>


Playground::Playground()
{
    width_ = 0;
    height_ = 0;
    gameOn_ = true;
    foodIndex_ = -1;
}

void Playground::__InitializeFieldFromDimensions()
{
    indentX_ = (nConsoleWidth / 2 - width_) / 2;
    indentY_ = (nConsoleHeight - height_) / 2;

    // Putting walls around perimeter
    field_.resize(width_ * height_);
    CellType cellType;
    for (int i = 0; i < field_.size(); ++i) {
        cellType = CellType::PASS;
        if (i % width_ == 0 || i % width_ == width_ - 1 || i / width_ == 0 || i / width_ == height_ - 1) cellType = CellType::WALL;
        field_[i] = Cell(i, width_, indentX_, indentY_, cellType);
    }
}

void Playground::FieldParametersInputForm()
{
    // Enter size of playing field
    draw::EnterFieldDimensions(width_, height_);
    __InitializeFieldFromDimensions();

    // Arrangement of walls, portals and snake itself
    __ArrangeFieldElements();
}

void Playground::SaveInitialData()
{
    stack<Direction> snakeTurnsStacked;
    queue<int> traversalOrder;
    traversalOrder.push(validation.startingCellIndex);
    unordered_set<int> passed = {validation.startingCellIndex};
    int cellIndex, x, y;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;
    int snakeLength = 0;

    initialSnakeHeadIndex_ = validation.startingCellIndex;
    while (!traversalOrder.empty()) {
        snakeLength++;
        cellIndex = traversalOrder.front();
        traversalOrder.pop();
        x = cellIndex % width_;
        y = cellIndex / width_;

        leftCellIndex = y * width_ + (x - 1);
        rightCellIndex = y * width_ + (x + 1);
        topCellIndex = (y - 1) * width_ + x;
        bottomCellIndex = (y + 1) * width_ + x;

        if (field_[leftCellIndex].type == CellType::SNAKE_BODY && !passed.count(leftCellIndex)) {
            passed.insert(leftCellIndex);
            snakeTurnsStacked.push(Direction::RIGHT);
            traversalOrder.push(leftCellIndex);
        }
        else if (field_[rightCellIndex].type == CellType::SNAKE_BODY && !passed.count(rightCellIndex)) {
            passed.insert(rightCellIndex);
            snakeTurnsStacked.push(Direction::LEFT);
            traversalOrder.push(rightCellIndex);
        }
        else if (field_[topCellIndex].type == CellType::SNAKE_BODY && !passed.count(topCellIndex)) {
            passed.insert(topCellIndex);
            snakeTurnsStacked.push(Direction::DOWN);
            traversalOrder.push(topCellIndex);
        }
        else if (field_[bottomCellIndex].type == CellType::SNAKE_BODY && !passed.count(bottomCellIndex)) {
            passed.insert(bottomCellIndex);
            snakeTurnsStacked.push(Direction::UP);
            traversalOrder.push(bottomCellIndex);
        }
    }
    initialSnakeAssIndex_ = cellIndex;

    // stakeTurnsStacked - stack with the first element to take being the direction from snake last element to second last
    fileHandler.SaveInitialData(
        width_, height_,
        indentX_, indentY_,
        snakeLength,
        validation.startingDirection,
        currentPassCells_.size() + snakeLength,
        snakeTurnsStacked,
        field_
    );
}

void Playground::ReinitializeStartingData()
{
    // field parameters
    field_ = initialField_;
    nodes_ = initialNodes_;
    currentPassCells_ = initialCurrentPassCells_;
    currentDirection_ = initialCurrentDirection_;
    snakeTurns_ = initialSnakeTurns_;

    // game calculation
    gameOn_ = true;
    foodIndex_ = -1;
    snakeHeadIndex_ = initialSnakeHeadIndex_;
    snakeAssIndex_ = initialSnakeAssIndex_;

    // recording the game
    firstFoodIndex_ = -1;
    headAndFoodIndexes_.clear();
}

void Playground::CalculateNextIteration()
{
    if (foodIndex_ == -1) {
        foodIndex_ = currentPassCells_[randomUnder(currentPassCells_.size())];
        firstFoodIndex_ = foodIndex_;
    }
    field_[foodIndex_].type = CellType::FOOD;

//    // Общая инфа слева сверху
//    ostringstream oss;
//    oss << "snakeHeadIndex_: (" << snakeHeadIndex_ % width_ << ", " << snakeHeadIndex_ / width_ << ")\nfoodIndex_: (";
//    oss << foodIndex_ % width_ << ", " << foodIndex_ / width_ << ")\nnodes_[snakeHeadIndex_]: ";
//    for (auto x : nodes_[snakeHeadIndex_]) oss << "(" << x % width_ << ", " << x / width_ << ") ";
//    oss << "| [" << nodes_[snakeHeadIndex_].size() << "]";
//    oss << string(30, ' ');
//    draw::smth(oss.str());

//    // Запись значений nodes_ в файл
//    ofstream file("log.txt", ios::app);
//    file << "\n\n";
//    for (int i = 1; i < height_ - 1; ++i) {
//        file << "\n\n";
//        for (int j = 1; j < width_ - 1; ++j) {
//            ostringstream foss;
//            for (auto x : nodes_[i * width_ + j]) {
//                foss << "(" << x % width_ << "," << x / width_ << ")";
//            }
//            int w = 25;
//            if (j == 1 || j == width_ - 2) w = 20;
//            file << setw(w) << foss.str();
//        }
//    }
//    file.close();

    // Calculate next snake head position
    vector<int> shortestPathToFood = Algorithm::FindShortestPath(nodes_, snakeHeadIndex_, foodIndex_);
    int nextCellIndex;
    Direction nextDirection;
    if (shortestPathToFood.size()) {
        nextCellIndex = shortestPathToFood[0];
        nextDirection = __FindMovementDirection(snakeHeadIndex_, nextCellIndex);
    }
    else {
        int straightIndex = __FindCellFromMovementDirection(snakeHeadIndex_, currentDirection_);
        if (field_[straightIndex].type == CellType::PASS || straightIndex == snakeAssIndex_) {
            nextCellIndex = straightIndex;
            nextDirection = currentDirection_;
        }
        else {
            nextDirection = toLeftFrom(currentDirection_);
            nextCellIndex = __FindCellFromMovementDirection(snakeHeadIndex_, nextDirection);
            if (field_[nextCellIndex].type != CellType::PASS && nextCellIndex != snakeAssIndex_) {
                nextDirection = toRightFrom(currentDirection_);
                nextCellIndex = __FindCellFromMovementDirection(snakeHeadIndex_, nextDirection);
                if (field_[nextCellIndex].type != CellType::PASS && nextCellIndex != snakeAssIndex_) {
                    nextCellIndex = straightIndex;  // nowhere to go, so crushing forward
                    nextDirection = currentDirection_;
                }
            }
        }
    }

    if (field_[nextCellIndex].type == CellType::WALL || field_[nextCellIndex].type == CellType::SNAKE_BODY && nextCellIndex != snakeAssIndex_) {
        gameOn_ = false;
        crashDirection_ = nextDirection;
        return;
    }
    headAndFoodIndexes_.push_back(nextCellIndex);

    // Update currentPassCells_ and nodes_
    vector<int>::iterator iter;
    if (nextCellIndex != snakeAssIndex_) {
        iter = find(currentPassCells_.begin(), currentPassCells_.end(), nextCellIndex);
        if (iter == currentPassCells_.end()) {
            setColor(Color::NORMAL);
            throw runtime_error("Next cell index not in currentPassCells_, but should be.");
        }
        currentPassCells_.erase(iter);
    }
    if (!snakeTurns_.empty()) {
        for (auto& index : nodes_[snakeHeadIndex_]) {
            iter = find(nodes_[index].begin(), nodes_[index].end(), snakeHeadIndex_);
            if (iter != nodes_[index].end()) nodes_[index].erase(iter);
        }
        nodes_[snakeHeadIndex_].clear();
    }

    snakeTurns_.push(nextDirection);
    bool gotFood = field_[nextCellIndex].type == CellType::FOOD;
    field_[snakeHeadIndex_].type = CellType::SNAKE_BODY;
    field_[nextCellIndex].type = CellType::SNAKE_HEAD;

    if (gotFood) {
        foodIndex_ = currentPassCells_[randomUnder(currentPassCells_.size())];
        field_[foodIndex_].type = CellType::FOOD;

        if (snakeTurns_.size() == 1) {  // snake length becomes 2
            iter = find(nodes_[nextCellIndex].begin(), nodes_[nextCellIndex].end(), snakeHeadIndex_);
            nodes_[nextCellIndex].erase(iter);  // so that it couldn't take a 180 degree turn and continue from its ass
        }

        headAndFoodIndexes_.push_back(foodIndex_);
    }
    else {  // field_[nextCellIndex].type == (CellType::PASS) || nextCellIndex == snakeAssIndex_
        int nextAssIndex = __FindCellFromMovementDirection(snakeAssIndex_, snakeTurns_.front());
        snakeTurns_.pop();

        if (snakeAssIndex_ != nextCellIndex) {
            field_[snakeAssIndex_].type = CellType::PASS;
            currentPassCells_.push_back(snakeAssIndex_);
        }
        for (auto& index : __GetCellVicinityByIndexes(nextAssIndex)) {
            if (field_[index].type == CellType::PASS ||
                field_[index].type == CellType::FOOD ||
                field_[index].type == CellType::SNAKE_HEAD && snakeTurns_.size() > 1)
            {
                iter = find(nodes_[nextAssIndex].begin(), nodes_[nextAssIndex].end(), index);
                if (iter == nodes_[nextAssIndex].end()) nodes_[nextAssIndex].push_back(index);

                iter = find(nodes_[index].begin(), nodes_[index].end(), nextAssIndex);
                if (iter == nodes_[index].end()) nodes_[index].push_back(nextAssIndex);
            }
        }
        snakeAssIndex_ = nextAssIndex;
    }

    currentDirection_ = nextDirection;
    snakeHeadIndex_ = nextCellIndex;

//    _getch();
//    draw::Field(field_, width_);
//    for (int i = 1; i < shortestPathToFood.size() - 1; ++i) draw::GameCell(field_[shortestPathToFood[i]], Color::BEIGE_ON_BLUE);  // путь к еде
}

void Playground::SaveLastGame()
{
    fileHandler.SaveLastGame(
        firstFoodIndex_,
        crashDirection_,
        snakeTurns_.size() + 1,
        headAndFoodIndexes_,
        width_
    );
}

Direction Playground::__FindMovementDirection(int fromIndex, int toIndex)
{
    int fromX, fromY, toX, toY;
    fromX = fromIndex % width_;
    fromY = fromIndex / width_;
    toX = toIndex % width_;
    toY = toIndex / width_;

    if (fromY == toY) {
        if (abs(fromX - toX) == 1) {
            if (toX < fromX) return Direction::LEFT;
            else return Direction::RIGHT;
        }
        else {
            if (toX < fromX) return Direction::RIGHT;
            else return Direction::LEFT;
        }
    }
    else {  // fromX == toX
        if (abs(fromY - toY) == 1) {
            if (toY < fromY) return Direction::UP;
            else return Direction::DOWN;
        }
        else {
            if (toY < fromY) return Direction::DOWN;
            else return Direction::UP;
        }
    }
}

int Playground::__FindCellFromMovementDirection(int cellIndex, Direction movementDirection)
{
    if (movementDirection == Direction::LEFT) cellIndex--;
    else if (movementDirection == Direction::RIGHT) cellIndex++;
    else if (movementDirection == Direction::UP) cellIndex -= width_;
    else cellIndex += width_;

    if (field_[cellIndex].type == CellType::PORTAL) cellIndex = __GetPortalExitIndex(cellIndex, movementDirection);
    return cellIndex;
}

vector<int> Playground::__GetCellVicinityByIndexes(int cellIndex)
{
    int x, y, leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;
    x = cellIndex % width_,
    y = cellIndex / width_;
    leftCellIndex = y * width_ + x - 1;
    rightCellIndex = y * width_ + x + 1;
    topCellIndex = (y - 1) * width_ + x;
    bottomCellIndex = (y + 1) * width_ + x;

    if (field_[leftCellIndex].type == CellType::PORTAL) leftCellIndex = __GetPortalExitIndex(leftCellIndex, Direction::LEFT);
    if (field_[rightCellIndex].type == CellType::PORTAL) rightCellIndex = __GetPortalExitIndex(rightCellIndex, Direction::RIGHT);
    if (field_[topCellIndex].type == CellType::PORTAL) topCellIndex = __GetPortalExitIndex(topCellIndex, Direction::UP);
    if (field_[bottomCellIndex].type == CellType::PORTAL) bottomCellIndex = __GetPortalExitIndex(bottomCellIndex, Direction::DOWN);

    return {leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex};
}

int Playground::__GetPortalExitIndex(int portalEnterIndex, Direction movementDirection)
{
    int possibleExitIndex = portalEnterIndex, nextCellIndex;
    int x = portalEnterIndex % width_,
        y = portalEnterIndex / width_;

    if (movementDirection == Direction::LEFT) {
        nextCellIndex = y * width_ + ++x;
        while (x < width_) {
            if (field_[nextCellIndex].type == CellType::PORTAL) return possibleExitIndex;
            else {
                possibleExitIndex = nextCellIndex;
                nextCellIndex = y * width_ + ++x;
            }
        }
        return -1;
    }
    else if (movementDirection == Direction::RIGHT) {
        nextCellIndex = y * width_ + --x;
        while (x >= 0) {
            if (field_[nextCellIndex].type == CellType::PORTAL) return possibleExitIndex;
            else {
                possibleExitIndex = nextCellIndex;
                nextCellIndex = y * width_ + --x;
            }
        }
        return -1;
    }
    else if (movementDirection == Direction::UP) {
        nextCellIndex = ++y * width_ + x;
        while (y < height_) {
            if (field_[nextCellIndex].type == CellType::PORTAL) return possibleExitIndex;
            else {
                possibleExitIndex = nextCellIndex;
                nextCellIndex = ++y * width_ + x;
            }
        }
        return -1;
    }
    else {  // movementDirection == Direction::DOWN
        nextCellIndex = --y * width_ + x;
        while (y >= 0) {
            if (field_[nextCellIndex].type == CellType::PORTAL) return possibleExitIndex;
            else {
                possibleExitIndex = nextCellIndex;
                nextCellIndex = --y * width_ + x;
            }
        }
        return -1;
    }
}

void Playground::__ArrangeFieldElements()
{
    // Draw empty field
    draw::Field(field_, width_, true);

    MouseInput mouseInput;
    bool isBorder, isCorner, isAdjacentToCorner;
    int cellIndex;
    bool needToRemoveAlert = false;
    while (true) {
        if (needToRemoveAlert) {
            needToRemoveAlert = false;
            this_thread::sleep_for(chrono::seconds(1));
            mouseInput.GetAnyClick();
            draw::alert::Remove();
            __MovePortalsBackToBorder();
        }

        // Arrangement of remaining elements
        while (true) {
            mouseInput.GetClickInfo();
            int clickedX = mouseInput.X / 2;
            int clickedY = mouseInput.Y;

            if (mouseInput.buttonPressed == ButtonPressed::WHEEL ||
                clickedX == nConsoleWidth / 2 - 1 && clickedY == nConsoleHeight - 1)  // cheatcode to continue
            {
                setColor(Color::NORMAL);
                break;
            }

            if (clickedX >= indentX_ && clickedX <= indentX_ + width_ - 1 &&
                clickedY >= indentY_ && clickedY <= indentY_ + height_ - 1)
            {
                cellIndex = explainClickInfo(
                    clickedX - indentX_,
                    clickedY - indentY_,
                    width_,
                    height_,
                    isBorder,
                    isCorner,
                    isAdjacentToCorner
                );

                if (isCorner) continue;
                if (isBorder) {
                    CellType typeToChangeTo = CellType::UNKNOWN;
                    if (mouseInput.buttonPressed == ButtonPressed::LEFT_BUTTON && field_[cellIndex].type != CellType::PORTAL) {
                        typeToChangeTo = CellType::PORTAL;
                        draw::GameCell(field_[cellIndex], typeToChangeTo);
                    }
                    else if (mouseInput.buttonPressed == ButtonPressed::RIGHT_BUTTON && field_[cellIndex].type != CellType::WALL) {
                        typeToChangeTo = CellType::WALL;
                        draw::GameCell(field_[cellIndex], typeToChangeTo);
                    }

                    if (typeToChangeTo != CellType::UNKNOWN) {
                        int oppositeCellIndex = getOppositeBorderCellIndex(clickedX - indentX_, clickedY - indentY_, width_, height_);
                        draw::GameCell(field_[oppositeCellIndex], typeToChangeTo);

                        if (isAdjacentToCorner) {  // updating corner
                            int pairedAdjacentCellIndex, cornerCellIndex;
                            getPairedAdjacentCellAndCornerCellIndex(
                                pairedAdjacentCellIndex,
                                cornerCellIndex,
                                clickedX - indentX_,
                                clickedY - indentY_,
                                width_,
                                height_
                            );
                            if (field_[cellIndex].type == CellType::PORTAL && field_[pairedAdjacentCellIndex].type == CellType::PORTAL) {
                                draw::GameCell(field_[cornerCellIndex], CellType::PORTAL);
                            }
                            else draw::GameCell(field_[cornerCellIndex], CellType::WALL);

                            getPairedAdjacentCellAndCornerCellIndex(
                                pairedAdjacentCellIndex,
                                cornerCellIndex,
                                oppositeCellIndex % width_,
                                oppositeCellIndex / width_,
                                width_,
                                height_
                            );
                            if (field_[oppositeCellIndex].type == CellType::PORTAL && field_[pairedAdjacentCellIndex].type == CellType::PORTAL) {
                                draw::GameCell(field_[cornerCellIndex], CellType::PORTAL);
                            }
                            else draw::GameCell(field_[cornerCellIndex], CellType::WALL);
                        }
                    }
                }

                else {  // if cell is inside field
                    if (mouseInput.buttonPressed == ButtonPressed::LEFT_BUTTON) {
                        draw::GameCell(field_[cellIndex], CellType::WALL);
                    }
                    else if (mouseInput.buttonPressed == ButtonPressed::RIGHT_BUTTON) {
                        draw::GameCell(field_[cellIndex], CellType::PASS);
                    }
                    else if (mouseInput.buttonPressed == ButtonPressed::CTRL_LEFT) {
                        draw::GameCell(field_[cellIndex], CellType::SNAKE_BODY);
                    }
                    else if (mouseInput.buttonPressed == ButtonPressed::CTRL_RIGHT) {
                        if (field_[cellIndex].type == CellType::SNAKE_BODY) {
                            draw::GameCell(field_[cellIndex], CellType::SNAKE_HEAD);
                        }
                    }
                }
            }
        }

        __AdjustPortals();

        // Validation
        if (!validation.SnakeSingularityAndCorrectness(field_, width_)) {
            if (validation.snakesAmount != 1) {
                draw::alert::MultimpleOrNoneSnakes(validation.snakesAmount);
                needToRemoveAlert = true;
                continue;
            }
            else if (!validation.snakeIsCorrect) {
                draw::alert::IncorrectSnake();
                needToRemoveAlert = true;
                continue;
            }
            else if (validation.snakeIsLooped) {
                draw::alert::LoopedSnake();
                needToRemoveAlert = true;
                continue;
            }
        }
        if (!validation.ClosedSpacesExistence(field_, width_)) {
            draw::alert::ClosedSpaces();
            needToRemoveAlert = true;
            continue;
        }
        if (!validation.SnakeHeadIdentification(field_, width_)) {
            draw::alert::NoPossibleStart();
            needToRemoveAlert = true;
            continue;
        }

        __RepaintSnakeCells();

        // Initialization of playground object
        __InitializePlayground();

        break;
    }
}

void Playground::__InitializePlayground()
{
    // Adjacency list
    nodes_.resize(field_.size());
    for (int i = 0; i < nodes_.size(); ++i) nodes_[i] = {};
    currentPassCells_.clear();
    __FillAdjacencyList();

    // Snake turns
    currentDirection_ = validation.startingDirection;
    snakeTurns_ = {};
    __FillSnakeTurnsQueue();

    // Initialize initial data for future attempts
    initialField_ = field_;
    initialNodes_ = nodes_;
    initialCurrentPassCells_ = currentPassCells_;
    initialCurrentDirection_ = currentDirection_;
    initialSnakeTurns_ = snakeTurns_;
}

void Playground::__FillAdjacencyList()
{
    int spaceIndex = -1;
    for (int i = 0; i < field_.size(); ++i) {
        if (field_[i].type == CellType::PASS) {
            spaceIndex = i;
            break;
        }
    }
    if (spaceIndex == -1) {
        throw runtime_error("Unable to find space index while filling adjacency list (should be impossible to end up here)");
    }

    queue<int> traversalOrder;
    traversalOrder.push(spaceIndex);
    vector<bool> passed(field_.size());
    passed[spaceIndex] = true;
    currentPassCells_.push_back(spaceIndex);
    int cellIndex, x, y;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;
    int portalExit;

    while (!traversalOrder.empty()) {
        cellIndex = traversalOrder.front();
        traversalOrder.pop();
        x = cellIndex % width_;
        y = cellIndex / width_;

        leftCellIndex = y * width_ + (x - 1);
        rightCellIndex = y * width_ + (x + 1);
        topCellIndex = (y - 1) * width_ + x;
        bottomCellIndex = (y + 1) * width_ + x;

        if (field_[leftCellIndex].type == CellType::PORTAL) {
            portalExit = __GetPortalExitIndex(leftCellIndex, Direction::LEFT);
            if (portalExit != -1) leftCellIndex = portalExit;
        }
        if (field_[rightCellIndex].type == CellType::PORTAL) {
            portalExit = __GetPortalExitIndex(rightCellIndex, Direction::RIGHT);
            if (portalExit != -1) rightCellIndex = portalExit;
        }
        if (field_[topCellIndex].type == CellType::PORTAL) {
            portalExit = __GetPortalExitIndex(topCellIndex, Direction::UP);
            if (portalExit != -1) topCellIndex = portalExit;
        }
        if (field_[bottomCellIndex].type == CellType::PORTAL) {
            portalExit = __GetPortalExitIndex(bottomCellIndex, Direction::DOWN);
            if (portalExit != -1) bottomCellIndex = portalExit;
        }

        if (field_[leftCellIndex].type == CellType::PASS ||
            leftCellIndex == validation.snakeAssIndex && cellIndex != validation.startingCellIndex ||
            leftCellIndex == validation.startingCellIndex && cellIndex != validation.snakeAssIndex)
        {
            nodes_[cellIndex].push_back(leftCellIndex);
            if (!passed[leftCellIndex]) {
                passed[leftCellIndex] = true;
                traversalOrder.push(leftCellIndex);
                if (leftCellIndex != validation.snakeAssIndex && leftCellIndex != validation.startingCellIndex) {
                    currentPassCells_.push_back(leftCellIndex);
                }
            }
        }
        if (field_[rightCellIndex].type == CellType::PASS ||
            rightCellIndex == validation.snakeAssIndex && cellIndex != validation.startingCellIndex ||
            rightCellIndex == validation.startingCellIndex && cellIndex != validation.snakeAssIndex)
        {
            nodes_[cellIndex].push_back(rightCellIndex);
            if (!passed[rightCellIndex]) {
                passed[rightCellIndex] = true;
                traversalOrder.push(rightCellIndex);
                if (rightCellIndex != validation.snakeAssIndex && rightCellIndex != validation.startingCellIndex) {
                    currentPassCells_.push_back(rightCellIndex);
                }
            }
        }
        if (field_[topCellIndex].type == CellType::PASS ||
            topCellIndex == validation.snakeAssIndex && cellIndex != validation.startingCellIndex ||
            topCellIndex == validation.startingCellIndex && cellIndex != validation.snakeAssIndex)
        {
            nodes_[cellIndex].push_back(topCellIndex);
            if (!passed[topCellIndex]) {
                passed[topCellIndex] = true;
                traversalOrder.push(topCellIndex);
                if (topCellIndex != validation.snakeAssIndex && topCellIndex != validation.startingCellIndex) {
                    currentPassCells_.push_back(topCellIndex);
                }
            }
        }
        if (field_[bottomCellIndex].type == CellType::PASS ||
            bottomCellIndex == validation.snakeAssIndex && cellIndex != validation.startingCellIndex ||
            bottomCellIndex == validation.startingCellIndex && cellIndex != validation.snakeAssIndex)
        {
            nodes_[cellIndex].push_back(bottomCellIndex);
            if (!passed[bottomCellIndex]) {
                passed[bottomCellIndex] = true;
                traversalOrder.push(bottomCellIndex);
                if (bottomCellIndex != validation.snakeAssIndex && bottomCellIndex != validation.startingCellIndex) {
                    currentPassCells_.push_back(bottomCellIndex);
                }
            }
        }
    }
}

void Playground::__FillSnakeTurnsQueue()
{
    queue<int> traversalOrder;
    traversalOrder.push(validation.startingCellIndex);
    unordered_set<int> passed = {validation.startingCellIndex};
    stack<Direction> snakeTurnsReversed;
    int cellIndex, x, y;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;

    while (!traversalOrder.empty()) {
        cellIndex = traversalOrder.front();
        traversalOrder.pop();
        x = cellIndex % width_;
        y = cellIndex / width_;

        leftCellIndex = y * width_ + (x - 1);
        rightCellIndex = y * width_ + (x + 1);
        topCellIndex = (y - 1) * width_ + x;
        bottomCellIndex = (y + 1) * width_ + x;

        if (field_[leftCellIndex].type == CellType::SNAKE_BODY && !passed.count(leftCellIndex)) {
            passed.insert(leftCellIndex);
            snakeTurnsReversed.push(Direction::RIGHT);
            traversalOrder.push(leftCellIndex);
        }
        else if (field_[rightCellIndex].type == CellType::SNAKE_BODY && !passed.count(rightCellIndex)) {
            passed.insert(rightCellIndex);
            snakeTurnsReversed.push(Direction::LEFT);
            traversalOrder.push(rightCellIndex);
        }
        else if (field_[topCellIndex].type == CellType::SNAKE_BODY && !passed.count(topCellIndex)) {
            passed.insert(topCellIndex);
            snakeTurnsReversed.push(Direction::DOWN);
            traversalOrder.push(topCellIndex);
        }
        else if (field_[bottomCellIndex].type == CellType::SNAKE_BODY && !passed.count(bottomCellIndex)) {
            passed.insert(bottomCellIndex);
            snakeTurnsReversed.push(Direction::UP);
            traversalOrder.push(bottomCellIndex);
        }
    }

    while (!snakeTurnsReversed.empty()) {
        snakeTurns_.push(snakeTurnsReversed.top());
        snakeTurnsReversed.pop();
    }
}

void Playground::__AdjustPortals()
{
    int index;
    for (int x = 1; x < width_ - 1; ++x) {
        if (field_[x].type == CellType::PORTAL) __CalculatePortalEntries(Orientation::VERTICAL, x);
    }
    for (int y = 1; y < height_ - 1; ++y) {
        index = y * width_;
        if (field_[index].type == CellType::PORTAL) __CalculatePortalEntries(Orientation::HORIZONTAL, y);
    }

    // remove corner "portals"
    int upperLeftCorner = 0,
        upperRightCorner = width_ - 1,
        lowerLeftCorner = (height_ - 1) * width_,
        lowerRightCorner = width_ * height_ - 1;
    int upperLeftAdj1 = 1,
        upperLeftAdj2 = width_,
        upperRightAdj1 = width_ - 2,
        upperRightAdj2 = 2 * width_ - 1,
        lowerLeftAdj1 = (height_ - 2) * width_,
        lowerLeftAdj2 = (height_ - 1) * width_ + 1,
        lowerRightAdj1 = (height_ - 2) * width_ + (width_ - 1),
        lowerRightAdj2 = width_ * height_ - 2;
    if (field_[upperLeftAdj1].type != CellType::PORTAL || field_[upperLeftAdj2].type != CellType::PORTAL) {
        draw::GameCell(field_[upperLeftCorner], CellType::WALL);
    }
    if (field_[upperRightAdj1].type != CellType::PORTAL || field_[upperRightAdj2].type != CellType::PORTAL) {
        draw::GameCell(field_[upperRightCorner], CellType::WALL);
    }
    if (field_[lowerLeftAdj1].type != CellType::PORTAL || field_[lowerLeftAdj2].type != CellType::PORTAL) {
        draw::GameCell(field_[lowerLeftCorner], CellType::WALL);
    }
    if (field_[lowerRightAdj1].type != CellType::PORTAL || field_[lowerRightAdj2].type != CellType::PORTAL) {
        draw::GameCell(field_[lowerRightCorner], CellType::WALL);
    }
}

void Playground::__CalculatePortalEntries(Orientation orientation, int axisValue)
{
    int index, possiblePortalIndex, nextCellIndex;
    if (orientation == Orientation::VERTICAL) {
        assert(axisValue >= 1 && axisValue < width_ - 1);
        const int x = axisValue;

        if (__WholeAxisIsAWall(orientation, axisValue)) {
            draw::GameCell(field_[x], CellType::WALL);
            index = (height_ - 1) * width_ + x;
            draw::GameCell(field_[index], CellType::WALL);
            return;
        }

        int y = 0;
        possiblePortalIndex = y * width_ + x;
        while (true) {
            if (++y >= height_) {
                throw runtime_error("Unable to calculate portal entry.");
            }
            nextCellIndex = y * width_ + x;
            if (field_[nextCellIndex].type != CellType::WALL) {
                if (y != 1) draw::GameCell(field_[possiblePortalIndex], CellType::PORTAL);
                break;
            }
            else {
                if (y == 1) draw::GameCell(field_[possiblePortalIndex], CellType::WALL);
                possiblePortalIndex = nextCellIndex;
            }
        }

        y = height_ - 1;
        possiblePortalIndex = y * width_ + x;
        while (true) {
            if (--y < 0) {
                throw runtime_error("Unable to calculate portal entry.");
            }
            nextCellIndex = y * width_ + x;
            if (field_[nextCellIndex].type != CellType::WALL) {
                if (y != height_ - 2) draw::GameCell(field_[possiblePortalIndex], CellType::PORTAL);
                break;
            }
            else {
                if (y == height_ - 2) draw::GameCell(field_[possiblePortalIndex], CellType::WALL);
                possiblePortalIndex = nextCellIndex;
            }
        }
    }
    else {  // orientation == Orientation::HORIZONTAL
        assert(axisValue >= 1 && axisValue < height_ - 1);
        const int y = axisValue;

        if (__WholeAxisIsAWall(orientation, axisValue)) {
            index = y * width_;
            draw::GameCell(field_[index], CellType::WALL);
            index = y * width_ + (width_ - 1);
            draw::GameCell(field_[index], CellType::WALL);
            return;
        }

        int x = 0;
        possiblePortalIndex = y * width_ + x;
        while (true) {
            if (++x >= width_) {
                throw runtime_error("Unable to calculate portal entry.");
            }
            nextCellIndex = y * width_ + x;
            if (field_[nextCellIndex].type != CellType::WALL && field_[nextCellIndex].type != CellType::PORTAL) {
                if (x != 1) draw::GameCell(field_[possiblePortalIndex], CellType::PORTAL);
                break;
            }
            else {
                if (x == 1) draw::GameCell(field_[possiblePortalIndex], CellType::WALL);
                possiblePortalIndex = nextCellIndex;
            }
        }

        x = width_ - 1;
        possiblePortalIndex = y * width_ + x;
        while (true) {
            if (--x < 0) {
                throw runtime_error("Unable to calculate portal entry.");
            }
            nextCellIndex = y * width_ + x;
            if (field_[nextCellIndex].type != CellType::WALL && field_[nextCellIndex].type != CellType::PORTAL) {
                if (x != width_ - 2) draw::GameCell(field_[possiblePortalIndex], CellType::PORTAL);
                break;
            }
            else {
                if (x == width_ - 2) draw::GameCell(field_[possiblePortalIndex], CellType::WALL);
                possiblePortalIndex = nextCellIndex;
            }
        }
    }
}

bool Playground::__WholeAxisIsAWall(Orientation orientation, int axisValue)
{
    int index;
    if (orientation == Orientation::VERTICAL) {
        const int x = axisValue;
        for (int y = 1; y < height_ - 1; ++y) {
            index = y * width_ + x;
            if (field_[index].type != CellType::WALL && field_[index].type != CellType::PORTAL) return false;
        }
    }
    else {  // orientation == Orientation::HORIZONTAL
        const int y = axisValue;
        for (int x = 1; x < width_ - 1; ++x) {
            index = y * width_ + x;
            if (field_[index].type != CellType::WALL && field_[index].type != CellType::PORTAL) return false;
        }
    }
    return true;
}

void Playground::__MovePortalsBackToBorder()
{
    int leftIndex, rightIndex, upperIndex, lowerIndex;
    for (int x = 1; x < width_ - 1; ++x) {
        upperIndex = x;
        lowerIndex = (height_ - 1) * width_ + x;
        if (field_[upperIndex].type == CellType::PORTAL || field_[lowerIndex].type == CellType::PORTAL) {
            draw::GameCell(field_[upperIndex], CellType::PORTAL);
            draw::GameCell(field_[lowerIndex], CellType::PORTAL);
        }
    }
    for (int y = 1; y < height_ - 1; ++y) {
        leftIndex = y * width_;
        rightIndex = y * width_ + (width_ - 1);
        if (field_[leftIndex].type == CellType::PORTAL || field_[rightIndex].type == CellType::PORTAL) {
            draw::GameCell(field_[leftIndex], CellType::PORTAL);
            draw::GameCell(field_[rightIndex], CellType::PORTAL);
        }
    }

    // also removing portals that both made it inside field (bc it is impossible to identify where they are from)
    for (int i = 0; i < field_.size(); ++i) {
        if ((i % width_ != 0) && (i % width_ != width_ - 1) && (i / width_ != 0) && (i / width_ != height_ - 1)) {
            if (field_[i].type == CellType::PORTAL) draw::GameCell(field_[i], CellType::WALL);
        }
    }

    // add corner "portals" back
    int upperLeftCorner = 0,
        upperRightCorner = width_ - 1,
        lowerLeftCorner = (height_ - 1) * width_,
        lowerRightCorner = width_ * height_ - 1;
    int upperLeftAdj1 = 1,
        upperLeftAdj2 = width_,
        upperRightAdj1 = width_ - 2,
        upperRightAdj2 = 2 * width_ - 1,
        lowerLeftAdj1 = (height_ - 2) * width_,
        lowerLeftAdj2 = (height_ - 1) * width_ + 1,
        lowerRightAdj1 = (height_ - 2) * width_ + (width_ - 1),
        lowerRightAdj2 = width_ * height_ - 2;
    if (field_[upperLeftAdj1].type == CellType::PORTAL && field_[upperLeftAdj2].type == CellType::PORTAL) {
        draw::GameCell(field_[upperLeftCorner], CellType::PORTAL);
    }
    if (field_[upperRightAdj1].type == CellType::PORTAL && field_[upperRightAdj2].type == CellType::PORTAL) {
        draw::GameCell(field_[upperRightCorner], CellType::PORTAL);
    }
    if (field_[lowerLeftAdj1].type == CellType::PORTAL && field_[lowerLeftAdj2].type == CellType::PORTAL) {
        draw::GameCell(field_[lowerLeftCorner], CellType::PORTAL);
    }
    if (field_[lowerRightAdj1].type == CellType::PORTAL && field_[lowerRightAdj2].type == CellType::PORTAL) {
        draw::GameCell(field_[lowerRightCorner], CellType::PORTAL);
    }
}

void Playground::__RepaintSnakeCells()
{
    queue<int> traversalOrder;
    traversalOrder.push(validation.startingCellIndex);
    unordered_set<int> passed;
    int cellIndex, x, y;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;

    while (!traversalOrder.empty()) {
        cellIndex = traversalOrder.front();
        traversalOrder.pop();
        passed.insert(cellIndex);

        if (cellIndex == validation.startingCellIndex) draw::GameCell(field_[cellIndex], CellType::SNAKE_HEAD);
        else draw::GameCell(field_[cellIndex], CellType::SNAKE_BODY);

        x = cellIndex % width_,
        y = cellIndex / width_;
        leftCellIndex = y * width_ + (x - 1);
        rightCellIndex = y * width_ + (x + 1);
        topCellIndex = (y - 1) * width_ + x;
        bottomCellIndex = (y + 1) * width_ + x;

        if (validation.IsASnake(field_[leftCellIndex]) && !passed.count(leftCellIndex)) {
            traversalOrder.push(leftCellIndex);
        }
        else if (validation.IsASnake(field_[rightCellIndex]) && !passed.count(rightCellIndex)) {
            traversalOrder.push(rightCellIndex);
        }
        else if (validation.IsASnake(field_[topCellIndex]) && !passed.count(topCellIndex)) {
            traversalOrder.push(topCellIndex);
        }
        else if (validation.IsASnake(field_[bottomCellIndex]) && !passed.count(bottomCellIndex)) {
            traversalOrder.push(bottomCellIndex);
        }
    }

    setColor(Color::NORMAL);
}
