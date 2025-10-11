#pragma once

#include <iostream>
#include <unordered_set>
#include <conio.h>

#include "playground.h"
#include "file_handler.h"
#include "draw.h"
#include "interface.h"
#include "mouse_input.h"


class Application
{
public:
    Application();
    void Run();

private:
    int gamesAmount_;
    Playground playground_;

    int ChooseOption();
    void CreateGames();
    void ListOfGames();
};
