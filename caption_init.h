#pragma once
#include "caption.h"

inline const Caption CAPTION_FIELD_DIMENSIONS_INPUT({
    CaptionRow({TextSpan("Reset (r) - сбросить введенные значения", Color::GRAY_ON_BLACK)}),
    CaptionRow({TextSpan("Exit (e) - вернуться в первое меню", Color::GRAY_ON_BLACK)}),
});

inline const Caption CAPTION_FIELD_ELEMENTS_ARRANGEMENT({
    CaptionRow({
        TextSpan("ЛКМ", Color::GOLD_ON_BLACK),
        TextSpan(" - добавить стену/портал"),
    }),
    CaptionRow({
        TextSpan("ПКМ", Color::GOLD_ON_BLACK),
        TextSpan(" - убрать что-либо"),
    }),
    CaptionRow({
        TextSpan("Ctrl+ЛКМ", Color::GOLD_ON_BLACK),
        TextSpan(" - добавить тело"),
    }),
    CaptionRow({TextSpan("змейки")}, 11),
    CaptionRow({
        TextSpan("Ctrl+ПКМ", Color::GOLD_ON_BLACK),
        TextSpan(" - добавить голову"),
    }),
    CaptionRow({TextSpan("змейки")}, 11),
    CaptionRow({
        TextSpan("Wheel", Color::GOLD_ON_BLACK),
        TextSpan(" - завершить"),
    }),
});
