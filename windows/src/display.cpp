#include "../include/display.h"

/*
void ResizeWindowToText(HWND hwnd, HFONT hFont, std::deque<std::wstring>& keyBuffer) {
    HDC hdc = GetDC(hwnd);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
    SIZE size;
    std::wstring keys = L"";
    for (const auto& key : keyBuffer) {
        keys += key + L"";
    }
    GetTextExtentPoint32W(hdc, keys.c_str(), keys.length(), &size);
    SelectObject(hdc, oldFont);
    ReleaseDC(hwnd, hdc);
    int padding = 32;
    int newWidth = size.cx + padding;
    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);
    int screenHeight = desktop.bottom;
    int screenWidth = desktop.right;
    int height = 48;
    int x = screenWidth - newWidth - padding;
    int y = screenHeight - height;
    MoveWindow(hwnd, x, y, newWidth, height, TRUE);
}
*/

KeyWindow::KeyWindow(
    HWND hwnd,
    Vector2 size,
    Vector2 position,
    Vector2 padding,
    Vector2 margin
) {
    this->hwnd = hwnd;
    this->size = size;
    this->position = position;
    this->padding = padding;
    this->margin = margin;
}

void KeyWindow::ResizeWindow() {

}

void KeyWindow::UpdateHWND(HWND hwnd) {
    this->hwnd = hwnd;
}

HWND KeyWindow::GetHWND() {
    return this->hwnd;
}

void KeyWindow::WriteText(const std::wstring text) {

}
