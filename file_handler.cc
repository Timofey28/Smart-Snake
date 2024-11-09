#include "file_handler.h"

using namespace std;


FileHandler::FileHandler()
{
    fs::create_directory(GAMES_FOLDER);
}

void FileHandler::SaveInitialData(
    int width,
    int height,
    int indentX,
    int indentY,
    int snakeLength,
    Direction startingDirection,
    int passCellsAmount,
    stack<Direction>& snakeTurnsStacked,
    vector<Cell>& field
) {
    __CreateCurrentDirectory();
    ofstream fout(currentDirectory / INITIAL_DATA_FILE);
    if (!fout.is_open()) {
        ostringstream oss;
        oss << "Unable to open file \"" << currentDirectory / INITIAL_DATA_FILE << "\".";
        throw runtime_error(oss.str());
    }

    fout << width << ' ' << height << ' ' << indentX << ' ' << indentY << ' ' << snakeLength << ' ' << startingDirection << ' ' << passCellsAmount << '\n';
    while (!snakeTurnsStacked.empty()) {
        fout << snakeTurnsStacked.top();
        snakeTurnsStacked.pop();
    }
    fout << '\n';
    for (int i = 0; i < field.size(); ++i) fout << field[i].type;

    fout.close();
}

void FileHandler::SaveLastGame(
    int firstFoodIndex,
    Direction crashDirection,
    int maxSnakeLength,
    vector<int> headAndFoodIndexes,
    int fieldWidth
) {
    if (!fs::exists(currentDirectory)) throw runtime_error("Current gaming directory was not found.");
    int filesAmount = __GetFilesAmount(currentDirectory);

    ofstream fout(currentDirectory / (to_string(filesAmount) + ".txt"));
    if (!fout.is_open()) {
        ostringstream oss;
        oss << "Unable to open file \"" << currentDirectory / (to_string(filesAmount) + ".txt") << "\".";
        throw runtime_error(oss.str());
    }

    fout << firstFoodIndex << ' ' << crashDirection << ' ' << maxSnakeLength << '\n';
    for (const auto& headOrFoodIndex : headAndFoodIndexes) {
        fout << toBase93(headOrFoodIndex % fieldWidth) << toBase93(headOrFoodIndex / fieldWidth);
    }

    fout.close();
}

void FileHandler::__CreateCurrentDirectory()
{
    auto nowTimePoint = chrono::system_clock::now();
    time_t nowTime = chrono::system_clock::to_time_t(nowTimePoint);
    ostringstream oss;
    oss << put_time(localtime(&nowTime), "%Y-%m-%d");
    fs::path todayFolder = oss.str();
    if (fs::exists(GAMES_FOLDER / todayFolder)) {
        int foldersAmount = __GetFoldersAmount(GAMES_FOLDER / todayFolder);
        currentDirectory = GAMES_FOLDER / todayFolder / to_string(foldersAmount + 1);
    }
    else currentDirectory = GAMES_FOLDER / todayFolder / "1";
    fs::create_directories(currentDirectory);
}

int FileHandler::__GetFoldersAmount(fs::path directory)
{
    int foldersAmount = 0;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (fs::is_directory(entry)) foldersAmount++;
    }
    return foldersAmount;
}

int FileHandler::__GetFilesAmount(fs::path directory)
{
    int filesAmount = 0;
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (fs::is_regular_file(entry)) filesAmount++;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }
    return filesAmount;
}
