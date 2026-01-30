#include "file_handler.h"

using namespace std;

map<time_t, fs::path> FileHandler::s_dateFolders;
map<time_t, int, greater<time_t>> FileHandler::s_experimentAmountsByDates;
map<time_t, int, greater<time_t>>::iterator FileHandler::s_currExpAmountsByDatesIter = FileHandler::s_experimentAmountsByDates.end();
fs::path FileHandler::s_currentDirectory_;


time_t FileHandler::GetLastWriteTime(fs::path pathToFileOrFolder)
{
    auto ftime = fs::last_write_time(pathToFileOrFolder);
    auto ctp = chrono::time_point_cast<chrono::system_clock::duration>(
        ftime - fs::file_time_type::clock::now() + chrono::system_clock::now()
    );
    return chrono::system_clock::to_time_t(ctp);
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
    ofstream fout(s_currentDirectory_ / INITIAL_DATA_FILE);
    if (!fout.is_open()) {
        ostringstream oss;
        oss << "Unable to open file \"" << s_currentDirectory_ / INITIAL_DATA_FILE << "\".";
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

void FileHandler::SaveGame(
    int finalSnakeLength,
    int firstFoodIndex,
    int lastFoodIndex,
    Direction crashDirection,
    vector<int> headAndFoodIndexes,
    int fieldWidth
) {
    if (!fs::exists(s_currentDirectory_)) throw runtime_error("Current gaming directory was not found.");
    int filesAmount = __GetFilesAmount(s_currentDirectory_);

    ofstream fout(s_currentDirectory_ / (to_string(filesAmount) + ".txt"));
    if (!fout.is_open()) {
        ostringstream oss;
        oss << "Unable to open file \"" << s_currentDirectory_ / (to_string(filesAmount) + ".txt") << "\".";
        throw runtime_error(oss.str());
    }

    fout << finalSnakeLength << ' ' << firstFoodIndex << ' ' << lastFoodIndex << ' ' << crashDirection << '\n';
    for (const auto& headOrFoodIndex : headAndFoodIndexes) {
        fout << toBase93(headOrFoodIndex % fieldWidth) << toBase93(headOrFoodIndex / fieldWidth);
    }

    fout.close();
}

void FileHandler::SaveGamesSummary(const vector<int>& gameScores)
{
    ofstream fout(s_currentDirectory_ / GAMES_SUMMARY_FILE);
    if (!fout.is_open()) {
        ostringstream oss;
        oss << "Unable to open file \"" << s_currentDirectory_ / GAMES_SUMMARY_FILE << "\".";
        throw runtime_error(oss.str());
    }

    for (const auto& score : gameScores) fout << score << ' ';
    fout.close();
}

void FileHandler::ReadGame(
    fs::path gameFilePath,
    int& fieldWidth, int& fieldHeight,
    int& indentX, int& indentY,
    vector<Cell>& field,
    deque<Direction>& snakeTurns,
    Direction& startingDirection, Direction& crashDirection,
    int& startingSnakeLength, int& finalSnakeLength, int& maxPossibleSnakeLength,
    int& firstFoodIndex, int& lastFoodIndex,
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

    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // skip line break character

    // .initialdata, line 2
    snakeTurns.clear();
    while (fin.peek() != '\n') {
        fin >> directionValue;
        snakeTurns.push_front(static_cast<Direction>(directionValue - 48));
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
    fin >> finalSnakeLength >> firstFoodIndex >> lastFoodIndex >> directionValue;
    crashDirection = static_cast<Direction>(directionValue - 48);

    // game file, line 2
    char coordX, coordY;
    gameIndexes.clear();
    while (fin >> coordX >> coordY) {
        gameIndexes.push_back(fromBase93ToDecimal(coordY) * fieldWidth + fromBase93ToDecimal(coordX));
    }
}

void FileHandler::UpdateDatesAndExperimentAmounts()
{
    tm tm = {};
    time_t tt;

    s_dateFolders.clear();
    s_experimentAmountsByDates.clear();
    for (const auto& entry : fs::directory_iterator(GAMES_FOLDER)) {
        if (!fs::is_directory(entry)) {
            cerr << "Not a directory encountered among date folders: " << entry;
            continue;
        }

        try {
            istringstream ss(entry.path().filename().string());
            ss >> std::get_time(&tm, "%Y-%m-%d");
            if (ss.fail()) throw runtime_error("Error in date interpretation, expected format: \"yyyy-mm-dd\", received: \"" + entry.path().string() + "\"");
            tt = std::mktime(&tm);
            s_dateFolders[tt] = entry;
            s_experimentAmountsByDates[tt] = __GetFoldersAmount(entry);
        } catch (const std::exception& e) {
            throw runtime_error(e.what());
        }
    }
}

void FileHandler::GetExperimentInitialData(
    fs::path experimentFolderPath,
    int& fieldWidth, int& fieldHeight,
    int& initialSnakeLength, int& maxPossibleSnakeLength,
    std::vector<int>& gameScores
) {
    ifstream fin(experimentFolderPath / INITIAL_DATA_FILE);
    if (!fin.is_open()) throw runtime_error("Unable to open file \"" + (experimentFolderPath / INITIAL_DATA_FILE).generic_string() + "\".");
    int _;
    fin >> fieldWidth >> fieldHeight >> _ >> _ >> _ >> initialSnakeLength >> maxPossibleSnakeLength;
    fin.close();

    gameScores.clear();
    fin.open(experimentFolderPath / GAMES_SUMMARY_FILE);
    if (!fin.is_open()) throw runtime_error("Unable to open file \"" + (experimentFolderPath / GAMES_SUMMARY_FILE).generic_string() + "\".");
    int _score;
    while (fin >> _score) gameScores.push_back(_score);
    fin.close();
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
        s_currentDirectory_ = GAMES_FOLDER / todayFolder / to_string(foldersAmount + 1);
    }
    else s_currentDirectory_ = GAMES_FOLDER / todayFolder / "1";
    fs::create_directories(s_currentDirectory_);
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
