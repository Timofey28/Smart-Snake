#include "game.h"
using namespace std;

Game::Game(fs::path gameFilePath, int score, bool loadRightAway) :
    path_(gameFilePath),
    no(stoi(gameFilePath.stem().string())),
    score(score),
    frameChanged_(false),
    forwardPlayback(true),
    snakeLengthChanged_(false)
{
    assert(fs::exists(gameFilePath));
    if (loadRightAway) {
        FileHandler::ReadGame(
            gameFilePath,
            width_, height_,
            fieldIndentX_, fieldIndentY_,
            initialField_,
            initialSnakeTurns_,
            startingDirection_, crashDirection_,
            startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_,
            foodIndex_, lastFoodIndex_,
            initialGameIndexes_
        );
        __CalculateMovesInfo();
        __InitDimensions();
    }
    else width_ = -1;
}

void Game::Load()
{
    if (IsLoaded()) return;
    FileHandler::ReadGame(
        path_,
        width_, height_,
        fieldIndentX_, fieldIndentY_,
        initialField_,
        initialSnakeTurns_,
        startingDirection_, crashDirection_,
        startingSnakeLength_, finalSnakeLength_, maxPossibleSnakeLength_,
        foodIndex_, lastFoodIndex_,
        initialGameIndexes_
    );
    __CalculateMovesInfo();
    __InitDimensions();
}

GameScrollbar::GameScrollbar(int _frames, int _width)
{
    sliderPos = 0;
    frames_ = _frames;
    length_ = min(_frames, _width);

    int _multiFrameRange = length_ - 1,
        _steps = _frames - 1;
    int _bigFreqCount = _steps % _multiFrameRange;
    int _smallFrequency = _steps / _multiFrameRange;
    int _bigFreqStart = length_ / 2 - _bigFreqCount / 2;
    int _bigFreqEnd = _bigFreqStart + _bigFreqCount - 1;
//setPosition(50, 6);cout<<"smallFreq: "<<_smallFrequency<<"; bfStart: "<<_bigFreqStart<<"; bfEnd: "<<_bigFreqEnd;

    sliderPosToFrame_[0] = -1;
    sliderPosToFrame_[length_ - 1] = frames_ - 2;
    for (int i = 1; i < _bigFreqStart; ++i) sliderPosToFrame_[i] = sliderPosToFrame_[i - 1] + _smallFrequency;
    for (int i = _bigFreqStart; i <= _bigFreqEnd; ++i) sliderPosToFrame_[i] = sliderPosToFrame_[i - 1] + _smallFrequency + 1;
    for (int i = _bigFreqEnd + 1; i < length_ - 1; ++i) sliderPosToFrame_[i] = sliderPosToFrame_[i - 1] + _smallFrequency;

    frameToSliderPos_[frames_ - 2] = length_ - 1;
    auto it = sliderPosToFrame_.begin();
    while (true) {
        auto nextIt = next(it);
        if (nextIt == sliderPosToFrame_.end()) break;
        for (int i = it->second; i < nextIt->second; ++i) frameToSliderPos_[i] = it->first;
        it++;
    }

//    setPosition(50, 12);
//    for (it = frameToSliderPos_.begin(); it != frameToSliderPos_.end(); ++it) {
//        cout << it->first << ":" << it->second << ' ';
//    }
}

