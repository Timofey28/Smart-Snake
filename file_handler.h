#pragma once

#include <iostream>
#include <stack>
#include <vector>
#include <queue>
#include <map>
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

    /* ".initialdata"
    line 1) width, height, indentX, indentY, direction, snakeLength (from start), passCellsAmount (current pass cells + snakeLength)
    line 2) snakeTurns ((snakeLength - 1) Direction values)
    line 3) field cell types ((width * height) CellType values)
    */
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

    /* "<n>.txt"
    line 1) firstFoodIndex, finalSnakeLength, crashDirection (0-3)
    line 2) head and food coordinates (xy) in number system with base 93
    */
    void SaveLastGame(
        int firstFoodIndex,
        int finalSnakeLength,
        Direction crashDirection,
        std::vector<int> headAndFoodIndexes,
        int fieldWidth
    );

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

    std::map<time_t, int> GetDatesAndExperimentAmounts();

private:
    const fs::path GAMES_FOLDER = "Games";
    const fs::path INITIAL_DATA_FILE = ".initialdata";
    fs::path currentDirectory_;
    std::map<time_t, fs::path> dateFolders_;

//    static const map<int, string> monthNoToStr = {
//        {0, "января"},
//        {1, "февраля"},
//        {2, "марта"},
//        {3, "апреля"},
//        {4, "мая"},
//        {5, "июня"},
//        {6, "июля"},
//        {7, "августа"},
//        {8, "сентября"},
//        {9, "октября"},
//        {10, "ноября"},
//        {11, "декабря"},
//    };

    void __CreateCurrentDirectory();
    int __GetFoldersAmount(fs::path directory);
    int __GetFilesAmount(fs::path directory);
    fs::path __GetGameInitialDataFilePath(fs::path gameFilePath);
};
