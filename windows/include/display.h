#pragma once

#include <utility>
#include <windows.h>
#include <string>
#include <deque>
#include <stdexcept>

#include "logging.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

struct Point {
    int x;
    int y;
};

class KeyWindow {
private:
    HWND hwnd = nullptr;
    RECT rect;
    Point padding = {6, 4};
    std::wstring text = L"";

    Point windowSize = {0, 0};
    Point windowOffset = {0, 0};

public:
    KeyWindow(HINSTANCE hInstance);
    ~KeyWindow();
    HWND getHwnd();
    void setText(const std::wstring& newText);
    void drawText(HDC hdc, HFONT hFont);
    Logger logger = Logger("build/display.log", 102480);
};
