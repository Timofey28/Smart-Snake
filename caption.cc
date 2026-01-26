#include "caption.h"
using namespace std;

template<class Mtx>
void Caption::Draw(Mtx& m) const
{
    lock_guard<Mtx> lock(m);
    for (int i = 0; i < rows_.size(); ++i) {
        setPosition(indentX_ + rows_[i].indent, indentY_ + i);
        for (int j = 0; j < rows_[i].spans.size(); ++j) {
            setColor(rows_[i].spans[j].color);
            cout << rows_[i].spans[j].text;
        }
    }
    setColor(Color::NORMAL);
}
template void Caption::Draw<NullMutex>(NullMutex&) const;
template void Caption::Draw<recursive_mutex>(recursive_mutex&) const;

void Caption::Clear(Color backgroundColor) const
{
    setColor(backgroundColor);
    for (int i = 0; i < rows_.size(); ++i) {
        setPosition(indentX_ + rows_[i].indent, indentY_ + i);
        for (int j = 0; j < rows_[i].spans.size(); ++j) {
            cout << string(rows_[i].spans[j].text.size(), ' ');
        }
    }
    setColor(Color::NORMAL);
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
