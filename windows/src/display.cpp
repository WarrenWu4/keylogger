#include "display.h"
#include <winuser.h>

KeyWindow::KeyWindow(HINSTANCE hInstance) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth = 400;
    int windowHeight = 40;
    int windowWidthOffset = screenWidth - windowWidth - 40;
    int windowHeightOffset = (int)(screenHeight*0.8) - windowHeight;
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"Key Display", L"Keylogger", WS_POPUP,
        windowWidthOffset, windowHeightOffset, // position
        windowWidth, windowHeight, // size (width, height)
        NULL, NULL, hInstance, NULL
    );
    GetClientRect(hwnd, &rect);
}

KeyWindow::~KeyWindow() {
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = NULL;
    }
}

HWND KeyWindow::getHwnd() {
    return this->hwnd;
}

void KeyWindow::drawText(HDC hdc, HFONT hFont) {
    COLORREF bgColor = RGB(0, 0, 0);
    COLORREF textColor = RGB(255, 255, 255);
    HBRUSH hBrush = CreateSolidBrush(bgColor);
    FillRect(hdc, &rect, hBrush);
    SetTextColor(hdc, textColor);
    SetBkColor(hdc, TRANSPARENT);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    RECT paddingRect = {
        rect.left+padding.x, 
        rect.top+padding.y, 
        rect.right-padding.x, 
        rect.bottom-padding.y
    };
    DrawTextW(hdc, this->text.c_str(), -1, &paddingRect, DT_SINGLELINE | DT_VCENTER);
    SelectObject(hdc, oldFont);
    DeleteObject(hBrush);
}

void KeyWindow::setText(std::wstring const& newText) {
    this->text = newText;
}