void Game::InitAndPrintInterface()
{
    assert(IsLoaded());
    field_ = initialField_;
    snakeTurns_ = initialSnakeTurns_;
    currentFrame_ = -1;  // first frame of interval [-1; n-2] where n -- number of frames
    currentFoodIndex_ = 0;
    snakeLength_ = startingSnakeLength_;
    field_[foodIndex_].type = CellType::FOOD;
    foodIndexes_ = {foodIndex_};
    gameIndexes_ = initialGameIndexes_;
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
    scrollbar_ = make_unique<GameScrollbar>(gameIndexes_.size() + 2, width_ * 2);
    currentSpeedNo.store(1, std::memory_order_relaxed);
    upperMargin_ = fieldIndentY_ - 1;

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

    int _rightPartMargin = fieldIndentX_ + leftPartWidth_ + s_spaceBetween_;
    setColor(Color::NORMAL);
    setPosition(_rightPartMargin, upperMargin_ + s_indentFrameNo);
    cout << "Кадр: " << setw(maxFrameNoLength_) << 1 << '/' << scrollbar_->Frames();
    setPosition(_rightPartMargin, upperMargin_ + s_indentSnakeLength_);
    cout << "Длина змейки: " << setw(maxSnakeNoLength_) << startingSnakeLength_ << '/' << finalSnakeLength_;
    setPosition(_rightPartMargin, upperMargin_ + s_indentApplesEaten_);
    cout << "Яблок съедено: " << setw(maxAppleNoLength_) << 0 << '/' << score;

    setPosition(_rightPartMargin, upperMargin_ + s_indentSpeed_);
    cout << "Скорость: " << setw(2) << ((double) 1000 / s_speedNoToFrameTime_[currentSpeedNo.load(std::memory_order_relaxed)]) << " кадр/с";

    TableData td(_rightPartMargin, upperMargin_ + s_indentSpeedsTable_);
    td.values = {{{1},{2},{3},{4},{5}},{{'<'},{"Пуск "},{'>'}},{{"<<"},{">>"}}};
    td.rowsAlignment = RowsHorizontalAlignment::CENTER;
    speedsTable_ = make_unique<Table<BoxStyle>>(td);
    speedsTable_->Draw();
    speedsTable_->PaintCell(0, currentSpeedNo.load(std::memory_order_relaxed) - 1, s_colorChosenSpeed_, CellSides());

    DrawScrollbar();
    WriteReproductionMode(forwardPlayback);
}

template <class Mtx>
bool Game::PrintNextFrame(Mtx& m)
{
    lock_guard<Mtx> lock(m);
    if (currentFrame_ == gameIndexes_.size()) return false;
    frameChanged_ = true;
    if (currentFrame_ == gameIndexes_.size() - 1) __DrawCrash(gameIndexes_[currentFrame_++]);
    else {
        int _prevHeadIndex = (currentFrame_ == -1) ? startingHeadIndex_ : gameIndexes_[currentFrame_];
        snakeTurns_.push_front(__FindMovementDirection(_prevHeadIndex, gameIndexes_[++currentFrame_]));
        field_[gameIndexes_[currentFrame_]].type = CellType::SNAKE_HEAD;
        draw::SnakeHead(field_[gameIndexes_[currentFrame_]], snakeTurns_[0]);
        if (snakeLength_ >= 2 || gameIndexes_[currentFrame_] == foodIndexes_[currentFoodIndex_]) {
            field_[_prevHeadIndex].type = CellType::SNAKE_BODY;
            draw::GameCell(field_[_prevHeadIndex]);
        }
        if (gameIndexes_[currentFrame_] == foodIndexes_[currentFoodIndex_]) {
            if (++snakeLength_ < maxPossibleSnakeLength_) {
                assert(currentFoodIndex_ < foodIndexes_.size() - 1);
                field_[foodIndexes_[++currentFoodIndex_]].type = CellType::FOOD;
                draw::GameCell(field_[foodIndexes_[currentFoodIndex_]]);
            }
            snakeLengthChanged_ = true;
        }
        else {
            if (gameIndexes_[currentFrame_] != snakeAssIndex_) {
                field_[snakeAssIndex_].type = CellType::PASS;
                draw::GameCell(field_[snakeAssIndex_]);
            }
            snakeAssIndex_ = __FindCellFromMovementDirection(snakeAssIndex_, snakeTurns_[snakeLength_ - 1]);
        }
    }

    CorrectScrollbarSlider();
    CorrectStateInfo();
    return true;
}
template bool Game::PrintNextFrame<NullMutex>(NullMutex&);
template bool Game::PrintNextFrame<mutex>(mutex&);

