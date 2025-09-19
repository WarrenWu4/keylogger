#include "settings_window.h"

SettingsWindow::SettingsWindow(HINSTANCE hInstance, std::shared_ptr<FontManager> fontManager, std::shared_ptr<KeyWindow> display) {
    this->fontManager = fontManager;
    this->display = display;
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(icex);
    icex.dwICC = ICC_UPDOWN_CLASS;
    InitCommonControlsEx(&icex);

    WNDCLASS wc = { };
    wc.lpfnWndProc = SettingsWindow::WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Settings Class";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Failed to register settings window class!", L"Error", MB_OK);
        return;
    }

    // TODO: center screen
    hwnd = CreateWindowEx(
        0,
        L"Settings Class",
        L"Settings",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 
        windowSize.first, windowSize.second,
        NULL,
        NULL, 
        hInstance,
        this
    );
    if (!hwnd) {
        MessageBox(NULL, L"Failed to create settings window!", L"Error", MB_OK);
        return;
    }

    HFONT font = fontManager->getFont(L"JetBrains Mono", 12);

    root->setSize({windowSize.first, windowSize.second})
        .setPosition({0, 0})
        .setPadding({16, 16});

    std::shared_ptr<Text> transparencyLabel = std::make_shared<Text>();
    transparencyLabel->setFont(font)
        .setText(L"Opacity")
        .setFontSize(12)
        .setTextColor(RGB(0, 0, 0))
        .setPosition({root->getPosition().x, root->getLastChildBottom() + 20});
    root->addChild(transparencyLabel);

    std::shared_ptr<Slider> transparencySlider = std::make_shared<Slider>();
    transparencySlider->setValue(0).setSize({200, 16}).setPosition({root->getPosition().x, root->getLastChildBottom() + 8});
    transparencySlider->getLabel().setText(L"0%").setFont(font).setFontSize(12).setTextColor(RGB(0, 0, 0)).setPosition({transparencySlider->getRect().right + 6, transparencySlider->getPosition().y}).verticalCenterFromElement(transparencySlider);
    transparencySlider->getTrack().setBackgroundColor(RGB(200, 200, 200)).setBorderRadius(4).setBorderWidth(0).setSize({200, 4}).setPosition(root->getPosition()).verticalCenterFromElement(transparencySlider);
    transparencySlider->getThumb().setBackgroundColor(RGB(0, 0, 0)).setSize({16, 16}).setPosition({0, transparencySlider->getPosition().y});
    root->addChild(transparencySlider);

    std::shared_ptr<Text> ftl = std::make_shared<Text>();
    ftl->setFont(font)
        .setText(L"Font")
        .setFontSize(12)
        .setTextColor(RGB(0, 0, 0))
        .setPosition({root->getPosition().x, root->getLastChildBottom() + 20});
    root->addChild(ftl);

    std::shared_ptr<TextInput> fti = std::make_shared<TextInput>();
    fti->setValidCharacters(L"0123456789")
        .setSize({140, 32})
        .setPosition({root->getPosition().x, root->getLastChildBottom() + 8});
    fti->getContainer()->setBackgroundColor(RGB(255, 255, 255))
        .setBorderColor(RGB(0, 0, 0))
        .setBorderRadius(8)
        .setBorderWidth(2)
        .setSize({120, 36})
        .setPosition(fti->getPosition());
    fti->getLabel()->setFont(font)
        .setText(std::to_wstring(appSettings.fontSize))
        .setFontSize(12)
        .setPosition({fti->getPosition().x + 8, fti->getPosition().y})
        .verticalCenterFromElement(fti->getContainer());
    fti->setOnSubmit([&, fti]() {
        int newFontSize = std::stoi(fti->getLabel()->getText());
        int clampedFontSize = std::clamp(newFontSize, 8, 40);
        appSettings.fontSize = clampedFontSize;
        HFONT font = this->fontManager->createNewFont(clampedFontSize);
        this->display->setFont(font);
        InvalidateRect(this->display->getHwnd(), this->display->getRect(), TRUE);
        fti->getLabel()->setText(std::to_wstring(clampedFontSize));
    });
    root->addChild(fti);

    std::shared_ptr<Text> bgcl = std::make_shared<Text>();
    bgcl->setFont(font).setText(L"Background Color").setFontSize(12).setTextColor(RGB(0, 0, 0)).setPosition({root->getPosition().x, root->getLastChildBottom() + 20});
    root->addChild(bgcl);

    std::shared_ptr<TextInput> bgci = std::make_shared<TextInput>();
    bgci->setValidCharacters(L"0123456789abcdefABCDEF")
        .setMaxLength(7)
        .setMinLength(1)
        .setSize({140, 32})
        .setPosition({root->getPosition().x, root->getLastChildBottom() + 8});
    bgci->getContainer()->setBackgroundColor(RGB(255, 255, 255))
        .setBorderColor(RGB(0, 0, 0))
        .setBorderRadius(8)
        .setBorderWidth(2)
        .setSize({120, 36})
        .setPosition(bgci->getPosition());
    bgci->getLabel()->setFont(font)
        .setText(rgbToHex(appSettings.backgroundColor))
        .setFontSize(12)
        .verticalCenterFromElement(bgci->getContainer());

    std::shared_ptr<Box> bgcb = std::make_shared<Box>();
    bgcb->setBackgroundColor(appSettings.backgroundColor)
        .setBorderRadius(4)
        .setBorderColor(RGB(0, 0, 0))
        .setBorderWidth(2)
        .setSize({16, 16})
        .setPosition({bgci->getContainer()->getPosition().x + 8, 0})
        .verticalCenterFromElement(bgci->getContainer());
    bgci->getLabel()->setPosition({bgcb->getRect().right + 8, bgci->getLabel()->getPosition().y});
    std::function<COLORREF(std::wstring, COLORREF)> hexToRgbFunc = std::bind(&SettingsWindow::hexToRgb, this, std::placeholders::_1, std::placeholders::_2);
    std::function<std::wstring(COLORREF)> rgbToHexFunc = std::bind(&SettingsWindow::rgbToHex, this, std::placeholders::_1);
    bgci->setOnSubmit([&, bgcb, bgci, hexToRgbFunc, rgbToHexFunc](){
        COLORREF defaultColor = bgcb->getBackgroundColor();
        std::wstring hexText = bgci->getLabel()->getText();
        bgcb->setBackgroundColor(hexToRgbFunc(hexText, defaultColor));
        appSettings.backgroundColor = bgcb->getBackgroundColor();
        if (hexText.size() == 7 && hexText[0] == L'#') {
            bgci->getLabel()->setText(hexText);
        } else {
            bgci->getLabel()->setText(rgbToHexFunc(defaultColor));
        }
    });

    bgci->addChild(bgcb);
    root->addChild(bgci);

    std::shared_ptr<Text> fadeLabel = std::make_shared<Text>();
    fadeLabel->setFont(font).setText(L"Fade Duration").setFontSize(12).setTextColor(RGB(0, 0, 0)).setPosition({root->getPosition().x, root->getLastChildBottom() + 20});
    root->addChild(fadeLabel);

    std::vector<std::wstring> fadeOptions = {L"1s", L"2s", L"3s", L"5s", L"10s", L"Never"};
    std::shared_ptr<Dropdown> fadeDropdown = std::make_shared<Dropdown>();
    fadeDropdown->setFont(font).setOptions(fadeOptions).setSelectedIndex(3).setSize({120, 32}).setPosition({root->getPosition().x, root->getLastChildBottom() + 8});
    fadeDropdown->getSelectedContainer()->setBackgroundColor(RGB(255, 255, 255)).setBorderColor(RGB(0, 0, 0)).setBorderRadius(8).setBorderWidth(2).setSize({120, 36}).setPosition(fadeDropdown->getPosition());
    fadeDropdown->getSelectedText()->setFont(font).setFontSize(12).setTextColor(RGB(0, 0, 0)).centerFromElement(fadeDropdown->getSelectedContainer());
    fadeDropdown->getOptionsContainer()->setBackgroundColor(RGB(255, 255, 255)).setBorderColor(RGB(0, 0, 0)).setBorderRadius(8).setBorderWidth(2).setSize({120, static_cast<int>(fadeOptions.size()) * 36}).setPosition({fadeDropdown->getPosition().x, fadeDropdown->getSelectedContainer()->getRect().bottom + 4});
    root->addChild(fadeDropdown);

    std::shared_ptr<Text> positionLabel = std::make_shared<Text>();
    positionLabel->setFont(font)
        .setText(L"Position")
        .setFontSize(12)
        .setTextColor(RGB(0, 0, 0))
        .setPosition({root->getPosition().x, root->getLastChildBottom() + 20});
    root->addChild(positionLabel);

    std::shared_ptr<Text> positionDescription = std::make_shared<Text>();
    positionDescription->setFont(fontManager->getFont(L"JetBrains Mono", 8))
        .setText(L"To adjust position, click and drag the display. Position will be automatically saved between sessions.")
        .setTextAlignment(DT_WORDBREAK | DT_LEFT)
        .setFontSize(12)
        .setTextColor(RGB(0, 0, 0));
    positionDescription->setPosition({root->getPosition().x, root->getLastChildBottom() + 8})
        .setSize({root->getSize().width, positionDescription->getSize().height*2});
    root->addChild(positionDescription);
}

