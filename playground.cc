#include "playground.h"

#include <stdexcept>
#include <cassert>
#include <thread>
#include <algorithm>

using namespace std;


void Playground::__Init()
{
    width_ = 0;
    height_ = 0;
    gameOn_ = true;
    foodIndex_ = -1;
    borderPortalIndexes_.clear();
    fieldPortalIndexes_.clear();
    captionFieldDimensions.SetIndents(TAB_WIDTH, 3);
}

void Playground::__InitializeFieldFromDimensions()
{
    int maxSpaceBetween = (Console::s_dimensions.width % 2 == 0) ? 3 : 2;
    int spaceBetween = min((Console::s_dimensions.width - width_ * 2 - captionFieldElements.Width()) / 3, maxSpaceBetween);
    indentX_ = (Console::s_dimensions.width - width_ * 2 - captionFieldElements.Width() - spaceBetween) / 2;
    indentY_ = (Console::s_dimensions.height - height_) / 2;
    captionFieldElements.SetIndents(indentX_ + width_ * 2 + spaceBetween, indentY_);

    // Putting walls around perimeter
    field_.resize(width_ * height_);
    CellType cellType;
    for (int i = 0; i < field_.size(); ++i) {
        cellType = CellType::PASS;
        if (i % width_ == 0 || i % width_ == width_ - 1 || i / width_ == 0 || i / width_ == height_ - 1) cellType = CellType::WALL;
        field_[i] = Cell(i, width_, indentX_, indentY_, cellType);
    }
}

bool Playground::FieldParametersInputForm()
{
    __Init();

    // Enter size of playing field
    __EnterFieldDimensions();
    if (width_ == -1) return false;
    __InitializeFieldFromDimensions();

    // Arrangement of walls, portals and snake itself
    __ArrangeFieldElements();
    if (width_ == -1) return false;

    return true;
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
    victory_ = false;
    foodIndex_ = -1;
    snakeHeadIndex_ = initialSnakeHeadIndex_;
    snakeAssIndex_ = initialSnakeAssIndex_;

    // recording the game
    firstFoodIndex_ = -1;
    lastFoodIndex_ = -1;
    headAndFoodIndexes_.clear();
    averageMovesToFood_ = 0;
    crashDirection_ = Direction::NONE;
}

