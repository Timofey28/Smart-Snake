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
    Direction startingDirection,
    int snakeLength,
    int passCellsAmount,
    stack<Direction>& snakeTurnsStacked,
    vector<Cell>& field
) {
    __CreateCurrentDirectory();
    ofstream fout(currentDirectory_ / INITIAL_DATA_FILE);
    if (!fout.is_open()) {
        ostringstream oss;
        oss << "Unable to open file \"" << currentDirectory_ / INITIAL_DATA_FILE << "\".";
        throw runtime_error(oss.str());
    }

    fout << width << ' ' << height << ' ' << indentX << ' ' << indentY << ' ';
    fout << startingDirection << ' ' << snakeLength << ' ' << passCellsAmount << '\n';
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
    int finalSnakeLength,
    Direction crashDirection,
    vector<int> headAndFoodIndexes,
    int fieldWidth
) {
    if (!fs::exists(currentDirectory_)) throw runtime_error("Current gaming directory was not found.");
    int filesAmount = __GetFilesAmount(currentDirectory_);

    ofstream fout(currentDirectory_ / (to_string(filesAmount) + ".txt"));
    if (!fout.is_open()) {
        ostringstream oss;
        oss << "Unable to open file \"" << currentDirectory_ / (to_string(filesAmount) + ".txt") << "\".";
        throw runtime_error(oss.str());
    }

    fout << firstFoodIndex << ' ' << finalSnakeLength << ' ' << crashDirection << '\n';
    for (const auto& headOrFoodIndex : headAndFoodIndexes) {
        fout << toBase93(headOrFoodIndex % fieldWidth) << toBase93(headOrFoodIndex / fieldWidth);
    }

    fout.close();
}

void FileHandler::ReadGame(
    fs::path gameFilePath,
    int& fieldWidth, int& fieldHeight,
    int& indentX, int& indentY,
    vector<Cell>& field,
    queue<Direction>& snakeTurns,
    Direction& startingDirection, Direction& crashDirection,
    int& startingSnakeLength, int& finalSnakeLength, int& maxPossibleSnakeLength,
    int& firstFoodIndex,
    vector<int>& gameIndexes
) {
    fs::path initialDataFilePath = __GetGameInitialDataFilePath(gameFilePath);
    ifstream fin;
    fin.open(initialDataFilePath);
    if (!fin.is_open()) throw runtime_error("Unable to open .initialdata file.");
    char directionValue, cellTypeValue;

    // .initialdata, line 1
    fin >> fieldWidth >> fieldHeight >> indentX >> indentY >> directionValue >> startingSnakeLength >> maxPossibleSnakeLength;
    startingDirection = static_cast<Direction>(directionValue - 48);

    // .initialdata, line 2
    snakeTurns = {};
    while (fin.peek() != '\n') {
        fin >> directionValue;
        snakeTurns.push(static_cast<Direction>(directionValue - 48));
    }

    // .initialdata, line 3
    field.resize(fieldWidth * fieldHeight);
    CellType cellType;
    for (int i = 0; i < fieldWidth * fieldHeight; ++i) {
        fin >> cellTypeValue;
        cellType = static_cast<CellType>(cellTypeValue - 48);
        field[i] = Cell(i, fieldWidth, indentX, indentY, cellType);
    }
    fin.close();

    fin.open(gameFilePath);
    if (!fin.is_open()) throw runtime_error("Unable to open game file.");

    // game file, line 1
    fin >> firstFoodIndex >> finalSnakeLength >> directionValue;
    crashDirection = static_cast<Direction>(directionValue - 48);

    // game file, line 2
    char coordX, coordY;
    while (fin >> coordX >> coordY) {
        gameIndexes.push_back(fromBase93ToDecimal(coordY) * fieldWidth + fromBase93ToDecimal(coordX));
    }
}

map<time_t, int> FileHandler::GetDatesAndExperimentAmounts()
{
    map<time_t, int> experimentAmountsByDates;
    std::tm tm = {};
    time_t tt;

    dateFolders_.clear();
    for (const auto& entry : fs::directory_iterator(GAMES_FOLDER)) {
        if (!fs::is_directory(entry)) {
            cerr << "Not a directory encountered among date folders: " << entry;
            continue;
        }

        try {
            istringstream ss(entry.path().string());
            ss >> std::get_time(&tm, "%Y-%m-%d");
            if (ss.fail()) throw runtime_error("Error in date interpretation.");
            tt = std::mktime(&tm);
            experimentAmountsByDates[tt] = __GetFoldersAmount(GAMES_FOLDER / entry);
            dateFolders_[tt] = GAMES_FOLDER / entry;
        } catch (const std::exception& e) {
            throw runtime_error(e.what());
        }
    }

    return experimentAmountsByDates;
}


fs::path FileHandler::__GetGameInitialDataFilePath(fs::path gameFilePath)
{
    if (!fs::exists(gameFilePath)) throw runtime_error("Path to game not found.");
    fs::path parentFolder = gameFilePath.parent_path();
    return parentFolder / INITIAL_DATA_FILE;
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
        currentDirectory_ = GAMES_FOLDER / todayFolder / to_string(foldersAmount + 1);
    }
    else currentDirectory_ = GAMES_FOLDER / todayFolder / "1";
    fs::create_directories(currentDirectory_);
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
