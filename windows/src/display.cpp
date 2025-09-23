#include "display.h"

KeyWindow::KeyWindow(HINSTANCE hInstance, std::shared_ptr<Settings> appSettings) {
    this->appSettings = appSettings;

    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE, 
        L"Key Display", 
        L"Keylogger", 
        WS_POPUP,
        0, 0, // position
        0, 0, // size (width, height)
        NULL, NULL, hInstance, NULL
    );

    boxEl->setBackgroundColor(appSettings->backgroundColor)
        .setPadding(appSettings->windowPadding);

    FontProperties fontProperties = {
        appSettings->fontFamily,
        appSettings->fontSize,
        Gdiplus::FontStyleBold
    };

    textEl->setFontProperties(fontProperties)
        .setTextColor(appSettings->textColor);
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
    // logger.write("Drawing text to window (" + std::to_string(this->appSettings->windowPosition.x) + ", " + std::to_string(this->appSettings->windowPosition.y) + ")\n");
    if (text.empty()) { return; }
    textEl->setText(this->text);
    boxEl->fitToChildren();
    boxEl->draw(hdc);
    MoveWindow(
        hwnd, 
        this->appSettings->windowPosition.x, 
        this->appSettings->windowPosition.y, 
        boxEl->getSize().width, 
        boxEl->getSize().height, 
        TRUE
    );
}


void KeyWindow::setText(std::wstring const& newText) {
    this->text = newText;
}

RECT KeyWindow::getRect() {
    return boxEl->getConvertedRect();
}

void KeyWindow::updateWindowPos(int x, int y) {
    // logger.write("Updating window position to (" + std::to_string(x) + ", " + std::to_string(y) + ")\n");
    this->appSettings->windowPosition.x = x;
    this->appSettings->windowPosition.y = y;
    MoveWindow(
        hwnd, 
        x,
        y,
        boxEl->getSize().width, 
        boxEl->getSize().height, 
        TRUE
    );
}