void Playground::CalculateNextIteration()
{
    if (foodIndex_ == -1) {
        foodIndex_ = currentPassCells_[randomUnder(currentPassCells_.size())];
        field_[foodIndex_].type = CellType::FOOD;
        firstFoodIndex_ = foodIndex_;
    }

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
//    file << "\n\n\n";
//    int cellIndex, x, y;
//    int w = 7;
//    for (int i = 1; i < height_ - 1; ++i) {
//        file << "\n\n";
//        for (int j = 1; j < width_ - 1; ++j) {
//            ostringstream foss;
//            cellIndex = i * width_ + j;
//            for (auto node : nodes_[cellIndex]) {
//                x = node % width_; y = node / width_;
//                if (cellIndex - 1 == node || x - j > 1) foss << 'l';
//                else if (cellIndex + 1 == node || j - x > 1) foss << 'r';
//                else if (cellIndex - width_ == node || y - i > 1) foss << 'u';
//                else if (cellIndex + width_ == node || i - y > 1) foss << 'd';
//            }
//            if (!nodes_[cellIndex].size()) foss << "...";
//            file << setw(w) << foss.str();
//        }
//    }
//    file << "\n";
//    for (int i = 1; i < height_ - 1; ++i) {
//        file << "\n\n";
//        for (int j = 1; j < width_ - 1; ++j) {
//            cellIndex = i * width_ + j;
//            auto it = find(currentPassCells_.begin(), currentPassCells_.end(), cellIndex);
//            if (field_[cellIndex].type == CellType::WALL) file << setw(w) << "wwww";
//            else if (field_[cellIndex].type == CellType::PORTAL) file << setw(w) << "wppw";
//            else if (field_[cellIndex].type == CellType::FOOD) {
//                if (it == currentPassCells_.end()) file << setw(w) << "!ff!";
//                else file << setw(w) << " ff ";
//            }
//            else if (field_[cellIndex].type == CellType::SNAKE_BODY) file << setw(w) << " ss ";
//            else if (field_[cellIndex].type == CellType::SNAKE_HEAD) file << setw(w) << " sh ";
//            else if (it == currentPassCells_.end()) file << setw(w) << "(**)";
//            else file << setw(w) << "(  )";
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

    // collision
    int portalExitIndex;
    if (field_[nextCellIndex].type == CellType::PORTAL) {
        portalExitIndex = __GetPortalExitIndex(nextCellIndex, nextDirection);
    }
    if (field_[nextCellIndex].type == CellType::WALL ||
        field_[nextCellIndex].type == CellType::SNAKE_BODY && nextCellIndex != snakeAssIndex_ ||
        field_[nextCellIndex].type == CellType::PORTAL && portalExitIndex == -1)
    {
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
//            draw::Field(field_, width_);
//            for (int i = 1; i < shortestPathToFood.size() - 1; ++i) draw::GameCell(field_[shortestPathToFood[i]], Color::BEIGE_ON_BLUE);
            setColor(Color::NORMAL);
//            _getch();
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
        lastFoodIndex_ = foodIndex_;
        if (currentPassCells_.size()) {
            foodIndex_ = currentPassCells_[randomUnder(currentPassCells_.size())];
            field_[foodIndex_].type = CellType::FOOD;
            headAndFoodIndexes_.push_back(foodIndex_);
        }

        if (snakeTurns_.size() == 1) {  // snake length becomes 2
            iter = find(nodes_[nextCellIndex].begin(), nodes_[nextCellIndex].end(), snakeHeadIndex_);
            nodes_[nextCellIndex].erase(iter);  // so that it couldn't take a 180 degree turn and continue from its ass
        }
    }
    else {  // field_[nextCellIndex].type == CellType::PASS || nextCellIndex == snakeAssIndex_
        int nextAssIndex = __FindCellFromMovementDirection(snakeAssIndex_, snakeTurns_.front());
        snakeTurns_.pop();

        if (snakeAssIndex_ != nextCellIndex) {  // head does not go to previous ass spot
            field_[snakeAssIndex_].type = CellType::PASS;
            currentPassCells_.push_back(snakeAssIndex_);
        }
        nodes_[nextAssIndex].clear();
        for (auto& index : __GetCellVicinityByIndexes(nextAssIndex)) {
            if (field_[index].type == CellType::PASS ||
                field_[index].type == CellType::FOOD ||
                field_[index].type == CellType::SNAKE_HEAD && snakeTurns_.size() > 1)  // ... && snake length is at least 3
            {
                iter = find(nodes_[nextAssIndex].begin(), nodes_[nextAssIndex].end(), index);
                if (iter == nodes_[nextAssIndex].end()) nodes_[nextAssIndex].push_back(index);

                iter = find(nodes_[index].begin(), nodes_[index].end(), nextAssIndex);
                if (iter == nodes_[index].end()) nodes_[index].push_back(nextAssIndex);
            }
        }
        if (snakeTurns_.size()) {
            int newSecondLastIndex = __FindCellFromMovementDirection(nextAssIndex, snakeTurns_.front());
            iter = find(nodes_[nextAssIndex].begin(), nodes_[nextAssIndex].end(), newSecondLastIndex);
            if (iter == nodes_[nextAssIndex].end()) nodes_[nextAssIndex].push_back(newSecondLastIndex);
        }

        snakeAssIndex_ = nextAssIndex;
    }

    currentDirection_ = nextDirection;
    snakeHeadIndex_ = nextCellIndex;

////     отрисовка поля и пути к еде
//    if (shortestPathToFood.size()) {
//        draw::Field(field_, width_);
//        for (int i = 1; i < shortestPathToFood.size() - 1; ++i) draw::GameCell(field_[shortestPathToFood[i]], Color::BEIGE_ON_BLUE);
//        _getch();
//    }

    if (!currentPassCells_.size()) {
        gameOn_ = false;
        victory_ = true;
    }
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

    // snakeTurnsStacked - stack with the first element to take being the direction from snake last element to second last
    FileHandler::SaveInitialData(
        width_, height_,
        indentX_, indentY_,
        validation.startingDirection,
        snakeLength,
        currentPassCells_.size() + snakeLength,
        snakeTurnsStacked,
        field_
    );
}

