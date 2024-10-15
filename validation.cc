#include "validation.h"

#include <unordered_set>

using namespace std;


Validation::Validation()
{

}

bool Validation::SnakeIsGood(const vector<Cell>& field, int fieldWidth)
{
    int fieldHeight = field.size() / fieldWidth;
    snakesAmount = 0;

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

    // todo: расписать случаи отсутвия змейки и нескольких змеек; добавить интерфейс Alert
    return false;
}

bool Validation::__IsASnake(Cell cell)
{
    if (cell.type == CellType::SNAKE_BODY || cell.type == CellType::SNAKE_HEAD) return true;
    return false;
}

void Validation::__GetInfoAboutWholeSnake(std::vector<std::vector<bool>>& fieldInfo, int snakeIndex, const std::vector<Cell>& field)
{
    int fieldWidth = fieldInfo[0].size();
    int fieldHeight = fieldInfo.size();
    queue<int> traversalOrder;
    traversalOrder.push(snakeIndex);
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
        if (x - 1 >= 0 && !fieldInfo[x - 1][y] && __IsASnake(field[leftCellIndex])) traversalOrder.push(leftCellIndex);
        if (x + 1 < fieldWidth && !fieldInfo[x + 1][y] && __IsASnake(field[rightCellIndex])) traversalOrder.push(rightCellIndex);
        if (y - 1 >= 0 && !fieldInfo[x][y - 1] && __IsASnake(field[topCellIndex])) traversalOrder.push(topCellIndex);
        if (y + 1 < fieldHeight && !fieldInfo[x][y + 1] && __IsASnake(field[bottomCellIndex])) traversalOrder.push(bottomCellIndex);
    }
}

bool Validation::NoClosedSpaces(const std::vector<Cell>& field, int fieldWidth)
{
    int fieldHeight = field.size() / fieldWidth;
    queue<int> traversalOrder;
    int index;
    for (int i = 0; i < )
}






