#include "validation.h"

#include <unordered_set>
#include <queue>
#include <cassert>
//#include <thread>

using namespace std;


bool Validation::SnakeSingularityAndCorrectness(const vector<Cell>& field, int fieldWidth)
{
    int fieldHeight = field.size() / fieldWidth;
    vector<bool> passed(field.size());
    snakesAmount = 0;
    snakeIsCorrect = true;
    snakeIsLooped = true;

    for (int i = 0; i < field.size(); ++i) {
        if (passed[i]) continue;
        passed[i] = true;
        if (IsASnake(field[i])) {
            snakesAmount++;
            __GetInfoAboutWholeSnake(field, fieldWidth, passed, i);
        }
    }

    if (snakesAmount == 1 && snakeIsCorrect && !snakeIsLooped) return true;
    return false;
}

void Validation::__GetInfoAboutWholeSnake(const vector<Cell>& field, int fieldWidth, vector<bool>& passed, int snakeIndex)
{
    queue<int> traversalOrder;
    traversalOrder.push(snakeIndex);
    int cellIndex, x, y;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;
    int neighboursAmount;

    while (!traversalOrder.empty()) {
        cellIndex = traversalOrder.front();
        traversalOrder.pop();

        x = cellIndex % fieldWidth;
        y = cellIndex / fieldWidth;
        leftCellIndex = y * fieldWidth + (x - 1);
        rightCellIndex = y * fieldWidth + (x + 1);
        topCellIndex = (y - 1) * fieldWidth + x;
        bottomCellIndex = (y + 1) * fieldWidth + x;

        neighboursAmount = 0;
        if (IsASnake(field[leftCellIndex])) {
            neighboursAmount++;
            if (!passed[leftCellIndex]) {
                traversalOrder.push(leftCellIndex);
                passed[leftCellIndex] = true;
            }
        }
        if (IsASnake(field[rightCellIndex])) {
            neighboursAmount++;
            if (!passed[rightCellIndex]) {
                traversalOrder.push(rightCellIndex);
                passed[rightCellIndex] = true;
            }
        }
        if (IsASnake(field[topCellIndex])) {
            neighboursAmount++;
            if (!passed[topCellIndex]) {
                traversalOrder.push(topCellIndex);
                passed[topCellIndex] = true;
            }
        }
        if (IsASnake(field[bottomCellIndex])) {
            neighboursAmount++;
            if (!passed[bottomCellIndex]) {
                traversalOrder.push(bottomCellIndex);
                passed[bottomCellIndex] = true;
            }
        }

        if (neighboursAmount > 2) snakeIsCorrect = false;
        if (neighboursAmount <= 1) snakeIsLooped = false;
    }
}