void Playground::SaveLastGame()
{
    FileHandler::SaveGame(
        snakeTurns_.size() + 1,
        firstFoodIndex_,
        lastFoodIndex_,
        crashDirection_,
        headAndFoodIndexes_,
        width_
    );
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

void Playground::__ArrangeFieldElements()
{
    // Draw empty field
    system("cls");
    draw::Field(field_, width_, noLock, true);
    captionFieldElements.Draw();

    bool isBorder, isCorner, isAdjacentToCorner;
    int cellIndex;
    bool needToRemoveAlert = false;
    while (true) {

        // Arrangement of remaining elements
        while (true) {
            MouseInput::GetClickInfo();
            int clickedX = (MouseInput::X - indentX_) / 2;
            int clickedY = MouseInput::Y - indentY_;

            if (MouseInput::buttonPressed == ButtonPressed::ESCAPE) {
                width_ = -1;
                setColor(Color::NORMAL);
                alert.RemovePreviousAlert();
                return;
            }

            if (MouseInput::buttonPressed == ButtonPressed::WHEEL ||
                MouseInput::X >= Console::s_dimensions.width - 2 && MouseInput::Y == Console::s_dimensions.height - 1)  // cheatcode to continue
            {
                setColor(Color::NORMAL);
                alert.RemovePreviousAlert();
                break;
            }

            if (clickedX >= 0 && clickedX <= width_ - 1 &&
                clickedY >= 0 && clickedY <= height_ - 1)
            {
                cellIndex = explainClickInfo(
                    clickedX,
                    clickedY,
                    width_,
                    height_,
                    isBorder,
                    isCorner,
                    isAdjacentToCorner
                );

                if (isCorner) continue;
                if (isBorder) {
                    CellType typeToChangeTo = CellType::UNKNOWN;
                    if (MouseInput::buttonPressed == ButtonPressed::LEFT_BUTTON && field_[cellIndex].type != CellType::PORTAL) {
                        typeToChangeTo = CellType::PORTAL;
                        field_[cellIndex].type = CellType::PORTAL;
                        draw::GameCell(field_[cellIndex], rmtx);
                    }
                    else if (MouseInput::buttonPressed == ButtonPressed::RIGHT_BUTTON && field_[cellIndex].type != CellType::WALL) {
                        typeToChangeTo = CellType::WALL;
                        field_[cellIndex].type = CellType::WALL;
                        draw::GameCell(field_[cellIndex], rmtx);
                    }

                    if (typeToChangeTo != CellType::UNKNOWN) {
                        int oppositeCellIndex = getOppositeBorderCellIndex(clickedX, clickedY, width_, height_);

                        field_[oppositeCellIndex].type = typeToChangeTo;
                        draw::GameCell(field_[oppositeCellIndex], rmtx);

                        if (isAdjacentToCorner) {  // updating corner
                            int pairedAdjacentCellIndex, cornerCellIndex;
                            getPairedAdjacentCellAndCornerCellIndex(
                                pairedAdjacentCellIndex,
                                cornerCellIndex,
                                clickedX,
                                clickedY,
                                width_,
                                height_
                            );
                            if (field_[cellIndex].type == CellType::PORTAL && field_[pairedAdjacentCellIndex].type == CellType::PORTAL) {
                                field_[cornerCellIndex].type = CellType::PORTAL;
                                draw::GameCell(field_[cornerCellIndex], rmtx);
                            }
                            else {
                                field_[cornerCellIndex].type = CellType::WALL;
                                draw::GameCell(field_[cornerCellIndex], rmtx);
                            }

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
                                draw::GameCell(field_[cornerCellIndex], rmtx);
                            }
                            else {
                                field_[cornerCellIndex].type = CellType::WALL;
                                draw::GameCell(field_[cornerCellIndex], rmtx);
                            }
                        }
                    }
                }

                else {  // if cell is inside field
                    if (MouseInput::buttonPressed == ButtonPressed::LEFT_BUTTON) {
                        field_[cellIndex].type = CellType::WALL;
                        draw::GameCell(field_[cellIndex], rmtx);
                    }
                    else if (MouseInput::buttonPressed == ButtonPressed::RIGHT_BUTTON) {

                        field_[cellIndex].type = CellType::PASS;
                        draw::GameCell(field_[cellIndex], rmtx);
                    }
                    else if (MouseInput::buttonPressed == ButtonPressed::CTRL_LEFT) {
                        field_[cellIndex].type = CellType::SNAKE_BODY;
                        draw::GameCell(field_[cellIndex], rmtx);
                    }
                    else if (MouseInput::buttonPressed == ButtonPressed::CTRL_RIGHT) {
                        field_[cellIndex].type = CellType::SNAKE_HEAD;
                        draw::GameCell(field_[cellIndex], rmtx);
                    }
                }
            }
        }

        // else alert thread was interrupted by another alert thread, sets haven't been cleaned up, portal adjustment is not needed
        if (borderPortalIndexes_.empty()) {
            __GetBorderPortals();
            __AdjustPortals();
        }

        // Validation
        if (!validation.SnakeSingularityAndCorrectness(field_, width_)) {
            if (validation.snakesAmount != 1) {
                if (!validation.snakesAmount) __ShowAlert(AlertType::MULTIPLE_SNAKES);
                else __ShowAlert(AlertType::NO_SNAKES);
                continue;
            }
            else if (!validation.snakeIsCorrect) {
                __ShowAlert(AlertType::INCORRECT_SNAKE);
                continue;
            }
            else if (validation.snakeIsLooped) {
                __ShowAlert(AlertType::LOOPED_SNAKE);
                continue;
            }
        }
        if (!validation.ClosedSpacesExistence(field_, width_)) {
            __ShowAlert(AlertType::CLOSED_SPACES);
            continue;
        }
        if (!validation.SnakeHeadIdentification(field_, width_)) {
            __ShowAlert(AlertType::NO_POSSIBLE_START);
            continue;
        }

        __RepaintSnakeCells();

        // Initialization of playground object
        __InitializePlayground();

        break;
    }
}

