#pragma once

#include <iostream>
#include <stack>
#include <vector>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "utils.h"

namespace fs = std::filesystem;


class FileHandler
{
public:
    FileHandler();
    void SaveInitialData(
        int width,
        int height,
        int indentX,
        int indentY,
        int snakeLength,
        Direction startingDirection,
        int passCellsAmount,
        std::stack<Direction>& snakeTurnsStacked,
        std::vector<Cell>& field
    );
    /* ".initialdata"
    line 1) width, height, indentX, indentY, snakeLength, direction, passCellsAmount (current pass cells + snakeLength)
    line 2) snakeTurns ((snakeLength - 1) Direction values)
    line 3) field cell types ((width * height) CellType values)
    */

    void SaveLastGame(
        int firstFoodIndex,
        Direction crashDirection,
        int maxSnakeLength,
        std::vector<int> headAndFoodIndexes,
        int fieldWidth
    );
    /* "<n>.txt"
    line 1) firstFoodIndex, crashDirection (0-3), maxSnakeLength
    line 2) headAndFoodIndexes in number system with base 93
    */

private:
    void __CreateCurrentDirectory();
    int __GetFoldersAmount(fs::path directory);
    int __GetFilesAmount(fs::path directory);

    const fs::path GAMES_FOLDER = "Games";
    const fs::path INITIAL_DATA_FILE = ".initialdata";
    fs::path currentDirectory;
};
