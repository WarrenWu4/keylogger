#include "display.h"

KeyWindow::KeyWindow(HINSTANCE hInstance, std::shared_ptr<Settings> appSettings) {
    this->appSettings = appSettings;

    hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE, 
        L"Key Display", 
        L"Keylogger", 
        WS_POPUP,
        0, 0, // position
        450, 60, // size (width, height)
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
    // RECT rect = RECT{0, 0, 450, 100};
    // SetTextColor(hdc, RGB(255, 255, 255));
    // SetBkColor(hdc, RGB(0, 0, 0));
    // HFONT oldFont = (HFONT)SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
    // HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
    // FillRect(hdc, &rect, brush);
    // SetLayeredWindowAttributes(hwnd, RGB(255, 0, 0), (BYTE)(255), LWA_COLORKEY | LWA_ALPHA);
    // DrawTextW(hdc, this->text.c_str(), -1, &rect, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
    // SelectObject(hdc, oldFont);
    // DeleteObject(brush);
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