void Playground::__EnterFieldDimensions()
{
    assert(width_ == 0 && height_ == 0);
    int maxFieldWidth = min(91, (Console::s_dimensions.width - captionFieldElements.Width()) / 2 - 2);
    int maxFieldHeight = min(91, Console::s_dimensions.height - 2);

    string phraseChooseWidth = "Выбери ширину поля (3 - " + to_string(maxFieldWidth) + ") => ";
    string phraseChooseHeight = "Выбери высоту поля (3 - " + to_string(maxFieldHeight) + ") => ";
    string input;
    int number;
    bool bChooseWidth = true;
    do {
        if (!width_ && !height_) system("cls");
        if (bChooseWidth) {
            if (!width_ && !height_) {
                cout << "\n\t" << phraseChooseWidth;
                captionFieldDimensions.Draw();
                setPosition(TAB_WIDTH + phraseChooseWidth.size(), 1);
            }
            getline(cin, input);
            if (canConvertToNumber(input)) {
                number = stoi(input);
                if (number >= 3 && number <= maxFieldWidth) {
                    width_ = number;
                    bChooseWidth = false;
                    if (!height_) {
                        cout << "\t" << phraseChooseHeight;
                        captionFieldDimensions.Clear();
                        captionFieldDimensions.SetIndents(TAB_WIDTH, 4);
                        captionFieldDimensions.Draw();
                        setPosition(8 + phraseChooseHeight.size(), 2);
                    }
                }
                else {
                    cout << '\a';
                    draw::ClearInputAndMoveCursorBack(TAB_WIDTH + phraseChooseWidth.length(), input.length());
                }
            }
            else {
                if (input.empty()) {
                    if (height_) setPosition(TAB_WIDTH + phraseChooseWidth.length(), 2);
                    else bChooseWidth = false;
                }
                else if (tolower(input[0]) == 'r') {
                    width_ = 0;
                    height_ = 0;
                    bChooseWidth = true;
                    captionFieldDimensions.SetIndents(TAB_WIDTH, 3);
                }
                else if (tolower(input[0] == 'e')) {
                    width_ = -1;
                    return;
                }
                else draw::ClearInputAndMoveCursorBack(TAB_WIDTH + phraseChooseWidth.length(), input.length());
            }
        }
        else {
            if (!width_ && !height_) {
                cout << "\n\t" << phraseChooseHeight;
                captionFieldDimensions.Draw();
                setPosition(TAB_WIDTH + phraseChooseHeight.size(), 1);
            }
            getline(cin, input);
            if (canConvertToNumber(input)) {
                number = stoi(input);
                if (number >= 3 && number <= maxFieldHeight) {
                    height_ = number;
                    bChooseWidth = true;
                    if (!width_) {
                        cout << "\t" << phraseChooseWidth;
                        captionFieldDimensions.Clear();
                        captionFieldDimensions.SetIndents(TAB_WIDTH, 4);
                        captionFieldDimensions.Draw();
                        setPosition(8 + phraseChooseWidth.size(), 2);
                    }
                }
                else {
                    cout << '\a';
                    draw::ClearInputAndMoveCursorBack(TAB_WIDTH + phraseChooseHeight.length(), input.length());
                }
            }
            else {
                if (input.empty()) {
                    if (width_) setPosition(TAB_WIDTH + phraseChooseHeight.length(), 2);
                    else bChooseWidth = true;
                }
                else if (tolower(input[0]) == 'r') {
                    width_ = 0;
                    height_ = 0;
                    bChooseWidth = true;
                    captionFieldDimensions.SetIndents(TAB_WIDTH, 3);
                }
                else if (tolower(input[0] == 'e')) {
                    width_ = -1;
                    return;
                }
                else draw::ClearInputAndMoveCursorBack(TAB_WIDTH + phraseChooseHeight.length(), input.length());
            }
        }
    } while (!width_ || !height_);
    width_ += 2;
    height_ += 2;
}

