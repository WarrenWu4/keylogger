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

    std::vector<std::pair<std::wstring, std::shared_ptr<Element>>> sections {
        {L"Transparency", std::make_shared<Slider>(RECT{0, 0, 200, 16}, 0)},
        {L"Font", nullptr},
        {L"Background Color", nullptr},
        {L"Fade Duration", nullptr},
        {L"Position", nullptr}
    };
    root = std::make_shared<Box>(RECT{0, 0, windowSize.first, windowSize.second}, RGB(24, 24, 37), 0);
    std::shared_ptr<Element> paddingContainer = std::make_shared<PaddingContainer>(RECT{0, 0, windowSize.first, windowSize.second}, 16, 16);
    root->children.push_back(paddingContainer);
    std::shared_ptr<Element> flexContainer = std::make_shared<FlexContainer>(paddingContainer->rect, VERTICAL, 20, 20);
    paddingContainer->children.push_back(flexContainer);
    std::shared_ptr<Element> display = std::make_shared<Box>(RECT{
        flexContainer->rect.left, 
        flexContainer->rect.top, 
        flexContainer->rect.right, 
        flexContainer->rect.top + 80
    }, RGB(0, 0, 0), 8);
    flexContainer->children.push_back(display);
    std::shared_ptr<Element> transparencySlider = std::make_shared<Slider>(RECT{
        flexContainer->rect.left, 
        flexContainer->rect.top, 
        flexContainer->rect.left + 200, 
        flexContainer->rect.top + 16
    }, 0);
    flexContainer->children.push_back(
        std::make_shared<Button>(RECT{
            flexContainer->rect.left, 
            flexContainer->rect.top, 
            flexContainer->rect.left + 100, 
            flexContainer->rect.top + 32
        }, 
        L"Test Button", RGB(255, 255, 255), RGB(70, 70, 90), [](){
            MessageBox(NULL, L"Button clicked!", L"Debug", MB_OK);
    }));
    // for (std::pair<std::wstring, std::shared_ptr<Element>> section : sections) {
    //     flexContainer->children.push_back(
    //         std::make_shared<Text>(RECT{
    //             flexContainer->rect.left,
    //             flexContainer->rect.top, 
    //             flexContainer->rect.right, 
    //             flexContainer->rect.top+24
    //         }, section.first, 16, RGB(255, 255, 255), TRANSPARENT, font)
    //     );
    //     if (section.second != nullptr) {
    //         flexContainer->children.push_back(section.second);
    //     }
    // }
}

SettingsWindow::~SettingsWindow() {
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
}

void SettingsWindow::LoadSettings(const std::wstring& path = L"../resources/settings.json") {

}

void Draw() {

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
        case WM_LBUTTONDOWN: {
            window->root->handlerStart(hwnd, uMsg, wParam, lParam);
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
