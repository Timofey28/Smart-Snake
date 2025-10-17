#include "utils.h"

static const std::string BASE93_DIGITS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!\"#$%&'()*+,-./:;<=>?@[]^_`{|}~";
static std::unordered_map<char, int> BASE93_TO_INT;

std::mt19937 generator(time(nullptr));
//std::mt19937 generator(1);
std::uniform_int_distribution<int> uid2{0, 1};

std::string toString(CellType cellType)
{
    switch (cellType) {
        case CellType::PASS: return "Pass";
        case CellType::WALL: return "Wall";
        case CellType::PORTAL: return "Portal";
        case CellType::FOOD: return "Food";
        case CellType::SNAKE_BODY: return "Snake body";
        case CellType::SNAKE_HEAD: return "Snake head";
        default: return "Unknown";
    }
}
std::string toString(Direction direction)
{
    switch (direction) {
        case Direction::LEFT: return "Left";
        case Direction::RIGHT: return "Right";
        case Direction::UP: return "Up";
        case Direction::DOWN: return "Down";
        case Direction::NONE: return "None";
        default: return "Unknown" + std::to_string((int) direction);
    }
}


void initializeBase93Map()
{
    for (int i = 0; i < BASE93_DIGITS.size(); ++i)
        BASE93_TO_INT[BASE93_DIGITS[i]] = i;
}

char toBase93(int num)
{
    assert(num >= 0 && num < BASE93_DIGITS.size());
    return BASE93_DIGITS[num];
}

int fromBase93ToDecimal(char numChar)
{
    return BASE93_TO_INT[numChar];
}


Direction findMovementDirection(int fromIndex, int toIndex, int width)
{
    int fromX, fromY, toX, toY;
    fromX = fromIndex % width;
    fromY = fromIndex / width;
    toX = toIndex % width;
    toY = toIndex / width;

    if (fromY == toY) {
        if (abs(fromX - toX) == 1) {
            if (toX < fromX) return Direction::LEFT;
            else return Direction::RIGHT;
        }
        else {
            if (toX < fromX) return Direction::RIGHT;
            else return Direction::LEFT;
        }
    }
    else {  // fromX == toX
        if (abs(fromY - toY) == 1) {
            if (toY < fromY) return Direction::UP;
            else return Direction::DOWN;
        }
        else {
            if (toY < fromY) return Direction::DOWN;
            else return Direction::UP;
        }
    }
}

int findCellFromMovementDirection(int cellIndex, Direction movementDirection, const std::vector<Cell>& field, int width, int height)
{
    if (movementDirection == Direction::LEFT) cellIndex--;
    else if (movementDirection == Direction::RIGHT) cellIndex++;
    else if (movementDirection == Direction::UP) cellIndex -= width;
    else if (movementDirection == Direction::DOWN) cellIndex += width;
    else throw std::runtime_error("Unknown direction (-1): " + std::to_string((int) movementDirection));

    if (field[cellIndex].type == CellType::PORTAL) {
        int possibleCellIndex = getPortalExitIndex(cellIndex, movementDirection, field, width, height);
        if (possibleCellIndex != -1) return possibleCellIndex;
    }
    return cellIndex;
}

