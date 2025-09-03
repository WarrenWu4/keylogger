#include "../include/display.h"
#include <winuser.h>

KeyWindow::KeyWindow(
    HINSTANCE hInstance,
    Vector2 size,
    Vector2 position,
    Vector2 padding,
    Vector2 margin
) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = KeyWindow::WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"KeyloggerClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE, L"Key Display", L"Keylogger", WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 40,
        NULL, NULL, hInstance, NULL
    );
    ShowWindow(hwnd, SW_SHOW);

    this->size = size;
    this->position = position;
    this->padding = padding;
    this->margin = margin;

    // setting window position
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

void KeyWindow::ResizeWindow(Vector2 newSize) {
    // calculate new position anchored at bottom right of screen
    Vector2 newPosition = {
        desktopRect.right - newSize.first - padding.first,
        desktopRect.bottom - newSize.second - padding.second
    };
    MoveWindow(hwnd, newPosition.first, newPosition.second, newSize.first, newSize.second, TRUE);
}

void KeyWindow::UpdateHWND(HWND hwnd) {
    this->hwnd = hwnd;
}

HWND KeyWindow::GetHWND() {
    return this->hwnd;
}

void KeyWindow::WriteText(const std::wstring text) {
    if (hwnd == NULL) { throw std::runtime_error("Window handle is NULL."); }
    // get size of text
    HDC hdc = GetDC(hwnd);
    if (hdc == NULL) { throw std::runtime_error("Failed to get device context."); }
    SIZE size;
    if (!GetTextExtentPoint32W(hdc, text.c_str(), text.length(), &size)) {
        ReleaseDC(hwnd, hdc);
        throw std::runtime_error("Failed to get text extent.");
    }
    ReleaseDC(hwnd, hdc);
    // calculate new size
    Vector2 newSize = {
        size.cx + padding.first * 2 + margin.first * 2,
        size.cy + padding.second * 2 + margin.second * 2
    };
    // resize window
    ResizeWindow(newSize);
}

LRESULT CALLBACK KeyWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}