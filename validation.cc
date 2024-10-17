#include "validation.h"

#include <unordered_set>
#include <queue>
#include <cassert>

using namespace std;


bool Validation::SnakeSingularityAndCorrectness(const vector<Cell>& field, int fieldWidth)
{
    int fieldHeight = field.size() / fieldWidth;
    snakesAmount = 0;
    snakeIsCorrect = true;
    snakeIsLooped = true;

    // 0 - not passed; 1 - passed
    vector<vector<bool>> fieldInfo(fieldHeight, vector<bool>(fieldWidth));
    int index;
    for (int y = 0; y < fieldHeight; ++y) {
        for (int x = 0; x < fieldWidth; ++x) {
            if (fieldInfo[x][y]) continue;

            index = y * fieldWidth + x;
            if (__IsASnake(field[index])) {
                __GetInfoAboutWholeSnake(fieldInfo, index, field);
                snakesAmount++;
            }
            else fieldInfo[x][y] = 1;
        }
    }

    if (snakesAmount == 1 && snakeIsCorrect && !snakeIsLooped) return true;
    return false;
}

void Validation::__GetInfoAboutWholeSnake(vector<vector<bool>>& fieldInfo, int snakeIndex, const vector<Cell>& field)
{
    int fieldWidth = fieldInfo[0].size();
    int fieldHeight = fieldInfo.size();
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

        fieldInfo[x][y] = 1;
        leftCellIndex = y * fieldWidth + x - 1;
        rightCellIndex = y * fieldWidth + x + 1;
        topCellIndex = (y - 1) * fieldWidth + x;
        bottomCellIndex = (y + 1) * fieldWidth + x;

        neighboursAmount = 0;
        if (x - 1 >= 0 && __IsASnake(field[leftCellIndex])) {
            neighboursAmount++;
            if (!fieldInfo[x - 1][y]) traversalOrder.push(leftCellIndex);
        }
        if (x + 1 < fieldWidth && __IsASnake(field[rightCellIndex])) {
            neighboursAmount++;
            if (!fieldInfo[x + 1][y]) traversalOrder.push(rightCellIndex);
        }
        if (y - 1 >= 0 && __IsASnake(field[topCellIndex])) {
            neighboursAmount++;
            if (!fieldInfo[x][y - 1]) traversalOrder.push(topCellIndex);
        }
        if (y + 1 < fieldHeight && __IsASnake(field[bottomCellIndex])) {
            neighboursAmount++;
            if (!fieldInfo[x][y + 1]) traversalOrder.push(bottomCellIndex);
        }

        if (neighboursAmount > 2) snakeIsCorrect = false;
        if (neighboursAmount <= 1) snakeIsLooped = false;
    }
}

bool Validation::ClosedSpacesExistence(const vector<Cell>& field, int fieldWidth)
{
    noSpaceAtAll = false;
    int fieldHeight = field.size() / fieldWidth;
    queue<int> traversalOrder;
    vector<vector<bool>> fieldInfo(fieldHeight, vector<bool>(fieldWidth));

    int index = __GetFirstSpaceIndex(field, fieldInfo);
    if (index == -1) {
        noSpaceAtAll = true;
        return false;
    }
    traversalOrder.push(index);

    int cellIndex, x, y;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;
    while (!traversalOrder.empty()) {
        cellIndex = traversalOrder.front();
        traversalOrder.pop();
        x = cellIndex % fieldWidth;
        y = cellIndex / fieldWidth;

        fieldInfo[x][y] = 1;
        leftCellIndex = y * fieldWidth + x - 1;
        rightCellIndex = y * fieldWidth + x + 1;
        topCellIndex = (y - 1) * fieldWidth + x;
        bottomCellIndex = (y + 1) * fieldWidth + x;

        if (field[leftCellIndex].type == CellType::PORTAL) {
            for (int xx = 2; xx < fieldWidth; ++xx) {
                cellIndex = y * fieldWidth + xx;
                if (field[cellIndex].type == CellType::PORTAL) break;
            }
            leftCellIndex = cellIndex - 1;
            x = leftCellIndex % fieldWidth;
        }
        if (field[rightCellIndex].type == CellType::PORTAL) {
            for (int xx = fieldWidth - 3; xx >= 0; --xx) {
                cellIndex = y * fieldWidth + xx;
                x = xx - 1;
                if (field[cellIndex].type == CellType::PORTAL) break;
            }
            rightCellIndex = cellIndex + 1;
            x = rightCellIndex % fieldWidth;
        }
        if (field[topCellIndex].type == CellType::PORTAL) {
            for (int yy = 2; yy < fieldHeight; ++yy) {
                cellIndex = yy * fieldWidth + x;
                if (field[cellIndex].type == CellType::PORTAL) break;
            }
            topCellIndex = cellIndex - fieldWidth;
            y = topCellIndex / fieldWidth;
        }
        if (field[bottomCellIndex].type == CellType::PORTAL) {
            for (int yy = fieldHeight - 3; yy >= 0; --yy) {
                cellIndex = yy * fieldWidth + x;
                if (field[cellIndex].type == CellType::PORTAL) break;
            }
            bottomCellIndex = cellIndex + fieldWidth;
            y = bottomCellIndex / fieldWidth;
        }

        if (!fieldInfo[x - 1][y] && __IsPossibleSpace(field[leftCellIndex])) traversalOrder.push(leftCellIndex);
        if (!fieldInfo[x + 1][y] && __IsPossibleSpace(field[rightCellIndex])) traversalOrder.push(rightCellIndex);
        if (!fieldInfo[x][y - 1] && __IsPossibleSpace(field[topCellIndex])) traversalOrder.push(topCellIndex);
        if (!fieldInfo[x][y + 1] && __IsPossibleSpace(field[bottomCellIndex])) traversalOrder.push(bottomCellIndex);
    }

    if (__SpaceIsLeft(field, fieldInfo)) return false;
    return true;
}