int getPortalExitIndex(int portalEnterIndex, Direction movementDirection, const std::vector<Cell>& field, int width, int height)
{
    assert(field[portalEnterIndex].type == CellType::PORTAL);
    int possibleExitIndex, nextCellIndex;
    int enterX = portalEnterIndex % width,
        enterY = portalEnterIndex / width;

    if (movementDirection == Direction::LEFT) {
        nextCellIndex = enterY * width + (enterX - 1);
        if (portalEnterIndex % width) {  // check if portal is correct for this orientation (horizontal)
            while (nextCellIndex % width) {
                if (field[nextCellIndex].type != CellType::WALL &&
                    field[nextCellIndex].type != CellType::PORTAL) return -1;
                nextCellIndex--;
            }
        }
        possibleExitIndex = enterY * width + (width - 1);
        while (possibleExitIndex != portalEnterIndex) {
            if (field[possibleExitIndex].type == CellType::PORTAL &&
                field[possibleExitIndex - 1].type != CellType::PORTAL &&
                field[possibleExitIndex - 1].type != CellType::WALL) return (possibleExitIndex - 1);
            possibleExitIndex--;
        }
    }

    else if (movementDirection == Direction::RIGHT) {
        nextCellIndex = enterY * width + (enterX + 1);
        if (portalEnterIndex % width < width - 1) {  // check if portal is correct for this orientation (horizontal)
            while (nextCellIndex % width < width - 1) {
                if (field[nextCellIndex].type != CellType::WALL &&
                    field[nextCellIndex].type != CellType::PORTAL) return -1;
                nextCellIndex++;
            }
        }
        possibleExitIndex = enterY * width;
        while (possibleExitIndex != portalEnterIndex) {
            if (field[possibleExitIndex].type == CellType::PORTAL &&
                field[possibleExitIndex + 1].type != CellType::PORTAL &&
                field[possibleExitIndex + 1].type != CellType::WALL) return (possibleExitIndex + 1);
            possibleExitIndex++;
        }
    }

    else if (movementDirection == Direction::UP) {
        nextCellIndex = portalEnterIndex - width;
        while (nextCellIndex / width > 0) {  // check if portal is correct for this orientation (vertical)
            if (field[nextCellIndex].type != CellType::WALL &&
                field[nextCellIndex].type != CellType::PORTAL) return -1;
            nextCellIndex -= width;
        }
        possibleExitIndex = (height - 1) * width + enterX;
        while (possibleExitIndex != portalEnterIndex) {
            if (field[possibleExitIndex].type == CellType::PORTAL &&
                field[possibleExitIndex - width].type != CellType::PORTAL &&
                field[possibleExitIndex - width].type != CellType::WALL) return (possibleExitIndex - width);
            possibleExitIndex -= width;
        }
    }

    else if (movementDirection == Direction::DOWN) {
        nextCellIndex = portalEnterIndex + width;
        while (nextCellIndex / width < height - 1) {  // check if portal is correct for this orientation (vertical)
            if (field[nextCellIndex].type != CellType::WALL &&
                field[nextCellIndex].type != CellType::PORTAL) return -1;
            nextCellIndex += width;
        }
        possibleExitIndex = enterX;
        while (possibleExitIndex != portalEnterIndex) {
            if (field[possibleExitIndex].type == CellType::PORTAL &&
                field[possibleExitIndex + width].type != CellType::PORTAL &&
                field[possibleExitIndex + width].type != CellType::WALL) return (possibleExitIndex + width);
            possibleExitIndex += width;
        }
    }
    else throw std::runtime_error("Unknown direction (-2).");

    return -1;
}