template <class Mtx>
bool Game::PrintPreviousFrame(Mtx& m)
{
    lock_guard<Mtx> lock(m);
    if (currentFrame_ == -1) return false;
    frameChanged_ = true;
    if (currentFrame_ == gameIndexes_.size()) __RemoveCrash(gameIndexes_[--currentFrame_]);
    else {
        int _prevHeadIndex = gameIndexes_[currentFrame_];
        if (--currentFrame_ == -1) {
            field_[startingHeadIndex_].type = CellType::SNAKE_HEAD;
            draw::SnakeHead(field_[startingHeadIndex_], startingDirection_);
        }
        else {
            field_[gameIndexes_[currentFrame_]].type = CellType::SNAKE_HEAD;
            if (snakeTurns_.size() >= 2) draw::SnakeHead(field_[gameIndexes_[currentFrame_]], snakeTurns_[1]);
            else draw::SnakeHead(field_[gameIndexes_[currentFrame_]], snakeTurns_[0]);
        }
        if (currentFoodIndex_ > 0 && _prevHeadIndex == foodIndexes_[currentFoodIndex_ - 1]) {
            snakeLength_--;
            field_[foodIndexes_[currentFoodIndex_]].type = CellType::PASS;
            draw::GameCell(field_[foodIndexes_[currentFoodIndex_--]]);
            field_[_prevHeadIndex].type = CellType::FOOD;
            draw::GameCell(field_[_prevHeadIndex]);
            snakeLengthChanged_ = true;
        }
        else if (snakeLength_ == maxPossibleSnakeLength_ && _prevHeadIndex == foodIndexes_.back()) {  // from last victory frame
            snakeLength_--;
            field_[_prevHeadIndex].type = CellType::FOOD;
            draw::GameCell(field_[_prevHeadIndex]);
            snakeLengthChanged_ = true;
        }
        else {
            snakeAssIndex_ = __FindCellFromMovementDirection(snakeAssIndex_, opposite(snakeTurns_[snakeLength_ - 1]));
            if (snakeLength_ >= 2) {
                field_[snakeAssIndex_].type = CellType::SNAKE_BODY;
                draw::GameCell(field_[snakeAssIndex_]);
            }
            if (_prevHeadIndex != snakeAssIndex_) {
                field_[_prevHeadIndex].type = CellType::PASS;
                draw::GameCell(field_[_prevHeadIndex]);
            }
        }
        snakeTurns_.pop_front();
    }

    CorrectScrollbarSlider();
    CorrectStateInfo();
    return true;
}
template bool Game::PrintPreviousFrame<NullMutex>(NullMutex&);
template bool Game::PrintPreviousFrame<mutex>(mutex&);

