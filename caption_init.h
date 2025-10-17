#pragma once
#include "caption.h"

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
