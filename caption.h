#pragma once

#include <vector>

#include "utils.h"
#include "draw.h"


struct TextSpan
{
    TextSpan(std::string _text, Color _color = Color::NORMAL) :
        text(_text), color(_color) {}
    std::string text;
    Color color;
};

struct CaptionRow
{
    CaptionRow(std::vector<TextSpan> _textSpans, int _indent = 0) :
        spans(std::move(_textSpans)), indent(_indent) {}
    std::vector<TextSpan> spans;
    int indent;
};


class Caption
{
public:
    Caption(std::vector<CaptionRow> _captionRows) :
        rows_(std::move(_captionRows)), indentX_(-1), indentY_(-1) {}
    int Width() const;
    int Height() const { return rows_.size(); }
    template<class Mtx = NullMutex> void Draw(Mtx& m = noLock) const;
    void Clear(Color backgroundColor = Color::BLACK) const;
    void SetIndents(int indentX, int indentY) const;

private:
    std::vector<CaptionRow> rows_;
    mutable int indentX_, indentY_;
};
