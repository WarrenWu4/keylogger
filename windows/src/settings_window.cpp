#include "settings_window.h"

SettingsWindow::SettingsWindow(HINSTANCE hInstance, std::shared_ptr<FontManager> fontManager) {
    this->fontManager = fontManager;
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

    root->setSize({windowSize.first, windowSize.second}).setPosition({0, 0}).setPadding({16, 16});

    std::shared_ptr<Box> display = std::make_shared<Box>();
    display->setBackgroundColor(RGB(0, 0, 0)).setBorderRadius(8).setSize({root->getSize().width, 80}).setPosition(root->getPosition());
    root->addChild(display);

    std::shared_ptr<Text> displayText = std::make_shared<Text>();
    displayText->setText(L"example text here").setFontSize(12).setTextColor(RGB(255, 255, 255)).setFont(font).centerFromElement(display);
    display->addChild(displayText);

    std::shared_ptr<Text> transparencyLabel = std::make_shared<Text>();
    transparencyLabel->setFont(font).setText(L"Opacity").setFontSize(12).setTextColor(RGB(0, 0, 0)).setPosition({root->getPosition().x, root->getLastChildBottom() + 20});
    root->addChild(transparencyLabel);

    std::shared_ptr<Slider> transparencySlider = std::make_shared<Slider>();
    transparencySlider->setValue(0).setSize({200, 16}).setPosition({root->getPosition().x, root->getLastChildBottom() + 8});
    transparencySlider->getLabel().setText(L"0%").setFont(font).setFontSize(12).setTextColor(RGB(0, 0, 0)).setPosition({transparencySlider->getRect().right + 6, transparencySlider->getPosition().y}).verticalCenterFromElement(transparencySlider);
    transparencySlider->getTrack().setBackgroundColor(RGB(200, 200, 200)).setBorderRadius(4).setBorderWidth(0).setSize({200, 4}).setPosition(root->getPosition()).verticalCenterFromElement(transparencySlider);
    transparencySlider->getThumb().setBackgroundColor(RGB(0, 0, 0)).setSize({16, 16}).setPosition({0, transparencySlider->getPosition().y});
    root->addChild(transparencySlider);

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

void SettingsWindow::SaveSettings() {

}

HWND SettingsWindow::GetHwnd() {
    return this->hwnd;
}
