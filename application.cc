#include "application.h"

#include <iostream>
#include <unordered_set>
#include <conio.h>

using namespace std;


Application::Application()
{

}

void Application::Run()
{
    int option;
    while (true) {
//        option = ChooseOption();
        option = '1';
        switch (option)
        {
            case '1': CreateGames(); break;
            case '2': break;
            case 27: return;
        }
        break;
    }
}

void Application::CreateGames()
{
    playground_.FieldParametersInputForm();
    draw::EnterGamesAmount(gamesAmount_);
    playground_.SaveInitialData();

    for (int gameNumber = 1; gameNumber <= gamesAmount_; ++gameNumber) {
        playground_.ReinitializeInitialData();
        while (playground_.GameOn()) {
            playground_.CalculateNextIteration();
            _getch();
        }
        break;
    }
}

int Application::ChooseOption()
{
    system("cls");
    cout << "\n\t1. Создать игры";
    cout << "\n\t2. Посмотреть игру";

    unordered_set<int> POSSIBLE_OPTIONS = {'1', '2', 27};
    int option;
    while (true) {
        option = getch();
        if (POSSIBLE_OPTIONS.find(option) != POSSIBLE_OPTIONS.end()) return option;
    }
}
