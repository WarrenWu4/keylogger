#include "display.h"

KeyWindow::KeyWindow(HINSTANCE hInstance) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    windowSize = {450, 40};
    windowOffset = {
        screenWidth-windowSize.x-40,
        (int)(screenHeight*0.8)-windowSize.y
    };

    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE, 
        L"Key Display", 
        L"Keylogger", 
        WS_POPUP,
        windowOffset.x, windowOffset.y, // position
        windowSize.x, windowSize.y, // size (width, height)
        NULL, NULL, hInstance, NULL
    );
    GetClientRect(hwnd, &rect); // {0, 0, windowSize.x, windowSize.y}

    boxEl->setBackgroundColor(Gdiplus::Color(0, 0, 0))
        .setPosition({0, 0})
        .setSize({windowSize.x, windowSize.y});

    fontProperties = {
        L"JetBrains Mono",
        20,
        Gdiplus::FontStyleBold
    };

    textEl->setFontProperties(fontProperties)
        .setTextColor(Gdiplus::Color(255, 255, 255))
        .setPosition({0, 0})
        .setSize({windowSize.x, windowSize.y});
    boxEl->addChild(textEl);
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
    textEl->setText(this->text);
    boxEl->draw(hdc);
}


void KeyWindow::setText(std::wstring const& newText) {
    this->text = newText;
}

RECT* KeyWindow::getRect() {
    return static_cast<RECT*>(&rect);
}
