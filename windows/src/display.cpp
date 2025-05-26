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

KeyWindow::KeyWindow(HWND hwnd, std::pair<int, int> size, std::pair<int, int> position, std::pair<int, int> padding, std::pair<int, int> margin) {

}

HWND KeyWindow::GetHandle() {
    return hwnd;
}

void KeyWindow::WriteText(const std::wstring text) {

}
