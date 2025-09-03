#include "display.h"
#include <winuser.h>

KeyWindow::KeyWindow(HINSTANCE hInstance) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    windowSize = {400, 40};
    windowOffset = {
        screenWidth-windowSize.x-40,
        (int)(screenHeight*0.8)-windowSize.y
    };
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"Key Display", L"Keylogger", WS_POPUP,
        windowOffset.x, windowOffset.y, // position
        windowSize.x, windowSize.y, // size (width, height)
        NULL, NULL, hInstance, NULL
    );
    GetClientRect(hwnd, &rect); // {0, 0, windowSize.x, windowSize.y}
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
    if (text.empty()) { return; }
    COLORREF bgColor = RGB(0, 0, 0);
    COLORREF textColor = RGB(255, 255, 255);
    SetTextColor(hdc, textColor);
    SetBkColor(hdc, bgColor);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    DrawTextW(hdc, this->text.c_str(), -1, &rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
    SelectObject(hdc, oldFont);
}

void KeyWindow::setText(std::wstring const& newText) {
    this->text = newText;
}
