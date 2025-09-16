#include "settings_window.h"

SettingsWindow::SettingsWindow(HINSTANCE hInstance, HFONT font) {
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

    root->setSize({windowSize.first, windowSize.second}).setPosition({0, 0}).setPadding({16, 16});

    std::shared_ptr<Box> display = std::make_shared<Box>();
    display->setBackgroundColor(RGB(0, 0, 0)).setBorderRadius(16).setSize({root->getSize().width, 80}).setPosition(root->getPosition());
    std::shared_ptr<Text> displayText = std::make_shared<Text>();
    displayText->setText(L"example text here").setFontSize(12).setTextColor(RGB(255, 255, 255)).setFont(font).centerFromElement(display);
    display->addChild(displayText);

    root->addChild(display);
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
            // window->root->handlerStart(hwnd, uMsg, wParam, lParam);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
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