template <class Mtx>
void Game::PrintFrameByIndex(Mtx& m)
{
    lock_guard<Mtx> lock(m);
    if (newFrame == currentFrame_) return;
    frameChanged_ = true;
    if (newFrame > currentFrame_) {
        for (int i = currentFrame_ + 1; i <= min((int) gameIndexes_.size() - 1, newFrame); ++i) {
            int _prevHeadIndex = (i - 1 == -1) ? startingHeadIndex_ : gameIndexes_[i - 1];
            snakeTurns_.push_front(__FindMovementDirection(_prevHeadIndex, gameIndexes_[i]));
            field_[gameIndexes_[i]].type = CellType::SNAKE_HEAD;
            if (snakeLength_ >= 2 || gameIndexes_[i] == foodIndexes_[currentFoodIndex_]) {
                field_[_prevHeadIndex].type = CellType::SNAKE_BODY;
            }
            if (gameIndexes_[i] == foodIndexes_[currentFoodIndex_]) {
                if (++snakeLength_ < maxPossibleSnakeLength_) {
                    assert(currentFoodIndex_ < foodIndexes_.size() - 1);
                    field_[foodIndexes_[++currentFoodIndex_]].type = CellType::FOOD;
                }
                snakeLengthChanged_ = true;
            }
            else {
                if (gameIndexes_[i] != snakeAssIndex_) field_[snakeAssIndex_].type = CellType::PASS;
                snakeAssIndex_ = __FindCellFromMovementDirection(snakeAssIndex_, snakeTurns_[snakeLength_ - 1]);
            }
        }
        currentFrame_ = newFrame;
    }

    else {  // newFrame < currentFrame_
        if (currentFrame_ == gameIndexes_.size()) __RemoveCrash(gameIndexes_[--currentFrame_]);
        assert(currentFrame_ >= newFrame);
        while (currentFrame_ != newFrame) {
            int _prevHeadIndex = gameIndexes_[currentFrame_];
            if (--currentFrame_ == -1) field_[startingHeadIndex_].type = CellType::SNAKE_HEAD;
            else field_[gameIndexes_[currentFrame_]].type = CellType::SNAKE_HEAD;
            if (currentFoodIndex_ > 0 && _prevHeadIndex == foodIndexes_[currentFoodIndex_ - 1]) {
                snakeLength_--;
                field_[foodIndexes_[currentFoodIndex_--]].type = CellType::PASS;
                field_[_prevHeadIndex].type = CellType::FOOD;
                snakeLengthChanged_ = true;
            }
            else if (snakeLength_ == maxPossibleSnakeLength_ && _prevHeadIndex == foodIndexes_.back()) {  // from last victory frame
                snakeLength_--;
                field_[_prevHeadIndex].type = CellType::FOOD;
                snakeLengthChanged_ = true;
            }
            else {
                snakeAssIndex_ = __FindCellFromMovementDirection(snakeAssIndex_, opposite(snakeTurns_[snakeLength_ - 1]));
                if (snakeLength_ >= 2) field_[snakeAssIndex_].type = CellType::SNAKE_BODY;
                if (_prevHeadIndex != snakeAssIndex_) field_[_prevHeadIndex].type = CellType::PASS;
            }
            snakeTurns_.pop_front();
        }
    }

    draw::Field(field_, width_);
    for (int i = 0; i < field_.size(); ++i) {
        if (field_[i].type == CellType::SNAKE_HEAD) {
            if (snakeLength_ == 1) draw::SnakeHead(field_[i], __FindMovementDirection(i, gameIndexes_[0]));
            else draw::SnakeHead(field_[i], snakeTurns_[0]);
            break;
        }
    }
    if (newFrame == gameIndexes_.size()) __DrawCrash(gameIndexes_.back());
    CorrectScrollbarSlider();
    CorrectStateInfo();
}
template void Game::PrintFrameByIndex<NullMutex>(NullMutex&);
template void Game::PrintFrameByIndex<mutex>(mutex&);

