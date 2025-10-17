#pragma once

#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <string>
#include <functional>

#include "utils.h"
#include "draw.h"
#include "console.h"


enum AlertType
{
    MULTIPLE_SNAKES,
    NO_SNAKES,
    INCORRECT_SNAKE,
    CLOSED_SPACES,
    LOOPED_SNAKE,
    NO_POSSIBLE_START,
};


class Alert
{
public:
    Alert() : shouldRemoveAlert_(false) {}
    void Show(
        AlertType alertType,
        std::vector<Cell>* field,
        int width,
        const std::function<void()>& cMovePortalsBackToBorder,
        const std::function<void()>& cDrawCaption
    );

private:
    static constexpr int s_displayDuration_ = 2000;
    static constexpr Color s_textColor = Color::BLACK_ON_RED;
    inline static const std::string s_alertMultipleSnakes = "Не обнаружено ни одной змейки. Добавь, пожалуйста!";
    inline static const std::string s_alertNoSnakes = "Змейка должна быть только одна!";
    inline static const std::string s_alertIncorrectSnake = "Змейка некорректна, ее несмежные части не могут находиться рядом друг с другом!";
    inline static const std::string s_alertClosedSpaces = "На игровом поле имеются комнаты, в которые невозможно попасть. Закрась их или сделай туда проход!";
    inline static const std::string s_alertLoopedSnake = "Змейка не может быть зациклена!";
    inline static const std::string s_alertNoPossibleStart = "Невозможно начать игру: змейке некуда ползти. Пожалуйста, освободи для нее немного места на поле!";

    std::unique_ptr<std::thread> thread_;
    std::condition_variable_any cv_;
    bool shouldRemoveAlert_, wasInterrupted_;

    void __RemovePreviousAlert();
    void __Show(
        const std::string& msg,
        std::vector<Cell>* field,
        int width,
        const std::function<void()>& cMovePortalsBackToBorder,
        const std::function<void()>& cDrawCaption
    );
};
