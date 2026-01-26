#include "table.h"
using namespace std;

template <typename BoxSymbols>
Table<BoxSymbols>::Table(const TableData& td)
{
    assert(s_sidePaddding_ >= 0);

    if (td.rowHeights.size()) heights_ = td.rowHeights;
    else if (td.rows != -1) heights_ = vector<int>(td.rows, (td.height != -1) ? td.height : 1);
    else if (td.customRowCellWidths.size()) heights_ = vector<int>(td.customRowCellWidths.size(), (td.height != -1) ? td.height : 1);
    else if (!td.values.size()) throw runtime_error("Variable 'heights_' could not be initialized. At least one of these must be passed: 'rowHeights', 'rows', 'customRowCellWidths'.");

    if (td.straightRowCellWidths.size()) cellWidths_ = vector<vector<int>>(heights_.size(), td.straightRowCellWidths);
    else if (td.customRowCellWidths.size()) cellWidths_ = td.customRowCellWidths;
    else if (td.rows != -1 && td.columns != -1 && td.width != -1) cellWidths_ = vector<vector<int>>(td.rows, vector<int>(td.columns, td.width));
    else if (!td.values.size()) throw runtime_error("Variable 'cellWidths_' must be initialized if parameter 'values' is not passed. At least one of these must be passed: 'straightRowCellWidths', 'customRowCellWidths', ('rows' && 'columns' && 'width').");

//    setPosition(0,1); cout << "rowWidths: ";
//    for (int i = 0; i < cellWidths_.size(); ++i) {
//        for (auto x : cellWidths_[i]) cout << x << ' ';
//        cout << "\n";
//    }
//    setPosition(0,0); cout << "heights: ";
//    for (auto x : heights_) cout << x <<  ' ';
//    _getch();

    if (td.values.size()) {
        ostringstream oss;
        values_.resize(td.values.size());
        for (int row = 0; row < td.values.size(); ++row) {
            values_[row].resize(td.values[row].size());
            for (int cell = 0; cell < td.values[row].size(); ++cell) {
                values_[row][cell].resize(td.values[row][cell].size());
                for (int line = 0; line < td.values[row][cell].size(); ++line) {
                    values_[row][cell][line] = __GetMultiTypeValue(oss, td.values[row][cell][line]);
                }
            }
        }

        if (!heights_.size() || !cellWidths_.size()) {
            // set custom row heights (heights_: vvint) and cell widths (cellWidths_: vvint) if needed depending on the content (values)
            bool _needToSetHeights = false, _needToSetRowWidths = false;
            if (!heights_.size()) {
                heights_.resize(values_.size());
                _needToSetHeights = true;
            }
            if (!cellWidths_.size()) {
                cellWidths_.resize(values_.size());
                _needToSetRowWidths = true;
            }
            int _maxLineWidth, _maxCellHeight;
            for (int row = 0; row < values_.size(); ++row) {
                if (_needToSetRowWidths) cellWidths_[row].resize(values_[row].size());
                _maxCellHeight = 1;
                for (int cell = 0; cell < values_[row].size(); ++cell) {
                    _maxLineWidth = 0;
                    for (const auto& line : values_[row][cell]) _maxLineWidth = max(_maxLineWidth, (int) line.size());
                    if (_needToSetRowWidths) cellWidths_[row][cell] = _maxLineWidth;
                    _maxCellHeight = max(_maxCellHeight, (int) values_[row][cell].size());
                }
                if (_needToSetHeights) heights_[row] = _maxCellHeight;
            }

            if (_needToSetRowWidths && !td.contentSize && cellWidths_.size() > 1 && __InnerVectorSizesAreEqual(cellWidths_)) {
                // make cell widths the same in each column
                int _maxCellWidth, _maxCellsInRow = cellWidths_[0].size();
                for (int row = 1; row < cellWidths_.size(); ++row) _maxCellsInRow = max(_maxCellsInRow, (int) cellWidths_[row].size());
                for (int col = 0; col < _maxCellsInRow; ++col) {
                    _maxCellWidth = 0;
                    for (int row = 0; row < cellWidths_.size(); ++row) {
//                        if (row < cellWidths_.size())
                            _maxCellWidth = max(_maxCellWidth, cellWidths_[row][col]);
                    }
                    for (int row = 0; row < cellWidths_.size(); ++row) cellWidths_[row][col] = _maxCellWidth;
                }
            }
        }

        // validate values
        assert(values_.size() <= cellWidths_.size());
        for (int row = 0; row < values_.size(); ++row) {
            assert(values_[row].size() <= cellWidths_[row].size());
            for (int cell = 0; cell < values_[row].size(); ++cell) {
                assert(values_[row][cell].size() <= heights_[row]);
            }
        }
    }

    // validate customContentColor
    if (td.customContentColor.size()) {
        contentColors_ = td.customContentColor;
        assert(contentColors_.size() == cellWidths_.size());
        for (int row = 0; row < contentColors_.size(); ++row) {
            assert(contentColors_[row].size() == cellWidths_[row].size());
        }
    }
    else {
        contentColors_.resize(cellWidths_.size());
        for (int row = 0; row < cellWidths_.size(); ++row) {
            contentColors_[row] = vector<Color>(cellWidths_[row].size(), td.contentColor);
        }
    }

    int _maxRowLength = 0;
    rowLengths_.resize(cellWidths_.size());
    for (int row = 0; row < cellWidths_.size(); ++row) {
        int _currentRowLength = 1;  // left border
        for (int cell = 0; cell < cellWidths_[row].size(); ++cell) {
            _currentRowLength += cellWidths_[row][cell] + s_sidePaddding_ * 2 + 1;
        }
        _maxRowLength = max(_maxRowLength, _currentRowLength);
        rowLengths_[row] = _currentRowLength;
    }

    // indents to the left border of first row of each line
    rowIndents_.resize(cellWidths_.size());
    for (int row = 0; row < cellWidths_.size(); ++row) {
        int dx = 0;  // always with rowsAlignment == RowsHorizontalAlignment::LEFT_EDGE
        if (td.rowsAlignment == RowsHorizontalAlignment::CENTER) {
            if (td.writeBeyondLeftEdge) dx = (rowLengths_[0] - rowLengths_[row]) / 2;
            else dx = (_maxRowLength - rowLengths_[row]) / 2;
        }
        else if (td.rowsAlignment == RowsHorizontalAlignment::RIGHT_EDGE) {
            if (td.writeBeyondLeftEdge) dx = rowLengths_[0] - rowLengths_[row];
            else dx = _maxRowLength - rowLengths_[row];
        }
        if (row == 0) rowIndents_[0] = {td.indentX + dx, td.indentY + 1};
        else rowIndents_[row] = {td.indentX + dx, rowIndents_[row - 1].second + heights_[row - 1] + 1};
    }

    // absolute column border indents of vertical cell borders in rows
    rowVertBorderIndents_.resize(cellWidths_.size());
    for (int row = 0; row < cellWidths_.size(); ++row) {
        int _previousNum = rowIndents_[row].first;
        unordered_set<int> borderNums = {_previousNum};
        for (int cell = 0; cell < cellWidths_[row].size(); ++cell) {
            borderNums.insert(_previousNum + cellWidths_[row][cell] + s_sidePaddding_ * 2 + 1);
            _previousNum += cellWidths_[row][cell] + s_sidePaddding_ * 2 + 1;
        }
        rowVertBorderIndents_[row] = borderNums;
    }

    contentSize_ = td.contentSize;
    borderColor_ = td.borderColor;
    rowsAlignment_ = td.rowsAlignment;
    writeBeyondLeftEdge_ = td.writeBeyondLeftEdge;
}