GameAction Game::ProcessUserInput()
{
    if (MouseInput::isKeyboardEvent) {
        if (MouseInput::buttonPressed == ButtonPressed::ARROW_LEFT) {
            if (paused.load(std::memory_order_acquire)) return GameAction::FRAME_BACKWARD;
        }
        else if (MouseInput::buttonPressed == ButtonPressed::ARROW_RIGHT) {
            if (paused.load(std::memory_order_acquire)) return GameAction::FRAME_FORWARD;
        }
        else if (MouseInput::buttonPressed == ButtonPressed::DIGIT_1 && currentSpeedNo.load(std::memory_order_acquire) != 1) {
            newSpeedNo = 1;
            return GameAction::SET_SPEED;
        }
        else if (MouseInput::buttonPressed == ButtonPressed::DIGIT_2 && currentSpeedNo.load(std::memory_order_acquire) != 2) {
            newSpeedNo = 2;
            return GameAction::SET_SPEED;
        }
        else if (MouseInput::buttonPressed == ButtonPressed::DIGIT_3 && currentSpeedNo.load(std::memory_order_acquire) != 3) {
            newSpeedNo = 3;
            return GameAction::SET_SPEED;
        }
        else if (MouseInput::buttonPressed == ButtonPressed::DIGIT_4 && currentSpeedNo.load(std::memory_order_acquire) != 4) {
            newSpeedNo = 4;
            return GameAction::SET_SPEED;
        }
        else if (MouseInput::buttonPressed == ButtonPressed::DIGIT_5 && currentSpeedNo.load(std::memory_order_acquire) != 5) {
            newSpeedNo = 5;
            return GameAction::SET_SPEED;
        }
        else if (MouseInput::buttonPressed == ButtonPressed::WHITESPACE) return GameAction::START_PAUSE;
        else if (MouseInput::buttonPressed == ButtonPressed::CTRL) return GameAction::CHANGE_PLAYBACK_MODE;
        else if (MouseInput::buttonPressed == ButtonPressed::ESCAPE) return GameAction::EXIT_GA;
    }

    else {  // Mouse Input
        int x = MouseInput::X, y = MouseInput::Y;
        if (MouseInput::buttonPressed == ButtonPressed::RIGHT_BUTTON) return GameAction::EXIT_GA;
        if (MouseInput::buttonPressed == ButtonPressed::WHEEL_UP) {
            if (paused.load(std::memory_order_acquire)) return GameAction::FRAME_BACKWARD;
        }
        else if (MouseInput::buttonPressed == ButtonPressed::WHEEL_DOWN) {
            if (paused.load(std::memory_order_acquire)) return GameAction::FRAME_FORWARD;
        }
        else if (MouseInput::buttonPressed == ButtonPressed::LEFT_BUTTON) {
            if (y == fieldIndentY_ + height_ + 1 && x >= fieldIndentX_ && x < fieldIndentX_ + width_ * 2) {
                newFrame = scrollbar_->SliderPosToFrame(x - fieldIndentX_);
                return GameAction::SET_FRAME;
            }

            int _tableSidePadding = 1;
            int _indentSpeeds = fieldIndentX_ + leftPartWidth_ + s_spaceBetween_,
                _cellWidth = _tableSidePadding * 2 + 1;
            if (y == fieldIndentY_ + s_indentSpeedsTable_ && x > _indentSpeeds &&
                x < _indentSpeeds + s_speedNoToFrameTime_.size() * (_cellWidth + 1) && (x - _indentSpeeds) % (_cellWidth + 1))
            {
                newSpeedNo = (x - _indentSpeeds) / (_cellWidth + 1) + 1;
                if (currentSpeedNo.load(std::memory_order_acquire) != newSpeedNo) return GameAction::SET_SPEED;
                else return GameAction::NONE_GA;
            }

            int _indentArrowLeft = _indentSpeeds + 2;
            if (y == fieldIndentY_ + s_indentSpeedsTable_ + 2 && x > _indentArrowLeft && x <= _indentArrowLeft + _cellWidth) {
                if (paused.load(std::memory_order_acquire)) return GameAction::FRAME_BACKWARD;
                else return GameAction::NONE_GA;
            }

            int _indentArrowRight = _indentArrowLeft + (_cellWidth + 1) + (_tableSidePadding * 2 + 6);
            if (y == fieldIndentY_ + s_indentSpeedsTable_ + 2 && x > _indentArrowRight && x <= _indentArrowRight + _cellWidth) {
                if (paused.load(std::memory_order_acquire)) return GameAction::FRAME_FORWARD;
                else return GameAction::NONE_GA;
            }

            int _indentPause = _indentArrowLeft + _cellWidth + 1;
            if (y == fieldIndentY_ + s_indentSpeedsTable_ + 2 && x > _indentPause && x <= _indentPause + _tableSidePadding * 2 + 5) {
                return GameAction::START_PAUSE;
            }

            _cellWidth++;
            int _indentModeBwd = _indentSpeeds + 5;
            if (y == fieldIndentY_ + s_indentSpeedsTable_ + 4 && x > _indentModeBwd && x <= _indentModeBwd + _cellWidth) {
                if (forwardPlayback.load(std::memory_order_acquire)) return GameAction::CHANGE_PLAYBACK_MODE;
                else return GameAction::NONE_GA;
            }

            int _indentModeFwd = _indentModeBwd + 5;
            if (y == fieldIndentY_ + s_indentSpeedsTable_ + 4 && x > _indentModeFwd && x <= _indentModeFwd + _cellWidth) {
                if (!forwardPlayback.load(std::memory_order_acquire)) return GameAction::CHANGE_PLAYBACK_MODE;
                else return GameAction::NONE_GA;
            }
        }
    }

    return GameAction::NONE_GA;
}

