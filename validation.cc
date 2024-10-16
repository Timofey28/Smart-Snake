#include "validation.h"

#include <unordered_set>

using namespace std;


bool Validation::SnakeSingularityAndCorrectness(const vector<Cell>& field, int fieldWidth)
{
    int fieldHeight = field.size() / fieldWidth;
    snakesAmount = 0;
    snakeIsCorrect = true;

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

    if (snakesAmount == 1) return true;
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
        neighboursAmount = 0;
        if (x - 1 >= 0 && !fieldInfo[x - 1][y] && __IsPossibleSpace(field[leftCellIndex])) traversalOrder.push(leftCellIndex);
        if (x + 1 < fieldWidth && !fieldInfo[x + 1][y] && __IsPossibleSpace(field[rightCellIndex])) traversalOrder.push(rightCellIndex);
        if (y - 1 >= 0 && !fieldInfo[x][y - 1] && __IsPossibleSpace(field[topCellIndex])) traversalOrder.push(topCellIndex);
        if (y + 1 < fieldHeight && !fieldInfo[x][y + 1] && __IsPossibleSpace(field[bottomCellIndex])) traversalOrder.push(bottomCellIndex);
    }

    if (__SpaceIsLeft(fieldInfo, field)) return false;
    return true;
}

bool Validation::__GetFirstSpaceIndex(const vector<Cell>& field, vector<vector<bool>>& fieldInfo)
{
    int spaceIndex = -1;
    int fieldWidth = fieldInfo[0].size();
    for (int i = 0; i < field.size(); ++i) {
        if (__IsPossibleSpace(field[i].type) && !fieldInfo[i % fieldWidth][i / fieldWidth]) {
            spaceIndex = i;
            break;
        }
    }
    return spaceIndex;
}
