#include "display.h"
#include <winuser.h>

KeyWindow::KeyWindow(HINSTANCE hInstance) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    windowSize = {450, 40};
    windowOffset = {
        screenWidth-windowSize.x-40,
        (int)(screenHeight*0.8)-windowSize.y
    };
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE | WS_EX_LAYERED, 
        L"Key Display", 
        L"Keylogger", 
        WS_POPUP,
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

void KeyWindow::drawText(HDC hdc) {
    if (text.empty()) { return; }
    COLORREF textColor = RGB(255, 255, 255);
    SetTextColor(hdc, textColor);
    SetBkColor(hdc, backgroundColor);
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    HBRUSH brush = CreateSolidBrush(rectColor);
    FillRect(hdc, &rect, brush);
    SetLayeredWindowAttributes(hwnd, rectColor, (BYTE)(255*transparency), LWA_COLORKEY | LWA_ALPHA);
    DrawTextW(hdc, this->text.c_str(), -1, &rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
    SelectObject(hdc, oldFont);
    DeleteObject(brush);
}


void KeyWindow::setText(std::wstring const& newText) {
    this->text = newText;
}

RECT* KeyWindow::getRect() {
    return static_cast<RECT*>(&rect);
}
