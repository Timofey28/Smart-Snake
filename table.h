#pragma once

#include <vector>
#include <string>
#include <unordered_set>
#include <variant>
#include <sstream>

#include "draw.h"

template <typename... Types>
using MultiType = std::variant<Types...>;

typedef std::vector<int> vint;
typedef std::vector<std::vector<int>> vvint;
typedef std::vector<std::string> vstring;
typedef std::vector<std::vector<std::vector<std::string>>> vvvstring;
typedef std::vector<std::vector<std::vector<MultiType<int,char,float,double,std::string>>>> vvvany;
typedef std::vector<std::vector<Color>> vvColor;
typedef std::vector<std::pair<int, int>> vpintint;
typedef std::vector<std::unordered_set<int>> vusint;


enum RowsHorizontalAlignment
{
    LEFT_EDGE,
    CENTER,
    RIGHT_EDGE,
};


struct TableData
{
    TableData(int _indentX = 0, int _indentY = 0) :
        indentX(_indentX), indentY(_indentY),
        rows(-1), columns(-1),
        width(-1), height(-1),
        rowHeights({}),
        straightRowCellWidths({}),
        customRowCellWidths({}),
        values({}),
        contentSize(false),  // false (default): straight columns;  true: custom width for each cell
        customContentColor({}),
        contentColor(Color::NORMAL),
        borderColor(Color::NORMAL),
        rowsAlignment(LEFT_EDGE),
        writeBeyondLeftEdge(false)
        {}
    int indentX, indentY;
    int rows, columns, width, height;
    vint rowHeights, straightRowCellWidths;
    vvint customRowCellWidths;
    vvvany values;
    bool contentSize;
    vvColor customContentColor;
    Color contentColor, borderColor;
    RowsHorizontalAlignment rowsAlignment;
    bool writeBeyondLeftEdge;
};


struct CellSides
{
    // initialization from UPPER LEFT corner CLOCKWISE
    CellSides(bool leftUp=1, bool up=1, bool rightUp=1, bool right=1, bool rightDown=1, bool down=1, bool leftDown=1, bool left=1) :
        left(left), right(right), up(up), down(down), leftUp(leftUp), rightUp(rightUp), leftDown(leftDown), rightDown(rightDown) {}
    bool left, right, up, down;
    bool leftUp, rightUp, leftDown, rightDown;
};


template <typename BoxSymbols>
class Table
{
public:
    Table(const TableData& td);
    void Draw();
    void PaintCell(int row, int column, Color color, CellSides cellSides, bool onlyValue = 0, bool onlyBorders = 0);
    void SetCellValue(int row, int column, vstring value);

private:
    vint heights_, rowLengths_;
    vvint cellWidths_;
    vpintint rowIndents_;  // relative XY indents to the leftmost topmost part of row
    vusint rowVertBorderIndents_;
    vvvstring values_;
    vvColor contentColors_;
    bool contentSize_;
    Color borderColor_;
    RowsHorizontalAlignment rowsAlignment_;
    bool writeBeyondLeftEdge_;

    static constexpr int s_sidePaddding_ = 1;

    bool __InnerVectorSizesAreEqual(vvint vectors);
    void __SetPosition(int row, int column, int line);
    int __EndOfRow(int row);

    template <typename... Types>
    std::string __GetMultiTypeValue(std::ostringstream& oss, const std::variant<Types...>& var);
};