COLORREF SettingsWindow::hexToRgb(std::wstring hex, COLORREF defaultColor) {
    if (hex.size() != 7 || hex[0] != L'#') { return defaultColor; }
    for (size_t i = 1; i < hex.size(); i++) { // !
        if (!iswxdigit(hex[i])) { return defaultColor; }
    }
    return RGB(
        std::stoi(hex.substr(1, 2), nullptr, 16),
        std::stoi(hex.substr(3, 2), nullptr, 16),
        std::stoi(hex.substr(5, 2), nullptr, 16)
    );
}

std::wstring SettingsWindow::rgbToHex(COLORREF color) {
    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);
    wchar_t buffer[8];
    swprintf(buffer, 8, L"#%02X%02X%02X", r, g, b);
    return std::wstring(buffer);
}

SettingsWindow::~SettingsWindow() {
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
}

void SettingsWindow::LoadSettings(const std::wstring& path = L"../resources/settings.json") {

}

LRESULT CALLBACK SettingsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    SettingsWindow* window = nullptr;
    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT *pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<SettingsWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->hwnd = hwnd;
    } else {
        window = reinterpret_cast<SettingsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
    switch (uMsg) {
        case WM_COMMAND: {
            if (LOWORD(wParam) == IDC_EDIT) {
                if (HIWORD(wParam) == EN_CHANGE) {
                    TCHAR buffer[32];
                    GetWindowText((HWND)lParam, buffer, 32);
                    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                    std::string narrowString = converter.to_bytes(buffer);
                }
                break;
            }
        }
        case WM_CHAR:
        case WM_MOUSEMOVE:
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN: {
            window->root->handlerStart(hwnd, uMsg, wParam, lParam);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HBRUSH clearPaint = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &ps.rcPaint, clearPaint);
            DeleteObject(clearPaint);
            window->root->drawStart(hdc);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY: {
            ShowWindow(hwnd, SW_HIDE);
            break;
        }
        case WM_CLOSE: {
            ShowWindow(hwnd, SW_HIDE);
            break;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void SettingsWindow::saveSettings() {

}