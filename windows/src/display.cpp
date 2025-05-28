#include "../include/display.h"
#include <winuser.h>

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

    // setting window position
    RECT desktopRect;
    if (!GetWindowRect(GetDesktopWindow(), &desktopRect)) {
        throw std::runtime_error("Failed to get desktop window rect.");
    }
    Vector2 newPosition = {
        desktopRect.right - size.first - padding.first,
        desktopRect.bottom - size.second - padding.second
    };
    SetWindowPos(hwnd, HWND_TOPMOST, newPosition.first, newPosition.second, 
                 size.first, size.second, SWP_NOACTIVATE | SWP_SHOWWINDOW);
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
