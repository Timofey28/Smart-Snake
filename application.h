#pragma once

#include <iostream>
#include <unordered_set>
#include <conio.h>

#include "console.h"
#include "file_handler.h"
#include "mouse_input.h"
#include "utils.h"
#include "draw.h"
#include "playground.h"
#include "interface.h"

void initializeBase93Map();


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