void Game::SetAndPaintNewSpeedNo()
{
    lock_guard<mutex> lock(::mtx);
    speedsTable_->PaintCell(0, currentSpeedNo.load(std::memory_order_acquire) - 1, Color::NORMAL, CellSides());
    speedsTable_->PaintCell(0, newSpeedNo - 1, s_colorChosenSpeed_, CellSides());
    setPosition(fieldIndentX_ + leftPartWidth_ + s_spaceBetween_ + 10, upperMargin_ + s_indentSpeed_);
    cout << setw(2) << round((double) 1000 / s_speedNoToFrameTime_[newSpeedNo]);
    currentSpeedNo.store(newSpeedNo, std::memory_order_release);
}

void Game::SetAndPaintPause()
{
    lock_guard<mutex> lock(::mtx);
    if (paused.load(std::memory_order_acquire)) {
        speedsTable_->SetCellValue(1, 1, {"Пауза"});
        speedsTable_->PaintCell(1,  0, s_colorArrows_, CellSides(0, 0, 0, 0, 0, 1, 1, 1));
        speedsTable_->PaintCell(1, -1, s_colorArrows_, CellSides(0, 0, 0, 1, 1, 1, 0, 0));
        paused.store(false, std::memory_order_release);
    }
    else {
        paused.store(true, std::memory_order_release);
        speedsTable_->SetCellValue(1, 1, {"Пуск"});
        speedsTable_->PaintCell(1,  0, Color::NORMAL, CellSides(0, 0, 0, 0, 0, 1, 1, 1));
        speedsTable_->PaintCell(1, -1, Color::NORMAL, CellSides(0, 0, 0, 1, 1, 1, 0, 0));
    }
}

void Game::WriteReproductionMode(bool frwd)
{
    string caption;
    unique_lock<mutex> ulocker(::mtx, std::defer_lock);
    if (frwd) {
        if (width_ >= 12) caption = s_fwdModeCaption_;
        else caption = s_fwdModeCaptionShort_;
        ulocker.lock();
        speedsTable_->PaintCell(2, 0, Color::NORMAL, CellSides(0, 0, 0, 0, 0, 1, 1, 1));
        speedsTable_->PaintCell(2, 1, s_colorArrows_, CellSides(0, 0, 0, 1, 1, 1, 0, 0));
        setColor(Color::BRIGHT_GREEN_ON_BLACK);
        ulocker.unlock();
    }
    else {
        if (width_ >= 12) caption = s_revModeCaption_;
        else caption = s_revModeCaptionShort_;
        ulocker.lock();
        speedsTable_->PaintCell(2, 0, s_colorArrows_, CellSides(0, 0, 0, 0, 0, 1, 1, 1));
        speedsTable_->PaintCell(2, 1, Color::NORMAL, CellSides(0, 0, 0, 1, 1, 1, 0, 0));
        setColor(Color::RED_ON_BLACK);
        ulocker.unlock();
    }
    int dx = max(0, (int) (width_ * 2 - caption.size()) / 2);
    ulocker.lock();
    setPosition(fieldIndentX_, fieldIndentY_ - 1);
    cout << string(dx, ' ') << caption << "  ";
    setColor(Color::NORMAL);
}

void Game::DrawScrollbar()
{
    setPosition(fieldIndentX_, fieldIndentY_ + height_ + 1);
    setColor(s_colorScrollbar_);
    _setmode(_fileno(stdout), _O_U16TEXT);
    wcout << wstring(width_ * 2, Symbols::BoxLight::HORIZONTAL_LINE);
    _setmode(_fileno(stdout), _O_TEXT);
    setPosition(fieldIndentX_ + scrollbar_->sliderPos, fieldIndentY_ + height_ + 1);
    setColor(s_colorSlider_);
    cout << ' ';
    setColor(Color::NORMAL);
}

