#pragma once

#include <iostream>
#include <windows.h>
#include <cmath>
#include <map>
#include <stdexcept>
#include <cassert>
#include <thread>
#include <chrono>


struct Dimensions
{
    Dimensions(int _width, int _height) : width(_width), height(_height) {}
    int width, height;
};


class Console
{
public:
    static constexpr int s_startingFontSize = 40;
    static int s_currentFontSize, s_pointOfNoReturn;
    static Dimensions s_dimensions;
    static std::map<int, Dimensions> s_fontSizeToDimensions;
    static HANDLE s_handle;
    static DWORD s_previousMode;

    static void Initialize();
    static void SetFontSize(int newFontSize, bool pause = true);
    static void SetInitialFontSize();

private:
    static void __GetConsoleDimensions();
    static void __CorrectBufferSize();
    static void __SetOrUnsetFullscreen();
    static void __MakeCursorInvisible();
    static void __SetCursorToLeftEdge();
    static void __SetFontSize(int newFontSize);
};
