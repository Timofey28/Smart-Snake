#pragma once

#include "playground.h"
#include "draw.h"


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
