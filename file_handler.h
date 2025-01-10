#pragma once

#include <iostream>
#include <stack>
#include <vector>
#include <queue>
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
        Direction startingDirection,
        int snakeLength,
        int passCellsAmount,
        std::stack<Direction>& snakeTurnsStacked,
        std::vector<Cell>& field
    );
    /* ".initialdata"
    line 1) width, height, indentX, indentY, direction, snakeLength, passCellsAmount (current pass cells + snakeLength)
    line 2) snakeTurns ((snakeLength - 1) Direction values)
    line 3) field cell types ((width * height) CellType values)
    */

    void SaveLastGame(
        int firstFoodIndex,
        int finalSnakeLength,
        Direction crashDirection,
        std::vector<int> headAndFoodIndexes,
        int fieldWidth
    );
    /* "<n>.txt"
    line 1) firstFoodIndex, finalSnakeLength, crashDirection (0-3)
    line 2) head and food coordinates (xy) in number system with base 93
    */

    void ReadGame(
        fs::path gameFilePath,
        int& fieldWidth, int& fieldHeight,
        int& indentX, int& indentY,
        std::vector<Cell>& field,
        std::queue<Direction>& snakeTurns,
        Direction& startingDirection, Direction& crashDirection,
        int& startingsnakeLength, int& finalSnakeLength, int& maxPossibleSnakeLength,
        int& firstFoodIndex,
        std::vector<int>& gameIndexes
    );

private:
    void __CreateCurrentDirectory();
    int __GetFoldersAmount(fs::path directory);
    int __GetFilesAmount(fs::path directory);
    fs::path __GetGameInitialDataFilePath(fs::path gameFilePath);

    const fs::path GAMES_FOLDER = "Games";
    const fs::path INITIAL_DATA_FILE = ".initialdata";
    fs::path currentDirectory;
};
