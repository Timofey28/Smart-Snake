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

    fout << width << ' ' << height << ' ' << indentX << ' ' << indentY << ' ' << snakeLength << ' ' << startingDirection << '\n';
    while (!snakeTurnsStacked.empty()) {
        fout << snakeTurnsStacked.top();
        snakeTurnsStacked.pop();
    }
    fout << '\n';
    for (int i = 0; i < field.size(); ++i) fout << field[i].type;

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
        int foldersAmount = 0;
        for (const auto& entry : fs::directory_iterator(GAMES_FOLDER / todayFolder)) {
            if (fs::is_directory(entry)) ++foldersAmount;
        }
        currentDirectory = GAMES_FOLDER / todayFolder / to_string(foldersAmount + 1);
    }
    else currentDirectory = GAMES_FOLDER / todayFolder / "1";
    fs::create_directories(currentDirectory);
}
