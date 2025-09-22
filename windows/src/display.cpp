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
        .setPadding({8, 8})
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
    // TODO: seems like this seems very expensive, maybe find a better optimization later
    if (text.empty()) { return; }
    textEl->setText(this->text);
    boxEl->fitToChildren();
    boxEl->draw(hdc);
    MoveWindow(hwnd, windowOffset.x, windowOffset.y, boxEl->getSize().width, boxEl->getSize().height, TRUE);
}


void KeyWindow::setText(std::wstring const& newText) {
    this->text = newText;
}

RECT* KeyWindow::getRect() {
    return static_cast<RECT*>(&rect);
}

void KeyWindow::updateWindowPos(int x, int y) {
    windowOffset.x = x;
    windowOffset.y = y;
    MoveWindow(hwnd, windowOffset.x, windowOffset.y, boxEl->getSize().width, boxEl->getSize().height, TRUE);
}
