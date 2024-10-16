#include "playground.h"

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
    for (int i = 0; i < width_ * height_; ++i) {
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

void Playground::__ArrangeFieldElements()
{
    // Отрисовка пустого поля
    draw::Field(field_, width_, true);

    // Расстановка остальных элементов
    MouseInput mouseInput;
    bool isBoundary, isCorner, isAdjacentToCorner;
    int cellIndex;
    bool needToRemoveAlert = false;
    while (true) {
        // Удаление предупреждения
        if (needToRemoveAlert) {
            needToRemoveAlert = false;
            // todo: remove alert...
        }

        // Расстановка
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
                    isBoundary,
                    isCorner,
                    isAdjacentToCorner
                );

                if (isCorner) continue;
                if (isBoundary) {
                    CellType typeToChangeTo = CellType::UNKNOWN;
                    if (mouseInput.buttonPressed == ButtonPressed::LEFT && field_[cellIndex].type != CellType::PORTAL) {
                        field_[cellIndex].type = CellType::PORTAL;
                        field_[cellIndex].UpdateColor();
                        draw::GameCell(field_[cellIndex]);
                        typeToChangeTo = CellType::PORTAL;
                    }
                    else if (mouseInput.buttonPressed == ButtonPressed::RIGHT && field_[cellIndex].type != CellType::WALL) {
                        field_[cellIndex].type = CellType::WALL;
                        field_[cellIndex].UpdateColor();
                        draw::GameCell(field_[cellIndex]);
                        typeToChangeTo = CellType::WALL;
                    }

                    if (typeToChangeTo != CellType::UNKNOWN) {
                        int oppositeCellIndex = getOppositeBoundaryCellIndex(clickedX - indentX_, clickedY - indentY_, width_, height_);
                        field_[oppositeCellIndex].type = typeToChangeTo;
                        field_[oppositeCellIndex].UpdateColor();
                        draw::GameCell(field_[oppositeCellIndex]);

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
                                field_[cornerCellIndex].type = CellType::PORTAL;
                            }
                            else {
                                field_[cornerCellIndex].type = CellType::WALL;
                            }
                            field_[cornerCellIndex].UpdateColor();
                            draw::GameCell(field_[cornerCellIndex]);

                            getPairedAdjacentCellAndCornerCellIndex(
                                pairedAdjacentCellIndex,
                                cornerCellIndex,
                                oppositeCellIndex % width_,
                                oppositeCellIndex / width_,
                                width_,
                                height_
                            );
                            if (field_[oppositeCellIndex].type == CellType::PORTAL && field_[pairedAdjacentCellIndex].type == CellType::PORTAL) {
                                field_[cornerCellIndex].type = CellType::PORTAL;
                            }
                            else {
                                field_[cornerCellIndex].type = CellType::WALL;
                            }
                            field_[cornerCellIndex].UpdateColor();
                            draw::GameCell(field_[cornerCellIndex]);
                        }
                    }
                }

                else {  // if cell is inside field
                    if (mouseInput.buttonPressed == ButtonPressed::LEFT) field_[cellIndex].type = CellType::WALL;
                    else if (mouseInput.buttonPressed == ButtonPressed::RIGHT) field_[cellIndex].type = CellType::PASS;
                    else if (mouseInput.buttonPressed == ButtonPressed::CTRL_LEFT) field_[cellIndex].type = CellType::SNAKE_BODY;
                    else if (mouseInput.buttonPressed == ButtonPressed::CTRL_RIGHT) {
                        if (field_[cellIndex].type == CellType::SNAKE_BODY)
                            field_[cellIndex].type = CellType::SNAKE_HEAD;
                    }
                    field_[cellIndex].UpdateColor();
                    draw::GameCell(field_[cellIndex]);
                }
            }
        }

        // Валидация
        // 1) количество змеек и корректность; SnakeSingularityAndCorrectness
        // 2) наличие закрытых пространств; ClosedSpacesExistence
        // 3) определение головы змейки; SnakeHeadIdentification
        // 4) удаление порталов, которые закрыты стеной хотя бы с одной стороны;
        //      RemovePointlessPortalsIfThereAreAny
        if (!validation.SnakeSingularityAndCorrectness(field_, width_)) {
            if (validation.snakesAmount != 1) {
                // multiple or none snakes alert
                needToRemoveAlert = true;
                continue;
            }
            else if (!validation.snakeIsCorrect) {
                // incorrect snake alert
                needToRemoveAlert = true;
                continue;
            }
        }
        if (!validation.ClosedSpacesExistence(field_, width_)) {
            if (validation.noSpaceAtAll) {
                // no playing space alert
                needToRemoveAlert = true;
                continue;
            }
            else {
                // closed spaces alert
                needToRemoveAlert = true;
                continue;
            }
        }

        // Определение головы и удаление бессмысленных порталов


        break;

        // Инициализация объекта playground


    }

//    _getch();
    exit(0);
}

void Playground::__IdentifySnakeHead()
{

}

void Playground::__DeletePointlessProtals()
{

}