bool canConvertToNumber(std::string str)
{
    try {
        size_t pos;
        std::stoi(str, &pos);
        if (pos != str.length()) {
            return false;
        }
        return true;
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
}

int explainClickInfo(
    int relX,
    int relY,
    int fieldWidth,
    int fieldHeight,
    bool& isBorder,
    bool& isCorner,
    bool& isAdjacentToCorner
) {
    isBorder = false;
    isCorner = false;
    isAdjacentToCorner = false;
    if (relX == 0 || relX == fieldWidth - 1 || relY == 0 || relY == fieldHeight - 1) {
        isBorder = true;
        if (relX == 0 && relY == 0 || relX == 0 && relY == fieldHeight - 1 ||
            relX == fieldWidth - 1 && relY == 0 || relX == fieldWidth - 1 && relY == fieldHeight - 1)
        {
            isCorner = true;
        }
        else if (relX == 0 && relY == 1 || relX == 1 && relY == 0 ||  // левые верхние
                 relX == 0 && relY == fieldHeight - 2 || relX == 1 && relY == fieldHeight - 1 ||  // левые нижние
                 relX == fieldWidth - 2 && relY == fieldHeight - 1 || relX == fieldWidth - 1 && relY == fieldHeight - 2 || // правые нижние
                 relX == fieldWidth - 2 && relY == 0 || relX == fieldWidth - 1 && relY == 1)  // правые верхние
        {
            isAdjacentToCorner = true;
        }
    }
    return relY * fieldWidth + relX;
}

int getOppositeBorderCellIndex(int relX, int relY, int fieldWidth, int fieldHeight)
{
    int oppositeX = relX,
        oppositeY = relY;
    if (relX != 0 && relX != fieldWidth - 1) {
        if (relY == 0) oppositeY = fieldHeight - 1;
        else oppositeY = 0;
    }
    else {
        if (relX == 0) oppositeX = fieldWidth - 1;
        else oppositeX = 0;
    }
    return oppositeY * fieldWidth + oppositeX;
}

void getPairedAdjacentCellAndCornerCellIndex(
    int& pairedAdjacentCellIndex,
    int& cornerCellIndex,
    int relX,
    int relY,
    int fieldWidth,
    int fieldHeight
) {
    int cornerX, cornerY, adjX, adjY;
    if (relX == 0 && relY == 1) {
        cornerX = 0;
        cornerY = 0;
        adjX = 1;
        adjY = 0;
    }
    else if (relX == 1 && relY == 0) {
        cornerX = 0;
        cornerY = 0;
        adjX = 0;
        adjY = 1;
    }
    else if (relX == fieldWidth - 2 && relY == 0) {
        cornerX = fieldWidth - 1;
        cornerY = 0;
        adjX = fieldWidth - 1;
        adjY = 1;
    }
    else if (relX == fieldWidth - 1 && relY == 1) {
        cornerX = fieldWidth - 1;
        cornerY = 0;
        adjX = fieldWidth - 2;
        adjY = 0;
    }
    else if (relX == 0 && relY == fieldHeight - 2) {
        cornerX = 0;
        cornerY = fieldHeight - 1;
        adjX = 1;
        adjY = fieldHeight - 1;
    }
    else if (relX == 1 && relY == fieldHeight - 1) {
        cornerX = 0;
        cornerY = fieldHeight - 1;
        adjX = 0;
        adjY = fieldHeight - 2;
    }
    else if (relX == fieldWidth - 2 && relY == fieldHeight - 1) {
        cornerX = fieldWidth - 1;
        cornerY = fieldHeight - 1;
        adjX = fieldWidth - 1;
        adjY = fieldHeight - 2;
    }
    else if (relX == fieldWidth - 1 && relY == fieldHeight - 2) {
        cornerX = fieldWidth - 1;
        cornerY = fieldHeight - 1;
        adjX = fieldWidth - 2;
        adjY = fieldHeight - 1;
    }

    pairedAdjacentCellIndex = adjY * fieldWidth + adjX;
    cornerCellIndex = cornerY * fieldWidth + cornerX;
}

int randomUnder(int num)
{
    if (num == 2) return uid2(generator);
    std::uniform_int_distribution<int> uid{0, num - 1};
    return uid(generator);
}

Direction toLeftFrom(Direction direction)
{
    if (direction == Direction::LEFT) return Direction::DOWN;
    else if (direction == Direction::RIGHT) return Direction::UP;
    else if (direction == Direction::UP) return Direction::LEFT;
    else if (direction == Direction::DOWN) return Direction::RIGHT;
    else throw std::runtime_error("Unknown direction (-3).");
}

Direction toRightFrom(Direction direction)
{
    if (direction == Direction::LEFT) return Direction::UP;
    else if (direction == Direction::RIGHT) return Direction::DOWN;
    else if (direction == Direction::UP) return Direction::RIGHT;
    else if (direction == Direction::DOWN) return Direction::LEFT;
    else throw std::runtime_error("Unknown direction (-4).");
}

Direction opposite(Direction direction)
{
    if (direction == Direction::LEFT) return Direction::RIGHT;
    else if (direction == Direction::RIGHT) return Direction::LEFT;
    else if (direction == Direction::UP) return Direction::DOWN;
    else if (direction == Direction::DOWN) return Direction::UP;
    else throw std::runtime_error("Unknown direction (-5).");
}


bool isToday(time_t timestamp)
{
    time_t now = time(nullptr);
    tm tm_timestamp = *localtime(&timestamp);
    tm* tm_today = localtime(&now);
    return (tm_timestamp.tm_year == tm_today->tm_year) &&
           (tm_timestamp.tm_mon == tm_today->tm_mon) &&
           (tm_timestamp.tm_mday == tm_today->tm_mday);
}

bool isYesterday(time_t timestamp)
{
    time_t yesterday = time(nullptr) - 24 * 60 * 60;
    tm tm_timestamp = *localtime(&timestamp);
    tm* tm_yesterday = localtime(&yesterday);
    return (tm_timestamp.tm_year == tm_yesterday->tm_year) &&
           (tm_timestamp.tm_mon == tm_yesterday->tm_mon) &&
           (tm_timestamp.tm_mday == tm_yesterday->tm_mday);
}

std::string _timestampToString(time_t timestamp, std::string format)
{
    std::ostringstream oss;
    oss << std::put_time(localtime(&timestamp), format.c_str());
    return oss.str();
}
std::string timestampToDateStr(time_t timestamp)          { return _timestampToString(timestamp, "%d.%m.%Y"); }
std::string timestampToISOFormatDateStr(time_t timestamp) { return _timestampToString(timestamp, "%Y-%m-%d"); }
std::string timestampToHourMinuteStr(time_t timestamp)    { return _timestampToString(timestamp, "%H:%M"); }

time_t dateStrISOFormatToTimestamp(std::string dateStr)
{
    tm tm_struct = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm_struct, "%Y-%m-%d");
    return mktime(&tm_struct);
}

std::string doubleToStr(double value, int precision)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    std::string result = ss.str();
    if (result.find('.') == std::string::npos) return result;
    while (result.back() == '0') result.pop_back();
    if (result.back() == '.') result.pop_back();
    return result;
}

int numberLength(int number)
{
    if (number >= 1000000000) return 10;
    if (number >= 100000000) return 9;
    if (number >= 10000000) return 8;
    if (number >= 1000000) return 7;
    if (number >= 100000) return 6;
    if (number >= 10000) return 5;
    if (number >= 1000) return 4;
    if (number >= 100) return 3;
    if (number >= 10) return 2;
    return 1;
}
