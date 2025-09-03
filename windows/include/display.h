#pragma once

#include <utility>
#include <windows.h>
#include <string>
#include <deque>
#include <stdexcept>

typedef std::pair<int, int> Vector2;

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

class KeyWindow {
private:
    HWND hwnd;
    RECT rect;
    Vector2 size;
    Vector2 position;
    Vector2 padding;
    Vector2 margin;

public:
    KeyWindow(HINSTANCE hInstance);
    ~KeyWindow();
    HWND getHwnd();
    void drawText(HDC hdc, const std::wstring text);
};
