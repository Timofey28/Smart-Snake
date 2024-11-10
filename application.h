#pragma once

#include "playground.h"
#include "draw.h"


class Application
{
public:
    Application();
    void Run();

private:
    int ChooseOption();
    void CreateGames();

    int gamesAmount_;
    Playground playground_;
};
