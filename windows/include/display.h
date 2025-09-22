#pragma once

#include <utility>
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <deque>
#include <stdexcept>
#include <winuser.h>

#include "logging.h"
#include "ui.h"

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

    std::unique_ptr<FontManager> fontManager = std::make_unique<FontManager>();
    FontProperties fontProperties;

    std::shared_ptr<Box> boxEl = std::make_shared<Box>();
    std::shared_ptr<Text> textEl = std::make_shared<Text>();

public:
    KeyWindow(HINSTANCE hInstance);
    ~KeyWindow();

    bool dragging = false;
    POINT dragOffset = {0, 0};

    HWND getHwnd();
    RECT* getRect();
    void setText(const std::wstring& newText);
    void drawText(HDC hdc);

    void updateWindowPos(int x, int y);
};
