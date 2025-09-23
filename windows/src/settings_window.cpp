#include "settings_window.h"

SettingsWindow::SettingsWindow(HINSTANCE hInstance, std::shared_ptr<KeyWindow> display) {
    loadSettings();
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

    FontProperties fontProperties(
        L"JetBrains Mono",
        16,
        Gdiplus::FontStyleBold
    );

    root->setSize({windowSize.first, windowSize.second})
        .setPosition({0, 0})
        .setPadding({24, 24})
        .setGap(8);
    
    std::shared_ptr<Text> ftl = std::make_shared<Text>();
    ftl->setFontProperties(fontProperties)
        .setText(L"Font")
        .setTextColor(Gdiplus::Color(0, 0, 0));
    root->addChild(ftl);

    std::shared_ptr<Box> ftc = std::make_shared<Box>();
    ftc->setBackgroundColor(Gdiplus::Color(255, 255, 255))
        .setBorderColor(Gdiplus::Color(0, 0, 0)) // for debugging purposes
        .setBorderWidth(2)
        .setSize({root->getWidth(), 48});
    root->addChild(ftc);

    std::shared_ptr<TextInput> fti = std::make_shared<TextInput>();
    ftc->addChild(fti);
    fti->init();
    // fti->setValidCharacters(L"0123456789");

    // fti->setFontProperties(fontProperties)
    //     .setText(std::to_wstring(appSettings.fontSize))
    //     .setTextColor(Gdiplus::Color(0, 0, 0));

    // fti->setOnSubmit([&, fti]() {
    //     int newFontSize = std::stoi(fti->getText());
    //     int clampedFontSize = std::clamp(newFontSize, 8, 40);
    //     appSettings.fontSize = clampedFontSize;
    //     // TODO: add display update function
    //     fti->setText(std::to_wstring(clampedFontSize));
    // });
    // fti->setOnFocus([ftic]() {
    //     MessageBoxA(NULL, "focus", "debug", MB_OK);
    //     ftic->setBorderColor(Gdiplus::Color(0, 0, 255));
    // });
    // fti->setOnBlur([ftic]() {
    //     MessageBoxA(NULL, "blur", "debug", MB_OK);
    //     ftic->setBorderColor(Gdiplus::Color(255, 0, 0));
    // });
    // ftic->addChild(fti);
    // fti->centerFromElement(ftic);

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

void SettingsWindow::loadSettings() {
    std::wstring settingsPath = resourceFinder->getResource(L"settings.json");
    std::string settingsPathNarrow(settingsPath.begin(), settingsPath.end());
    JsonValue res = jsonParser->parseFromFile(settingsPathNarrow);
    JsonObject obj = res.getObject();
    appSettings.fontSize = static_cast<int>(obj.at("fontSize").getNumber());
    appSettings.textBufferLength = static_cast<int>(obj.at("textBufferLength").getNumber());
    appSettings.inactiveTimeout = static_cast<int>(obj.at("inactiveTimeout").getNumber());
    appSettings.transparency = obj.at("transparency").getNumber();
    appSettings.textColor = hexToRgb(std::wstring(obj.at("textColor").getString().begin(), obj.at("textColor").getString().end()), RGB(255, 255, 255));
    appSettings.backgroundColor = hexToRgb(std::wstring(obj.at("backgroundColor").getString().begin(), obj.at("backgroundColor").getString().end()), RGB(0, 0, 0));
    JsonObject windowPos = obj.at("windowPosition").getObject();
    appSettings.windowPosition.first = static_cast<int>(windowPos.at("x").getNumber());
    appSettings.windowPosition.second = static_cast<int>(windowPos.at("y").getNumber());
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
            window->root->handler(hwnd, uMsg, wParam, lParam);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HBRUSH clearPaint = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &ps.rcPaint, clearPaint);
            DeleteObject(clearPaint);
            window->root->draw(hdc);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_CLOSE: {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void SettingsWindow::saveSettings() {

}