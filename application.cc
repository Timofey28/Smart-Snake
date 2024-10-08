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
    }
}

void Application::CreateGames()
{
    draw::FieldParametersInputForm(playground_);
    exit(0);
}

int Application::ChooseOption()
{
    system("cls");
    cout << "\n\t1. ������� ����";
    cout << "\n\t2. ���������� ����";

    unordered_set<int> POSSIBLE_OPTIONS = {'1', '2', 27};
    int option;
    while (true) {
        option = getch();
        if (POSSIBLE_OPTIONS.find(option) != POSSIBLE_OPTIONS.end()) return option;
    }
}