bool Validation::ClosedSpacesExistence(const vector<Cell>& field, int fieldWidth)
{
    int fieldHeight = field.size() / fieldWidth;
    queue<int> traversalOrder;
    vector<bool> passed(field.size());

    int index = __GetFirstSpaceIndex(field, passed);
    if (index == -1) throw runtime_error("No space index found. Should be impossible end up here.");
    traversalOrder.push(index);
    passed[index] = true;
//    int cycle = 1;

    int x, y, cellIndex;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;
    while (!traversalOrder.empty()) {
        cellIndex = traversalOrder.front();
        traversalOrder.pop();
        x = cellIndex % fieldWidth;
        y = cellIndex / fieldWidth;
//        draw::GameCell(field[cellIndex], Color::BRIGHT_RED);

        leftCellIndex = y * fieldWidth + (x - 1);
        rightCellIndex = y * fieldWidth + (x + 1);
        topCellIndex = (y - 1) * fieldWidth + x;
        bottomCellIndex = (y + 1) * fieldWidth + x;

        if (field[leftCellIndex].type == CellType::PORTAL) {
            for (int xx = x + 1; xx < fieldWidth; ++xx) {
                cellIndex = y * fieldWidth + xx;
                if (field[cellIndex].type == CellType::PORTAL) {
                    leftCellIndex = cellIndex - 1;
                    break;
                }
            }
        }
        if (field[rightCellIndex].type == CellType::PORTAL) {
            for (int xx = x - 1; xx >= 0; --xx) {
                cellIndex = y * fieldWidth + xx;
                if (field[cellIndex].type == CellType::PORTAL) {
                    rightCellIndex = cellIndex + 1;
                    break;
                }
            }
        }
        if (field[topCellIndex].type == CellType::PORTAL) {
            for (int yy = y + 1; yy < fieldHeight; ++yy) {
                cellIndex = yy * fieldWidth + x;
                if (field[cellIndex].type == CellType::PORTAL) {
                    topCellIndex = cellIndex - fieldWidth;
                    break;
                }
            }
        }
        if (field[bottomCellIndex].type == CellType::PORTAL) {
            for (int yy = y - 1; yy >= 0; --yy) {
                cellIndex = yy * fieldWidth + x;
                if (field[cellIndex].type == CellType::PORTAL) {
                    bottomCellIndex = cellIndex + fieldWidth;
                    break;
                }
            }
        }

        if (__IsPossibleSpace(field[leftCellIndex]) && !passed[leftCellIndex]) {
            traversalOrder.push(leftCellIndex);
            passed[leftCellIndex] = true;
//            draw::GameCell(field[leftCellIndex], Color::RED);
        }
        if (__IsPossibleSpace(field[rightCellIndex]) && !passed[rightCellIndex]) {
            traversalOrder.push(rightCellIndex);
            passed[rightCellIndex] = true;
//            draw::GameCell(field[rightCellIndex], Color::RED);
        }
        if (__IsPossibleSpace(field[topCellIndex]) && !passed[topCellIndex]) {
            traversalOrder.push(topCellIndex);
            passed[topCellIndex] = true;
//            draw::GameCell(field[topCellIndex], Color::RED);
        }
        if (__IsPossibleSpace(field[bottomCellIndex]) && !passed[bottomCellIndex]) {
            traversalOrder.push(bottomCellIndex);
            passed[bottomCellIndex] = true;
//            draw::GameCell(field[bottomCellIndex], Color::RED);
        }

//        // slowdown for cute animation
//        if (--cycle == 0) {
//            cycle = traversalOrder.size();
//            this_thread::sleep_for(30ms);
//        }
    }

    if (__GetFirstSpaceIndex(field, passed) != -1) return false;
    return true;
}

bool Validation::SnakeHeadIdentification(const vector<Cell>& field, int fieldWidth)
{
    int snakeEnd1, snakeEnd2;
    __FindSnakeEnds(field, fieldWidth, snakeEnd1, snakeEnd2);

    Cell end1Left, end1Right, end1Top, end1Bottom;
    Cell end2Left, end2Right, end2Top, end2Bottom;

    if (snakeEnd1 == snakeEnd2) {  // snake consists of a single cell
        startingCellIndex = snakeEnd1;
        snakeAssIndex = snakeEnd1;
        __GetCellVicinity(field, fieldWidth, snakeEnd1, end1Left, end1Right, end1Top, end1Bottom);

        unordered_set<CellType> endDirections = {end1Left.type, end1Right.type, end1Top.type, end1Bottom.type};
        if (!endDirections.count(CellType::PASS) && !endDirections.count(CellType::PORTAL)) return false;

        vector<Direction> possibleStartingDirections;
        possibleStartingDirections.reserve(4);
        if (__IsPossiblePassage(end1Left)) possibleStartingDirections.push_back(Direction::LEFT);
        if (__IsPossiblePassage(end1Right)) possibleStartingDirections.push_back(Direction::RIGHT);
        if (__IsPossiblePassage(end1Top)) possibleStartingDirections.push_back(Direction::UP);
        if (__IsPossiblePassage(end1Bottom)) possibleStartingDirections.push_back(Direction::DOWN);
        startingDirection = possibleStartingDirections[randomUnder(possibleStartingDirections.size())];
        return true;
    }

    __GetCellVicinity(field, fieldWidth, snakeEnd1, end1Left, end1Right, end1Top, end1Bottom);
    __GetCellVicinity(field, fieldWidth, snakeEnd2, end2Left, end2Right, end2Top, end2Bottom);

    unordered_set<CellType> end1Directions = {end1Left.type, end1Right.type, end1Top.type, end1Bottom.type};
    unordered_set<CellType> end2Directions = {end2Left.type, end2Right.type, end2Top.type, end2Bottom.type};
    bool possibleStartFromEnd1 = false, possibleStartFromEnd2 = false;
    if (end1Directions.count(CellType::PASS) || end1Directions.count(CellType::PORTAL)) {
        possibleStartFromEnd1 = true;
    }
    if (end2Directions.count(CellType::PASS) || end2Directions.count(CellType::PORTAL)) {
        possibleStartFromEnd2 = true;
    }

    if (!possibleStartFromEnd1 && !possibleStartFromEnd2) return false;

    Cell end1 = field[snakeEnd1];
    Cell end2 = field[snakeEnd2];
    if (possibleStartFromEnd1 && possibleStartFromEnd2 && end1.type == end2.type) {
        if (randomUnder(2)) {
            swap(snakeEnd1, snakeEnd2);
            swap(end1, end2);
            swap(end1Left, end2Left);
            swap(end1Right, end2Right);
            swap(end1Top, end2Top);
            swap(end1Bottom, end2Bottom);
        }
    }

    if (end1.type == CellType::SNAKE_HEAD && possibleStartFromEnd1 || !possibleStartFromEnd2) {
        startingCellIndex = snakeEnd1;
        snakeAssIndex = snakeEnd2;
        startingDirection = __CalculateStartingDirection(end1Left, end1Right, end1Top, end1Bottom);
    }
    else {  // starting from 2nd end
        startingCellIndex = snakeEnd2;
        snakeAssIndex = snakeEnd1;
        startingDirection = __CalculateStartingDirection(end2Left, end2Right, end2Top, end2Bottom);
    }
    return true;
}

