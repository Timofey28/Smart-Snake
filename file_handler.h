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
        std::stack<Direction>& snakeTurnsStacked,
        std::vector<Cell>& field
    );
    /* ".initialdata"
    line 1) width, height, indentX, indentY, snakeLength, direction
    line 2) snakeTurns ((snakeLength - 1) Direction values)
    line 3) field cell types ((width * height) CellType values)
    */

private:
    void __CreateCurrentDirectory();

    const fs::path GAMES_FOLDER = "Games";
    const fs::path INITIAL_DATA_FILE = ".initialdata";
    fs::path currentDirectory;
};
