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

    std::shared_ptr<Box> container = std::make_shared<Box>();

    POINT windowSize = {0, 0};
    POINT windowOffset = {0, 0};
    double transparency = 1.0;

    HFONT font = nullptr;
    COLORREF backgroundColor = RGB(0, 0, 0);
    COLORREF rectColor = RGB(255, 0, 0);

    std::unique_ptr<FontManager> fontManager = std::make_unique<FontManager>();
    FontProperties fontProperties;

public:
    KeyWindow(HINSTANCE hInstance);
    ~KeyWindow();
    HWND getHwnd();
    RECT* getRect();
    void setText(const std::wstring& newText);
    void drawText(HDC hdc);
    KeyWindow& setFont(HFONT newFont) {
        font = newFont; 
        RECT temp = this->rect;
        InvalidateRect(hwnd, &temp, TRUE);
        return *this; 
    }
    KeyWindow& setBackgroundColor(COLORREF newBackgroundColor) {
        if (newBackgroundColor == rectColor) {
            rectColor = backgroundColor;
        }
        backgroundColor = newBackgroundColor; 
        RECT temp = this->rect;
        InvalidateRect(hwnd, &temp, TRUE);
        return *this;
    }
};
