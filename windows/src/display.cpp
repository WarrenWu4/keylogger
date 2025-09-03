#include "display.h"
#include <winuser.h>

KeyWindow::KeyWindow(HINSTANCE hInstance) {
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"Key Display", L"Keylogger", WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 40,
        NULL, NULL, hInstance, NULL
    );
    GetClientRect(hwnd, &rect);
    ShowWindow(hwnd, SW_SHOW);
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

void KeyWindow::drawText(HDC hdc, const std::wstring text) {
    COLORREF bgColor = RGB(0, 0, 0);
    COLORREF textColor = RGB(255, 255, 255);
    HBRUSH hBrush = CreateSolidBrush(bgColor);
    // SIZE size;
    // if (!GetTextExtentPoint32W(hdc, text.c_str(), text.length(), &size)) {
    //     ReleaseDC(hwnd, hdc);
    //     throw std::runtime_error("Failed to get text extent.");
    // }
    FillRect(hdc, &rect, hBrush);
    SetTextColor(hdc, textColor);
    DrawText(hdc, text.c_str(), -1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
    DeleteObject(hBrush);
}
