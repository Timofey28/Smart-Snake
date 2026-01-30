#include "application.h"
using namespace std;


Application::Application()
{
    setlocale(0, "");
    SetConsoleCP(866);
    SetConsoleOutputCP(866);

    Console::Initialize();
    FileHandler::Initialize();
    MouseInput::Initialize();
    initializeBase93Map();
}

void Application::Run()
{
    int option;
    while (true) {
        option = ChooseOption();
        switch (option)
        {
            case '1': CreateGames(); break;
            case '2': ListOfGames(); break;
            case 27: return;
        }
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
        option = _getch();
        if (POSSIBLE_OPTIONS.find(option) != POSSIBLE_OPTIONS.end()) return option;
    }
}

void Application::CreateGames()
{
    if (!playground_.FieldParametersInputForm()) return;
    playground_.EnterGamesAmount();
    playground_.SaveInitialData();

    system("cls");

    auto start = chrono::high_resolution_clock::now();

    assert(playground_.gamesAmount <= 1000);
    draw::ProgressBar(0, playground_.gamesAmount);
    for (int gameNumber = 1; gameNumber <= playground_.gamesAmount; ++gameNumber) {
        playground_.ReinitializeStartingData();
        while (playground_.GameOn()) {
            playground_.CalculateNextIteration();
        }
        playground_.SaveLastGame();
        draw::ProgressBar(gameNumber, playground_.gamesAmount);
    }
    playground_.SaveGamesSummary();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "\n\n\tВыполнено за: " << round(duration.count() * 100) / 100 << " сек";
    MouseInput::WaitForAnyEvent();
}

void Application::ListOfGames()
{
    FileHandler::UpdateDatesAndExperimentAmounts();
    while (true) {
        bool experimentIsChosen = Interface::DatesAndExperimentsList();
        if (!experimentIsChosen) return;

        while (true) {
            Result result = Interface::GamesList();
            if (result == Result::EXIT) return;
            else if (result == Result::BACK) break;
            else if (result == Result::GAME_CHOSEN) Interface::GamePlayback();
            else throw runtime_error("invalid choosing game result");
        }
    }
}
