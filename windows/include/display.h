#pragma once

#include <utility>
#include <windows.h>
#include <string>
#include <deque>
#include <stdexcept>

#include "logging.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

class KeyWindow {
private:
    HWND hwnd = nullptr;
    RECT rect;
    POINT padding = {6, 4};
    std::wstring text = L"";

    POINT windowSize = {0, 0};
    POINT windowOffset = {0, 0};
    double transparency = 1.0;

    HFONT font = nullptr;

public:
    KeyWindow(HINSTANCE hInstance);
    ~KeyWindow();
    HWND getHwnd();
    RECT* getRect();
    void setText(const std::wstring& newText);
    void drawText(HDC hdc);
    KeyWindow& setFont(HFONT newFont) {
        font = newFont; 
        return *this; 
    }
};
