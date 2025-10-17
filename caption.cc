#include "caption.h"
using namespace std;

void Caption::Draw() const
{
    for (int i = 0; i < rows_.size(); ++i) {
        setPosition(indentX_ + rows_[i].indent, indentY_ + i);
        for (int j = 0; j < rows_[i].spans.size(); ++j) {
            setColor(rows_[i].spans[j].color);
            cout << rows_[i].spans[j].text;
        }
    }
}

int Caption::Width() const
{
    int longestWidth = 0, rowWidth;
    for (int i = 0; i < rows_.size(); ++i) {
        rowWidth = rows_[i].indent;
        for (int j = 0; j < rows_[i].spans.size(); ++j) {
            rowWidth += rows_[i].spans[j].text.size();
        }
        longestWidth = max(longestWidth, rowWidth);
    }
    return longestWidth;
}

void Caption::SetIndents(int indentX, int indentY) const
{
    this->indentX_ = indentX;
    this->indentY_ = indentY;
}