Direction Validation::__CalculateStartingDirection(const Cell& leftCell, const Cell& rightCell, const Cell& topCell, const Cell& bottomCell)
{
    if (IsASnake(leftCell)) {
        if (__IsPossiblePassage(rightCell)) return Direction::RIGHT;
        else {
            if (__IsPossiblePassage(topCell) && __IsPossiblePassage(bottomCell)) {
                if (randomUnder(2)) return Direction::UP;
                else return Direction::DOWN;
            }
            else if (__IsPossiblePassage(topCell)) return Direction::UP;
            else return Direction::DOWN;
        }
    }
    else if (IsASnake(rightCell)) {
        if (__IsPossiblePassage(leftCell)) return Direction::LEFT;
        else {
            if (__IsPossiblePassage(topCell) && __IsPossiblePassage(bottomCell)) {
                if (randomUnder(2)) return Direction::UP;
                else return Direction::DOWN;
            }
            else if (__IsPossiblePassage(topCell)) return Direction::UP;
            else return Direction::DOWN;
        }
    }
    else if (IsASnake(topCell)) {
        if (__IsPossiblePassage(bottomCell)) return Direction::DOWN;
        else {
            if (__IsPossiblePassage(leftCell) && __IsPossiblePassage(rightCell)) {
                if (randomUnder(2)) return Direction::LEFT;
                else return Direction::RIGHT;
            }
            else if (__IsPossiblePassage(leftCell)) return Direction::LEFT;
            else return Direction::RIGHT;
        }
    }
    else {
        if (__IsPossiblePassage(topCell)) return Direction::UP;
        else {
            if (__IsPossiblePassage(leftCell) && __IsPossiblePassage(rightCell)) {
                if (randomUnder(2)) return Direction::LEFT;
                else return Direction::RIGHT;
            }
            else if (__IsPossiblePassage(leftCell)) return Direction::LEFT;
            else return Direction::RIGHT;
        }
    }
}

