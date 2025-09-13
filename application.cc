#include "application.h"

#include <iostream>
#include <unordered_set>
#include <conio.h>

using namespace std;

#include <thread>
#include <chrono>


Application::Application()
{

}

void Application::Run()
{
    int option;
    while (true) {
        option = ChooseOption();
//        option = '1';
        switch (option)
        {
            case '1': CreateGames(); break;
            case '2': break;
            case 27: return;
        }
        break;
    }
}

int Application::ChooseOption()
{
    system("cls");
    cout << "\n\t1. Создать игры";
    cout << "\n\t2. Список игр";

    unordered_set<int> POSSIBLE_OPTIONS = {'1', '2', 27};
    int option;
    while (true) {
        option = getch();
        if (POSSIBLE_OPTIONS.find(option) != POSSIBLE_OPTIONS.end()) return option;
    }
}

void Application::CreateGames()
{
    playground_.FieldParametersInputForm();
    draw::EnterGamesAmount(gamesAmount_);
    playground_.SaveInitialData();

    system("cls");

    auto start = chrono::high_resolution_clock::now();

    draw::ProgressBar(0, gamesAmount_);
    for (int gameNumber = 1; gameNumber <= gamesAmount_; ++gameNumber) {
        playground_.ReinitializeStartingData();
        while (playground_.GameOn()) {
            playground_.CalculateNextIteration();
        }
        playground_.SaveLastGame();
        draw::ProgressBar(gameNumber, gamesAmount_);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "\n\tВремя выполнения: " << round(duration.count() * 100) / 100 << " секунд\n";
}

void Application::ListOfGames()
{

}
