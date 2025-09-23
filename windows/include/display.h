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
#include "settings_manager.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

class KeyWindow {
private:
    HWND hwnd = nullptr;
    std::wstring text = L"";

    std::shared_ptr<Box> boxEl = std::make_shared<Box>();
    std::shared_ptr<Text> textEl = std::make_shared<Text>();

    std::shared_ptr<Settings> appSettings = std::make_shared<Settings>();

    Logger logger = Logger("debug.log", 1024 * 1024); // 1MB max file size

public:
    KeyWindow(HINSTANCE hInstance, std::shared_ptr<Settings> appSettings);
    ~KeyWindow();

    bool dragging = false;

    HWND getHwnd();
    RECT getRect();
    void setText(const std::wstring& newText);
    void drawText(HDC hdc);

    void updateWindowPos(int x, int y);
};