void Validation::__GetCellVicinity(
    const vector<Cell>& field,
    int fieldWidth,
    int cellIndex,
    Cell& leftCell,
    Cell& rightCell,
    Cell& topCell,
    Cell& bottomCell
) {
    int fieldHeight = field.size() / fieldWidth;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;
    int x = cellIndex % fieldWidth,
        y = cellIndex / fieldWidth;

    leftCellIndex = y * fieldWidth + x - 1;
    rightCellIndex = y * fieldWidth + x + 1;
    topCellIndex = (y - 1) * fieldWidth + x;
    bottomCellIndex = (y + 1) * fieldWidth + x;

    Cell noneCell = Cell();
    if (x - 1 >= 0) leftCell = field[leftCellIndex];
    else leftCell = noneCell;
    if (x + 1 < fieldWidth) rightCell = field[rightCellIndex];
    else rightCell = noneCell;
    if (y - 1 >= 0) topCell = field[topCellIndex];
    else topCell = noneCell;
    if (y + 1 < fieldHeight) bottomCell = field[bottomCellIndex];
    else bottomCell = noneCell;
}

void Validation::__FindSnakeEnds(const vector<Cell>& field, int fieldWidth, int& snakeEnd1, int& snakeEnd2)
{
    int snakeIndex = -1;
    for (int i = 0; i < field.size(); ++i) {
        if (IsASnake(field[i])) {
            snakeIndex = i;
            break;
        }
    }
    assert(snakeIndex != -1);

    queue<int> traversalOrder;
    traversalOrder.push(snakeIndex);
    unordered_set<int> passedCells;
    int cellIndex, x, y;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;
    int neighboursAmount;
    int fieldHeight = field.size() / fieldWidth;
    snakeEnd1 = 0;

    while (!traversalOrder.empty()) {
        cellIndex = traversalOrder.front();
        traversalOrder.pop();
        x = cellIndex % fieldWidth;
        y = cellIndex / fieldWidth;

        passedCells.insert(cellIndex);
        leftCellIndex = y * fieldWidth + x - 1;
        rightCellIndex = y * fieldWidth + x + 1;
        topCellIndex = (y - 1) * fieldWidth + x;
        bottomCellIndex = (y + 1) * fieldWidth + x;
        neighboursAmount = 0;
        if (x - 1 >= 0 && IsASnake(field[leftCellIndex])) {
            neighboursAmount++;
            if (!passedCells.count(leftCellIndex)) traversalOrder.push(leftCellIndex);
        }
        if (x + 1 < fieldWidth && IsASnake(field[rightCellIndex])) {
            neighboursAmount++;
            if (!passedCells.count(rightCellIndex)) traversalOrder.push(rightCellIndex);
        }
        if (y - 1 >= 0 && IsASnake(field[topCellIndex])) {
            neighboursAmount++;
            if (!passedCells.count(topCellIndex)) traversalOrder.push(topCellIndex);
        }
        if (y + 1 < fieldHeight && IsASnake(field[bottomCellIndex])) {
            neighboursAmount++;
            if (!passedCells.count(bottomCellIndex)) traversalOrder.push(bottomCellIndex);
        }

        if (neighboursAmount == 1) {
            if (!snakeEnd1) snakeEnd1 = cellIndex;
            else snakeEnd2 = cellIndex;
        }
        else if (!neighboursAmount) {  // snake consists of a single cell
            snakeEnd1 = cellIndex;
            snakeEnd2 = cellIndex;
        }
    }
}

int Validation::__GetFirstSpaceIndex(const vector<Cell>& field, const vector<bool>& passed)
{
    int spaceIndex = -1;
    for (int i = 0; i < field.size(); ++i) {
        if (__IsPossibleSpace(field[i]) && !passed[i]) {
            spaceIndex = i;
            break;
        }
    }
    return spaceIndex;
}

bool Validation::IsASnake(const Cell& cell)
{
    if (cell.type == CellType::SNAKE_BODY || cell.type == CellType::SNAKE_HEAD) return true;
    return false;
}

bool Validation::IsASnake(CellType cellType)
{
    if (cellType == CellType::SNAKE_BODY || cellType == CellType::SNAKE_HEAD) return true;
    return false;
}

bool Validation::__IsPossibleSpace(const Cell& cell)
{
    unordered_set<CellType> possibleSpace = {
        CellType::PASS,
        CellType::SNAKE_BODY,
        CellType::SNAKE_HEAD
    };
    if (possibleSpace.count(cell.type)) return true;
    return false;
}

bool Validation::__IsPossiblePassage(const Cell& cell)
{
    if (cell.type == CellType::PASS || cell.type == CellType::PORTAL) return true;
    return false;
}
