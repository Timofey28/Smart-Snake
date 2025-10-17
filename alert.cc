#include "alert.h"
using namespace std;

void Alert::Show(
    AlertType alertType,
    vector<Cell>* field,
    int width,
    const function<void()>& cMovePortalsBackToBorder,
    const function<void()>& cDrawCaption
) {
    unique_lock<recursive_mutex> ulocker(rmtx, std::defer_lock);
    __RemovePreviousAlert();
    string msg;
    switch (alertType) {
        case MULTIPLE_SNAKES: msg = s_alertMultipleSnakes; break;
        case NO_SNAKES: msg = s_alertNoSnakes; break;
        case INCORRECT_SNAKE: msg = s_alertIncorrectSnake; break;
        case CLOSED_SPACES: msg = s_alertClosedSpaces; break;
        case LOOPED_SNAKE: msg = s_alertLoopedSnake; break;
        case NO_POSSIBLE_START: msg = s_alertNoPossibleStart; break;
    };
    ulocker.lock();
    shouldRemoveAlert_ = false;
    ulocker.unlock();
    thread_ = make_unique<thread>([this, msg, field, width, cMovePortalsBackToBorder, cDrawCaption]() {
        this->__Show(msg, field, width, cMovePortalsBackToBorder, cDrawCaption);
    });
}

void Alert::__RemovePreviousAlert()
{
    unique_lock<recursive_mutex> ulocker(rmtx);
    shouldRemoveAlert_ = true;
    ulocker.unlock();

    cv_.notify_one();
    if (thread_ && thread_->joinable()) {
        thread_->join();
        thread_.reset();
    }
}

void Alert::__Show(
    const string& msg,
    vector<Cell>* field,
    int width,
    const function<void()>& cMovePortalsBackToBorder,
    const function<void()>& cDrawCaption
) {
    int indentX = 0;
    if (msg.size() < Console::s_dimensions.width) indentX = (Console::s_dimensions.width - msg.size()) / 2;
    unique_lock<recursive_mutex> ulocker(rmtx);
    setPosition(indentX, 0);
    setColor(s_textColor);
    cout << msg;

    wasInterrupted_ = cv_.wait_for(ulocker, chrono::milliseconds(s_displayDuration_), [this]() {
        return this->shouldRemoveAlert_;
    });

    setPosition(indentX, 0);
    setColor(Color::NORMAL);
    cout << string(msg.size(), ' ');
    draw::Field(*field, width);
    if (!wasInterrupted_) cMovePortalsBackToBorder();
    cDrawCaption();
}
