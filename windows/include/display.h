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
    // window properties
    HWND hwnd;
    Vector2 size;
    Vector2 position;
    Vector2 padding;
    Vector2 margin;

    // additional helpful properties
    RECT desktopRect;

    // font information
    HFONT hFont = CreateFontW(
        28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    // helper functions
    void ResizeWindow(Vector2 newSize);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    KeyWindow(HINSTANCE hInstance,
              Vector2 size,
              Vector2 position,
              Vector2 padding,
              Vector2 margin
              );

    HWND GetHWND();

    void UpdateHWND(HWND hwnd);

    void WriteText(const std::wstring text);
};