template <typename BoxSymbols>
void Table<BoxSymbols>::Draw()
{
    // borders
    _setmode(_fileno(stdout), _O_U16TEXT);
    setColor(borderColor_);
    setPosition(rowIndents_[0].first, rowIndents_[0].second - 1);
    wcout << BoxSymbols::LEFT_UP_CORNER;
    for (int cell = 0; cell < cellWidths_[0].size(); ++cell) {
        wcout << wstring(cellWidths_[0][cell] + s_sidePaddding_ * 2, BoxSymbols::HORIZONTAL_LINE);
        if (cell == cellWidths_[0].size() - 1) wcout << BoxSymbols::RIGHT_UP_CORNER;
        else wcout << BoxSymbols::TSHAPE_UP;
    }
    for (int row = 0; row < cellWidths_.size(); ++row) {
        // middle row part
        for (int line = 0; line < heights_[row]; ++line) {
            setPosition(rowIndents_[row].first, rowIndents_[row].second + line);
            wcout << BoxSymbols::VERTICAL_LINE;
            for (int cell = 0; cell < cellWidths_[row].size(); ++cell) {
                wcout << wstring(cellWidths_[row][cell] + s_sidePaddding_ * 2, ' ') << BoxSymbols::VERTICAL_LINE;
            }
        }

        // row floor
        if (row == cellWidths_.size() - 1) {
            setPosition(rowIndents_[row].first, rowIndents_[row].second + heights_[row]);
            wcout << BoxSymbols::LEFT_DOWN_CORNER;
            for (int cell = 0; cell < cellWidths_[row].size(); ++cell) {
                wcout << wstring(cellWidths_[row][cell] + s_sidePaddding_ * 2, BoxSymbols::HORIZONTAL_LINE);
                if (cell == cellWidths_[row].size() - 1) wcout << BoxSymbols::RIGHT_DOWN_CORNER;
                else wcout << BoxSymbols::TSHAPE_DOWN;
            }
        }
        else {
            int col;
            if (rowIndents_[row + 1].first < rowIndents_[row].first) {
                setPosition(rowIndents_[row + 1].first, rowIndents_[row + 1].second - 1);
                for (col = rowIndents_[row + 1].first; col < rowIndents_[row].first; ++col) {
                    if (col == rowIndents_[row + 1].first) wcout << BoxSymbols::LEFT_UP_CORNER;
                    else {
                        if (rowVertBorderIndents_[row + 1].contains(col)) wcout << BoxSymbols::TSHAPE_UP;
                        else wcout << BoxSymbols::HORIZONTAL_LINE;
                    }
                }
                if (rowVertBorderIndents_[row + 1].contains(col)) wcout << BoxSymbols::CROSS;
                else wcout << BoxSymbols::TSHAPE_DOWN;
            }
            else {
                setPosition(rowIndents_[row].first, rowIndents_[row + 1].second - 1);
                if (rowVertBorderIndents_[row + 1].contains(rowIndents_[row].first)) wcout << BoxSymbols::TSHAPE_LEFT;
                else wcout << BoxSymbols::LEFT_DOWN_CORNER;
            }

            // lower [row] border
            for (col = rowIndents_[row].first + 1; col < rowIndents_[row].first + rowLengths_[row] - 1; ++col) {
                if (rowVertBorderIndents_[row].contains(col)) {
                    if (rowVertBorderIndents_[row + 1].contains(col)) wcout << BoxSymbols::CROSS;
                    else wcout << BoxSymbols::TSHAPE_DOWN;
                }
                else if (rowVertBorderIndents_[row + 1].contains(col)) wcout << BoxSymbols::TSHAPE_UP;
                else wcout << BoxSymbols::HORIZONTAL_LINE;
            }
            // last symbol
            if (rowVertBorderIndents_[row + 1].contains(col)) {
                if (rowIndents_[row + 1].first + rowLengths_[row + 1] > rowIndents_[row].first + rowLengths_[row]) wcout << BoxSymbols::CROSS;
                else wcout << BoxSymbols::TSHAPE_RIGHT;
            }
            else {
                if (rowIndents_[row + 1].first + rowLengths_[row + 1] > rowIndents_[row].first + rowLengths_[row]) wcout << BoxSymbols::TSHAPE_DOWN;
                else wcout << BoxSymbols::RIGHT_DOWN_CORNER;
            }

            // [row + 1] ceiling, part after the end of lower [row] border
            for (col = rowIndents_[row].first + rowLengths_[row]; col < rowIndents_[row + 1].first + rowLengths_[row + 1]; ++col) {
                if (col == rowIndents_[row + 1].first + rowLengths_[row + 1] - 1) {
                    wcout << BoxSymbols::RIGHT_UP_CORNER;
                    break;
                }
                if (rowVertBorderIndents_[row + 1].contains(col)) wcout << BoxSymbols::TSHAPE_UP;
                else wcout << BoxSymbols::HORIZONTAL_LINE;
            }
        }
    }
    _setmode(_fileno(stdout), _O_TEXT);

    // content
    for (int row = 0; row < values_.size(); ++row) {
        for (int cell = 0; cell < values_[row].size(); ++cell) {
            setColor(contentColors_[row][cell]);
            for (int line = 0; line < values_[row][cell].size() && line < heights_[row]; ++line) {
                __SetPosition(row, cell, line);
                if (values_[row][cell][line].size() > cellWidths_[row][cell]) {
                    cout << values_[row][cell][line].substr(0, cellWidths_[row][cell]);
                    if (s_sidePaddding_) {
                        _setmode(_fileno(stdout), _O_U16TEXT);
                        wcout << Symbols::ELLIPSIS;
                        _setmode(_fileno(stdout), _O_TEXT);
                    }
                }
                else cout << values_[row][cell][line];
            }
        }
    }

    setColor(Color::NORMAL);
}

