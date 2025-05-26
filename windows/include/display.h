#pragma once

#include <utility>
#include <windows.h>
#include <string>
#include <deque>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

class KeyWindow {
private:
    // window info
    WNDCLASS wc = {};

    // main display
    HWND hwnd;
    std::pair<int, int> size;
    std::pair<int, int> position;
    std::pair<int, int> padding;
    std::pair<int, int> margin;

    // tray information
    NOTIFYICONDATA nid = {};
    HMENU hTrayMenu = NULL;
    HICON hIcon = NULL;

    // font information
    HFONT hFont;

    void ResizeWindow();
    void AddTrayIcon();
public:
    KeyWindow(HWND hwnd, std::pair<int, int> size = {800, 600}, std::pair<int, int> position = {100, 100},
              std::pair<int, int> padding = {10, 10}, std::pair<int, int> margin = {10, 10});

    HWND GetHandle();

    void WriteText(const std::wstring text);
};