//bool Validation::SnakeHeadIdentification(const vector<Cell>& field, int fieldWidth)
//{
//    int snakeEnd1, snakeEnd2;
//    __FindSnakeEnds(field, fieldWidth, snakeEnd1, snakeEnd2);
//
//    CellType end1Left, end1Right, end1Top, end1Bottom;
//    CellType end2Left, end2Right, end2Top, end2Bottom;
//    __GetCellVicinity(field, fieldWidth, snakeEnd1, end1Left, end1Right, end1Top, end1Bottom);
//    __GetCellVicinity(field, fieldWidth, snakeEnd2, end2Left, end2Right, end2Top, end2Bottom);
//
//    unordered_set<CellType> end1Directions = {end1Left, end1Right, end1Top, end1Bottom};
//    unordered_set<CellType> end2Directions = {end2Left, end2Right, end2Top, end2Bottom};
//    if (end1Directions.count(CellType::PASS) && )
//
//    CellType end1 = field[snakeEnd1].type;
//    CellType end2 = field[snakeEnd2].type;
//
//}

void Validation::__GetCellVicinity(
    const vector<Cell>& field,
    int fieldWidth,
    int cellIndex,
    CellType& leftCellType,
    CellType& rightCellType,
    CellType& topCellType,
    CellType& bottomCellType
) {
    int fieldHeight = field.size() / fieldWidth;
    int leftCellIndex, rightCellIndex, topCellIndex, bottomCellIndex;
    int x = cellIndex % fieldWidth,
        y = cellIndex / fieldWidth;

    leftCellIndex = y * fieldWidth + x - 1;
    rightCellIndex = y * fieldWidth + x + 1;
    topCellIndex = (y - 1) * fieldWidth + x;
    bottomCellIndex = (y + 1) * fieldWidth + x;

    if (x - 1 >= 0) leftCellType = field[leftCellIndex].type;
    else leftCellType = CellType::UNKNOWN;
    if (x + 1 < fieldWidth) rightCellType = field[rightCellIndex].type;
    else rightCellType = CellType::UNKNOWN;
    if (y - 1 >= 0) topCellType = field[topCellIndex].type;
    else topCellType = CellType::UNKNOWN;
    if (y + 1 < fieldHeight) bottomCellType = field[bottomCellIndex].type;
    else bottomCellType = CellType::UNKNOWN;
}

void Validation::__FindSnakeEnds(const vector<Cell>& field, int fieldWidth, int& snakeEnd1, int& snakeEnd2)
{
    int snakeIndex = -1;
    for (int i = 0; i < field.size(); ++i) {
        if (field[i].type == CellType::SNAKE_BODY || field[i].type == CellType::SNAKE_HEAD) {
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
        if (x - 1 >= 0 && __IsASnake(field[leftCellIndex])) {
            neighboursAmount++;
            if (!passedCells.count(leftCellIndex)) traversalOrder.push(leftCellIndex);
        }
        if (x + 1 < fieldWidth && __IsASnake(field[rightCellIndex])) {
            neighboursAmount++;
            if (!passedCells.count(rightCellIndex)) traversalOrder.push(rightCellIndex);
        }
        if (y - 1 >= 0 && __IsASnake(field[topCellIndex])) {
            neighboursAmount++;
            if (!passedCells.count(topCellIndex)) traversalOrder.push(topCellIndex);
        }
        if (y + 1 < fieldHeight && __IsASnake(field[bottomCellIndex])) {
            neighboursAmount++;
            if (!passedCells.count(bottomCellIndex)) traversalOrder.push(bottomCellIndex);
        }

        if (neighboursAmount == 1) {
            if (!snakeEnd1) snakeEnd1 = cellIndex;
            else snakeEnd2 = cellIndex;
        }
    }
}

int Validation::__GetFirstSpaceIndex(const vector<Cell>& field, vector<vector<bool>>& fieldInfo)
{
    int spaceIndex = -1;
    int fieldWidth = fieldInfo[0].size();
    for (int i = 0; i < field.size(); ++i) {
        if (__IsPossibleSpace(field[i]) && !fieldInfo[i % fieldWidth][i / fieldWidth]) {
            spaceIndex = i;
            break;
        }
    }
    return spaceIndex;
}

bool Validation::__SpaceIsLeft(const vector<Cell>& field, vector<vector<bool>>& fieldInfo)
{
    int fieldWidth = fieldInfo[0].size();
    int fieldHeight = fieldInfo.size();
    int index;
    for (int y = 0; y < fieldHeight; ++y) {
        for (int x = 0; x < fieldWidth; ++x) {
            index = y * fieldWidth + x;
            if (!fieldInfo[x][y] && __IsPossibleSpace(field[index])) return true;
        }
    }
    return false;
}

bool Validation::__IsASnake(Cell cell)
{
    if (cell.type == CellType::SNAKE_BODY || cell.type == CellType::SNAKE_HEAD) return true;
    return false;
}

bool Validation::__IsPossibleSpace(Cell cell)
{
    unordered_set<CellType> possibleSpace = {
        CellType::PASS,
        CellType::SNAKE_BODY,
        CellType::SNAKE_HEAD
    };
    if (possibleSpace.count(cell.type)) return true;
    return false;
}