template <typename BoxSymbols>
void Table<BoxSymbols>::PaintCell(int row, int column, Color color, CellSides cellSides, bool onlyValue, bool onlyBorders)
{
    if (onlyBorders && onlyValue) throw runtime_error("'onlyBorders' and 'onlyValue' cannot both be set to 'true' simultaneously.");
    if (row < 0) row += heights_.size();
    if (!(0 <= row < cellWidths_.size())) return;
    if (column < 0) column += cellWidths_[row].size();
    if (!(0 <= column <= cellWidths_[row].size())) return;
    int x = rowIndents_[row].first,
        y = rowIndents_[row].second - 1;
    for (int cell = 0; cell < column; ++cell) x += cellWidths_[row][cell] + 2 * s_sidePaddding_ + 1;
    setColor(color);

    if (!onlyValue) {
        _setmode(_fileno(stdout), _O_U16TEXT);

        // corners
        if (cellSides.leftUp) {
            setPosition(x, y);
            if (row > 0 && rowVertBorderIndents_[row - 1].contains(x)) {
                if (column == 0) wcout << BoxSymbols::TSHAPE_LEFT;
                else wcout << BoxSymbols::CROSS;
            }
            else {
                if (column == 0 && !(row > 0 && rowIndents_[row - 1].first < x)) wcout << BoxSymbols::LEFT_UP_CORNER;
                else wcout << BoxSymbols::TSHAPE_UP;
            }
        }
        if (cellSides.rightUp) {
            int _col = x + cellWidths_[row][column] + 2 * s_sidePaddding_ + 1;
            setPosition(_col, y);
            if (row > 0 && rowVertBorderIndents_[row - 1].contains(_col)) {
                if (column == cellWidths_[row].size() - 1) wcout << BoxSymbols::TSHAPE_RIGHT;
                else wcout << BoxSymbols::CROSS;
            }
            else {
                if (column == cellWidths_[row].size() - 1 && !(row > 0 && __EndOfRow(row - 1) > _col)) wcout << BoxSymbols::RIGHT_UP_CORNER;
                else wcout << BoxSymbols::TSHAPE_UP;
            }
        }
        if (cellSides.leftDown) {
            setPosition(x, y + heights_[row] + 1);
            if (row < heights_.size() - 1 && rowVertBorderIndents_[row + 1].contains(x)) {
                if (column == 0) wcout << BoxSymbols::TSHAPE_LEFT;
                else wcout << BoxSymbols::CROSS;
            }
            else {
                if (column == 0 && !(row < heights_.size() - 1 && rowIndents_[row + 1].first < x)) wcout << BoxSymbols::LEFT_DOWN_CORNER;
                else wcout << BoxSymbols::TSHAPE_DOWN;
            }
        }
        if (cellSides.rightDown) {
            int _col = x + cellWidths_[row][column] + 2 * s_sidePaddding_ + 1;
            setPosition(_col, y + heights_[row] + 1);
            if (row < heights_.size() - 1 && rowVertBorderIndents_[row + 1].contains(_col)) {
                if (column == cellWidths_[row].size() - 1) wcout << BoxSymbols::TSHAPE_RIGHT;
                else wcout << BoxSymbols::CROSS;
            }
            else {
                if (column == cellWidths_[row].size() - 1 && !(row < heights_.size() - 1 && __EndOfRow(row + 1) > _col)) wcout << BoxSymbols::RIGHT_DOWN_CORNER;
                else wcout << BoxSymbols::TSHAPE_DOWN;
            }
        }

        // border sides
        if (cellSides.up) {
            setPosition(x + 1, y);
            for (int _col = x + 1; _col <= x + cellWidths_[row][column] + 2 * s_sidePaddding_; ++_col) {
                if (row > 0 && rowVertBorderIndents_[row - 1].contains(_col)) wcout << BoxSymbols::TSHAPE_DOWN;
                else wcout << BoxSymbols::HORIZONTAL_LINE;
            }
        }
        if(cellSides.down) {
            setPosition(x + 1, y + heights_[row] + 1);
            for (int _col = x + 1; _col <= x + cellWidths_[row][column] + 2 * s_sidePaddding_; ++_col) {
                if (row < heights_.size() - 1 && rowVertBorderIndents_[row + 1].contains(_col)) wcout << BoxSymbols::TSHAPE_UP;
                else wcout << BoxSymbols::HORIZONTAL_LINE;
            }
        }
        if (cellSides.left) {
            for (int line = 0; line < heights_[row]; ++line) {
                setPosition(x, y + 1 + line);
                wcout << BoxStyle::VERTICAL_LINE;
            }
        }
        if (cellSides.right) {
            for (int line = 0; line < heights_[row]; ++line) {
                setPosition(x + cellWidths_[row][column] + 2 * s_sidePaddding_ + 1, y + 1 + line);
                wcout << BoxStyle::VERTICAL_LINE;
            }
        }

        _setmode(_fileno(stdout), _O_TEXT);
    }

    if (!onlyBorders) {
        contentColors_[row][column] = color;
        for (int line = 0; line < values_[row][column].size(); ++line) {
            __SetPosition(row, column, line);
            if (values_[row][column][line].size() > cellWidths_[row][column]) {
                cout << values_[row][column][line].substr(0, cellWidths_[row][column]);
                if (s_sidePaddding_) {
                    _setmode(_fileno(stdout), _O_U16TEXT);
                    wcout << Symbols::ELLIPSIS;
                    _setmode(_fileno(stdout), _O_TEXT);
                }
            }
            else cout << values_[row][column][line];
        }
    }

    setColor(Color::NORMAL);
}

