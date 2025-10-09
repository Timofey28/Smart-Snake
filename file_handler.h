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
    static std::map<time_t, fs::path> s_dateFolders;
    static std::map<time_t, int, std::greater<time_t>> s_experimentAmountsByDates;
    static std::map<time_t, int, std::greater<time_t>>::iterator s_currExpAmountsByDatesIter;

    static void Initialize();
    inline static fs::path GetGamesFolder() { return GAMES_FOLDER; }
    static time_t GetLastWriteTime(fs::path pathToFileOrFolder);

    /* ".initialdata"
    line 1) width, height, indentX, indentY, direction, snakeLength (from start), passCellsAmount (current pass cells + snakeLength)
    line 2) snakeTurns ((snakeLength - 1) Direction values)
    line 3) field cell types ((width * height) CellType values)
    */
    static void SaveInitialData(
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
    line 1) finalSnakeLength, firstFoodIndex, lastFoodIndex, crashDirection (0-3)
    line 2) head and food coordinates (xy) in number system with base 93
    */
    static void SaveGame(
        int finalSnakeLength,
        int firstFoodIndex,
        int lastFoodIndex,
        Direction crashDirection,
        std::vector<int> headAndFoodIndexes,
        int fieldWidth
    );

    static void ReadGame(
        fs::path gameFilePath,
        int& fieldWidth, int& fieldHeight,
        int& indentX, int& indentY,
        std::vector<Cell>& field,
        std::queue<Direction>& snakeTurns,
        Direction& startingDirection, Direction& crashDirection,
        int& startingsnakeLength, int& finalSnakeLength, int& maxPossibleSnakeLength,
        int& firstFoodIndex, int& lastFoodIndex,
        std::vector<int>& gameIndexes
    );

    static void UpdateDatesAndExperimentAmounts();
    static void GetExperimentInitialData(
        fs::path experimentFolderPath,
        int& fieldWidth, int& fieldHeight,
        int& initialSnakeLength, int& maxPossibleSnakeLength,
        std::vector<int>& gameScores
    );

private:
    static const fs::path GAMES_FOLDER;
    static const fs::path INITIAL_DATA_FILE;
    static fs::path s_currentDirectory_;

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

    static void __CreateCurrentDirectory();
    static int __GetFoldersAmount(fs::path directory);
    static int __GetFilesAmount(fs::path directory);
    static fs::path __GetGameInitialDataFilePath(fs::path gameFilePath);
};
