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
    container->setBackgroundColor(Gdiplus::Color(0, 0, 0))
        .setBorderColor(Gdiplus::Color(255, 0, 0))
        .setBorderRadius(8)
        .setPosition({0, 0})
        .setSize({windowSize.x, windowSize.y});
    std::shared_ptr<Text> textElement = std::make_shared<Text>();
    textElement->setFontProperties({
            L"JetBrains Mono",
            16,
            Gdiplus::FontStyleBold
        })
        .setTextColor(Gdiplus::Color(255, 255, 255))
        .setText(L"Testing")
        .setPosition({0, 0})
        .setSize({windowSize.x, windowSize.y});
    container->addChild(textElement);
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
    // COLORREF textColor = RGB(255, 255, 255);
    // SetTextColor(hdc, textColor);
    // SetBkColor(hdc, TRANSPARENT);
    // HFONT oldFont = (HFONT)SelectObject(hdc, font);
    // DrawTextW(hdc, this->text.c_str(), -1, &rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
    // SelectObject(hdc, oldFont);
    container->draw(hdc);
}


void KeyWindow::setText(std::wstring const& newText) {
    this->text = newText;
}

RECT* KeyWindow::getRect() {
    return static_cast<RECT*>(&rect);
}