void Playground::EnterGamesAmount()
{
    int gamesLimit = 1000;
    string phrase = "Введи количество игр (1 - " + to_string(gamesLimit) + ") => ";
    string input;
    int number;

    system("cls");
    cout << "\n\t" << phrase;
    do {
        getline(cin, input);
        if (canConvertToNumber(input)) {
            number = stoi(input);
            if (number >= 1 && number <= gamesLimit) {
                gamesAmount = number;
                return;
            }
            cout << '\a';
            draw::ClearInputAndMoveCursorBack(TAB_WIDTH + phrase.length(), input.length());
        }
        else {
            if (input.empty()) setPosition(TAB_WIDTH + phrase.length(), 1);
            else draw::ClearInputAndMoveCursorBack(TAB_WIDTH + phrase.length(), input.length());
        }
    } while (true);
}

void Playground::__InitializePlayground()
{
    // Adjacency list
    nodes_.resize(field_.size());
    for (int i = 0; i < nodes_.size(); ++i) {
        nodes_[i] = {};
        nodes_[i].reserve(4);
    }
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
    int spaceIndex;
    while (true) {
        spaceIndex = -1;
        for (int i = 0; i < field_.size(); ++i) {
            if (field_[i].type == CellType::PASS &&
                find(currentPassCells_.begin(), currentPassCells_.end(), i) == currentPassCells_.end())
            {
                spaceIndex = i;
                break;
            }
        }
        if (spaceIndex == -1) return;

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

void Playground::__GetBorderPortals()
{
    if (!borderPortalIndexes_.empty()) return;  // alert thread was interrupted by another alert thread, sets haven't been cleaned up
    for (int i = 0; i < field_.size(); ++i) {
        if ((i % width_ != 0) && (i % width_ != width_ - 1) && (i / width_ != 0) && (i / width_ != height_ - 1)) continue;
        if (field_[i].type == CellType::PORTAL) borderPortalIndexes_.insert(i);
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
        field_[upperLeftCorner].type = CellType::WALL;
        draw::GameCell(field_[upperLeftCorner], rmtx);
    }
    if (field_[upperRightAdj1].type != CellType::PORTAL || field_[upperRightAdj2].type != CellType::PORTAL) {
        field_[upperRightCorner].type = CellType::WALL;
        draw::GameCell(field_[upperRightCorner], rmtx);
    }
    if (field_[lowerLeftAdj1].type != CellType::PORTAL || field_[lowerLeftAdj2].type != CellType::PORTAL) {
        field_[lowerLeftCorner].type = CellType::WALL;
        draw::GameCell(field_[lowerLeftCorner], rmtx);
    }
    if (field_[lowerRightAdj1].type != CellType::PORTAL || field_[lowerRightAdj2].type != CellType::PORTAL) {
        field_[lowerRightCorner].type = CellType::WALL;
        draw::GameCell(field_[lowerRightCorner], rmtx);
    }
}

void Playground::__CalculatePortalEntries(Orientation orientation, int axisValue)
{
    int index, possiblePortalIndex, nextCellIndex;
    if (orientation == Orientation::VERTICAL) {
        assert(axisValue >= 1 && axisValue < width_ - 1);
        const int X = axisValue;

        if (__WholeAxisIsAWall(orientation, axisValue)) {
            field_[X].type = CellType::WALL;
            draw::GameCell(field_[X], rmtx);
            index = (height_ - 1) * width_ + X;
            field_[index].type = CellType::WALL;
            draw::GameCell(field_[index], rmtx);
            return;
        }

        int y = 0;
        possiblePortalIndex = y * width_ + X;
        while (true) {
            if (++y >= height_) {
                throw runtime_error("Unable to calculate portal entry.");
            }
            nextCellIndex = y * width_ + X;
            if (field_[nextCellIndex].type != CellType::WALL) {
                if (y != 1) {
                    fieldPortalIndexes_.insert(possiblePortalIndex);
                    field_[possiblePortalIndex].type = CellType::PORTAL;
                    draw::GameCell(field_[possiblePortalIndex], rmtx);
                }
                break;
            }
            else {
                if (y == 1) {
                    field_[possiblePortalIndex].type = CellType::WALL;
                    draw::GameCell(field_[possiblePortalIndex], rmtx);
                }
                possiblePortalIndex = nextCellIndex;
            }
        }

        y = height_ - 1;
        possiblePortalIndex = y * width_ + X;
        while (true) {
            if (--y < 0) {
                throw runtime_error("Unable to calculate portal entry.");
            }
            nextCellIndex = y * width_ + X;
            if (field_[nextCellIndex].type != CellType::WALL) {
                if (y != height_ - 2) {
                    fieldPortalIndexes_.insert(possiblePortalIndex);
                    field_[possiblePortalIndex].type = CellType::PORTAL;
                    draw::GameCell(field_[possiblePortalIndex], rmtx);
                }
                break;
            }
            else {
                if (y == height_ - 2) {
                    field_[possiblePortalIndex].type = CellType::WALL;
                    draw::GameCell(field_[possiblePortalIndex], rmtx);
                }
                possiblePortalIndex = nextCellIndex;
            }
        }
    }
    else {  // orientation == Orientation::HORIZONTAL
        assert(axisValue >= 1 && axisValue < height_ - 1);
        const int Y = axisValue;

        if (__WholeAxisIsAWall(orientation, axisValue)) {
            index = Y * width_;
            field_[index].type = CellType::WALL;
            draw::GameCell(field_[index], rmtx);
            index = Y * width_ + (width_ - 1);
            field_[index].type = CellType::WALL;
            draw::GameCell(field_[index], rmtx);
            return;
        }

        int x = 0;
        possiblePortalIndex = Y * width_ + x;
        while (true) {
            if (++x >= width_) {
                throw runtime_error("Unable to calculate portal entry.");
            }
            nextCellIndex = Y * width_ + x;
            if (field_[nextCellIndex].type != CellType::WALL && field_[nextCellIndex].type != CellType::PORTAL) {
                if (x != 1) {
                    fieldPortalIndexes_.insert(possiblePortalIndex);
                    field_[possiblePortalIndex].type = CellType::PORTAL;
                    draw::GameCell(field_[possiblePortalIndex], rmtx);
                }
                break;
            }
            else {
                if (x == 1) {
                    field_[possiblePortalIndex].type = CellType::WALL;
                    draw::GameCell(field_[possiblePortalIndex], rmtx);
                }
                possiblePortalIndex = nextCellIndex;
            }
        }

        x = width_ - 1;
        possiblePortalIndex = Y * width_ + x;
        while (true) {
            if (--x < 0) {
                throw runtime_error("Unable to calculate portal entry.");
            }
            nextCellIndex = Y * width_ + x;
            if (field_[nextCellIndex].type != CellType::WALL && field_[nextCellIndex].type != CellType::PORTAL) {
                if (x != width_ - 2) {
                    fieldPortalIndexes_.insert(possiblePortalIndex);
                    field_[possiblePortalIndex].type = CellType::PORTAL;
                    draw::GameCell(field_[possiblePortalIndex], rmtx);
                }
                break;
            }
            else {
                if (x == width_ - 2) {
                    field_[possiblePortalIndex].type = CellType::WALL;
                    draw::GameCell(field_[possiblePortalIndex], rmtx);
                }
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
    for (auto it = borderPortalIndexes_.begin(); it != borderPortalIndexes_.end(); ++it) {
        if (field_[*it].type == CellType::WALL) {
            field_[*it].type = CellType::PORTAL;
            draw::GameCell(field_[*it], rmtx);
        }
    }
    for (auto it = fieldPortalIndexes_.begin(); it != fieldPortalIndexes_.end(); ++it) {
        field_[*it].type = CellType::WALL;
        draw::GameCell(field_[*it], rmtx);
    }
    borderPortalIndexes_.clear();
    fieldPortalIndexes_.clear();
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

        if (cellIndex == validation.startingCellIndex) {
            field_[cellIndex].type = CellType::SNAKE_HEAD;
            draw::GameCell(field_[cellIndex], rmtx);
        }
        else {
            field_[cellIndex].type = CellType::SNAKE_BODY;
            draw::GameCell(field_[cellIndex], rmtx);
        }

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