template <typename BoxSymbols>
void Table<BoxSymbols>::SetCellValue(int row, int column, vstring value)
{
    if (row < 0) row += heights_.size();
    if (!(0 <= row < cellWidths_.size())) return;
    if (column < 0) column += cellWidths_[row].size();
    if (!(0 <= column <= cellWidths_[row].size())) return;
    setColor(contentColors_[row][column]);
    for (int line = 0; line < value.size() && line < heights_[row]; ++line) {
        __SetPosition(row, column, line);
        if (value[line].size() > cellWidths_[row][column]) {
            cout << value[line].substr(0, cellWidths_[row][column]);
            if (s_sidePaddding_) {
                _setmode(_fileno(stdout), _O_U16TEXT);
                wcout << Symbols::ELLIPSIS;
                _setmode(_fileno(stdout), _O_TEXT);
            }
        }
        else cout << value[line] << string(cellWidths_[row][column] - value[line].size(), ' ');
    }
    for (int line = value.size(); line < heights_[row]; ++line) {
        __SetPosition(row, column, line);
        cout << string(cellWidths_[row][column], ' ');
    }
    setColor(Color::NORMAL);
}

template <typename BoxSymbols>
bool Table<BoxSymbols>::__InnerVectorSizesAreEqual(vvint vectors)
{
    for (int i = 1; i < vectors.size(); ++i) {
        if (vectors[i].size() != vectors[0].size()) return false;
    }
    return true;
}

template <typename BoxSymbols>
void Table<BoxSymbols>::__SetPosition(int row, int column, int line)
{
    int x = rowIndents_[row].first;
    for (int i = 0; i < column; ++i) x += cellWidths_[row][i] + s_sidePaddding_ * 2 + 1;
    x += 1 + s_sidePaddding_;
    int y = rowIndents_[row].second + line;
    setPosition(x, y);
}

template <typename BoxSymbols>
int Table<BoxSymbols>::__EndOfRow(int row)
{
    assert(0 <= row < heights_.size());
    int endOfRow = rowIndents_[row].first;
    for (int cell = 0; cell < cellWidths_[row].size(); ++cell) endOfRow += cellWidths_[row][cell] + 2 * s_sidePaddding_ + 1;
    return endOfRow;
}

template <typename BoxSymbols>
template <typename... Types>
string Table<BoxSymbols>::__GetMultiTypeValue(ostringstream& oss, const std::variant<Types...>& var)
{
    oss.str("");
    oss.clear();
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, var);
    return oss.str();
}

template class Table<Symbols::BoxLight>;
template class Table<Symbols::BoxHeavy>;