void Game::CorrectScrollbarSlider()
{
    if (scrollbar_->FrameToSliderPos(currentFrame_) != scrollbar_->sliderPos) {
        setPosition(fieldIndentX_ + scrollbar_->sliderPos, fieldIndentY_ + height_ + 1);
        setColor(s_colorScrollbar_);
        _setmode(_fileno(stdout), _O_U16TEXT);
        wcout << Symbols::BoxLight::HORIZONTAL_LINE;
        _setmode(_fileno(stdout), _O_TEXT);
        scrollbar_->sliderPos = scrollbar_->FrameToSliderPos(currentFrame_);
        setPosition(fieldIndentX_ + scrollbar_->sliderPos, fieldIndentY_ + height_ + 1);
        setColor(s_colorSlider_);
        cout << ' ';
        setColor(Color::NORMAL);
    }
}

void Game::CorrectStateInfo()
{
    if (frameChanged_) {
        frameChanged_ = false;
        setColor(Color::NORMAL);
        setPosition(fieldIndentX_ + leftPartWidth_ + s_spaceBetween_, upperMargin_ + s_indentFrameNo);
        cout << "Кадр: " << setw(maxFrameNoLength_) << currentFrame_ + 2;
    }
    if (snakeLengthChanged_) {
        snakeLengthChanged_ = false;
        setPosition(fieldIndentX_ + leftPartWidth_ + s_spaceBetween_, upperMargin_ + s_indentSnakeLength_);
        cout << "Длина змейки: " << setw(maxSnakeNoLength_) << snakeLength_;
        setPosition(fieldIndentX_ + leftPartWidth_ + s_spaceBetween_, upperMargin_ + s_indentApplesEaten_);
        cout << "Яблок съедено: " << setw(maxAppleNoLength_) << snakeLength_ - startingSnakeLength_;
    }
}

void Game::SetFieldIndents(int indentX, int indentY)
{
    fieldIndentX_ = indentX;
    fieldIndentY_ = indentY;
    for (int i = 0; i < initialField_.size(); ++i) initialField_[i] = Cell(i, width_, indentX, indentY, initialField_[i].type);
}

void Game::__CalculateMovesInfo()
{
    movesAmount_ = initialGameIndexes_.size() - (finalSnakeLength_ - startingSnakeLength_);
    auto it = find(initialGameIndexes_.rbegin(), initialGameIndexes_.rend(), lastFoodIndex_);
    if (it != initialGameIndexes_.rend()) {
        int _movesToLastFood = initialGameIndexes_.size() - distance(initialGameIndexes_.rbegin(), it);
        avgMovesToFood_ = (double) (_movesToLastFood - (finalSnakeLength_ - startingSnakeLength_ - 1)) / (finalSnakeLength_ - startingSnakeLength_);
    }
    else avgMovesToFood_ = -1;
}

void Game::__InitDimensions()
{
    maxFrameNoLength_ = numberLength(initialGameIndexes_.size() - (finalSnakeLength_ - startingSnakeLength_) + 2),
    maxSnakeNoLength_ = numberLength(finalSnakeLength_),
    maxAppleNoLength_ = numberLength(finalSnakeLength_ - startingSnakeLength_);

    leftPartWidth_ = width_ * 2;
    contentWidth_ = leftPartWidth_ + s_spaceBetween_ + max({21, maxFrameNoLength_ * 2 + 7, maxSnakeNoLength_ * 2 + 15, maxAppleNoLength_ * 2 + 16});
    contentHeight_ = max(height_ + 3, 12);
}

void Game::__FindSnakeAssIndex()
{
    snakeAssIndex_ = startingHeadIndex_;
    for (int i = 0; i < snakeTurns_.size(); ++i) snakeAssIndex_ = __FindCellFromMovementDirection(snakeAssIndex_, opposite(snakeTurns_[i]));
}
