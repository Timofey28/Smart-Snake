#include "playground.h"

#include <stdexcept>
#include <cassert>
#include <thread>

#include <conio.h>

using namespace std;


Playground::Playground()
{
    width_ = 0;
    height_ = 0;
}

void Playground::__InitializeFieldFromDimensions()
{
    indentX_ = (nConsoleWidth / 2 - width_) / 2;
    indentY_ = (nConsoleHeight - height_) / 2;

    // Ставим стены по периметру
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
    // Ввод размеров игрового поля
    draw::EnterFieldDimensions(width_, height_);
    __InitializeFieldFromDimensions();

    // Расположение стен, порталов и самой змейки
    __ArrangeFieldElements();
}

int Playground::GetPortalExitIndex(int portalEnterIndex)
{
    int x = portalEnterIndex % width_,
        y = portalEnterIndex / width_;
    int index;

    if (currentDirection_ == Direction::LEFT) {
        while (true) {
            if (++x >= width_) {
                throw runtime_error("Unable to get portal exit, none were found.");
            }
            index = y * width_ + x;
            if (field_[index].type == CellType::PORTAL) return index;
        }
    }
    else if (currentDirection_ == Direction::RIGHT) {
        while (true) {
            if (--x < 0) {
                throw runtime_error("Unable to get portal exit, none were found.");
            }
            index = y * width_ + x;
            if (field_[index].type == CellType::PORTAL) return index;
        }
    }
    else if (currentDirection_ == Direction::UP) {
        while (true) {
            if (++y >= height_) {
                throw runtime_error("Unable to get portal exit, none were found.");
            }
            index = y * width_ + x;
            if (field_[index].type == CellType::PORTAL) return index;
        }
    }
    else {  // direction == Direction::DOWN
        while (true) {
            if (--y < 0) {
                throw runtime_error("Unable to get portal exit, none were found.");
            }
            index = y * width_ + x;
            if (field_[index].type == CellType::PORTAL) return index;
        }
    }
}

void Playground::__ArrangeFieldElements()
{
    // Отрисовка пустого поля
    draw::Field(field_, width_, true);

    // Расстановка остальных элементов
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

        // Корректировка порталов
        __AdjustPortals();

        cout << '\a';

        // Валидация
        // 1) количество змеек и корректность; SnakeSingularityAndCorrectness
        // 2) наличие закрытых пространств; ClosedSpacesExistence
        // 3) определение головы змейки; SnakeHeadIdentification
        // 4) удаление порталов, которые закрыты стеной хотя бы с одной стороны;
        //      RemovePointlessPortalsIfThereAreAny

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
            if (validation.noSpaceAtAll) {
                draw::alert::NoPlayingSpace();
                needToRemoveAlert = true;
                continue;
            }
            else {
                draw::alert::ClosedSpaces();
                needToRemoveAlert = true;
                continue;
            }
        }


        // Идентификация головы и начального направления

        break;

        // Инициализация объекта playground


    }

//    _getch();
    exit(0);
}

void Playground::__IdentifySnakeHead()
{

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
