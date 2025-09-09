#include "settings_window.h"

SettingsWindow::SettingsWindow(HINSTANCE hInstance) {
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
        // logger.write("Failed to register settings window class!\n");
        // logger.flush();
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
        // logger.write("Failed to create settings window!\n");
        // logger.flush();
        MessageBox(NULL, L"Failed to create settings window!", L"Error", MB_OK);
        return;
    }
    // logger.write("Successfully created settings window\n");
    // logger.flush();
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
        case WM_CREATE: {
            CreateWindowEx(
                WS_EX_CLIENTEDGE, L"EDIT", L"16",
                WS_CHILD | WS_VISIBLE | ES_NUMBER,
                16, 48, 100, 25,
                hwnd, (HMENU)IDC_EDIT, GetModuleHandle(NULL), NULL);
            CreateWindowEx(
                WS_EX_CLIENTEDGE, L"EDIT", L"20",
                WS_CHILD | WS_VISIBLE | ES_NUMBER,
                16, 98, 100, 25,
                hwnd, (HMENU)IDC_EDIT, GetModuleHandle(NULL), NULL);
            CreateWindowEx(
                WS_EX_CLIENTEDGE, L"EDIT", L"1.0",
                WS_CHILD | WS_VISIBLE | ES_NUMBER,
                16, 148, 100, 25,
                hwnd, (HMENU)IDC_EDIT, GetModuleHandle(NULL), NULL);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            window->DrawWindow(hdc);
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

void SettingsWindow::DrawWindow(HDC hdc) {
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    RECT paddingRect = {
        padding.first, 
        padding.second,
        windowSize.first - padding.first * 2,
        40
    };
    std::wstring text = L"Font Size (8 - 40)";
    DrawTextW(hdc, text.c_str(), -1, &paddingRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    std::wstring text2 = L"Text Display Length (5 - 40)";
    RECT text2Rect = {
        padding.first,
        padding.second + 50,
        paddingRect.right,
        paddingRect.bottom + 50};
    DrawTextW(hdc, text2.c_str(), -1, &text2Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    RECT text3Rect = {
        padding.first,
        padding.second + 100,
        paddingRect.right,
        paddingRect.bottom + 100};
    std::wstring text3 = L"Transparency (0.1 - 1.0)";
    DrawTextW(hdc, text3.c_str(), -1, &text3Rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

HWND SettingsWindow::GetHwnd() {
    return this->hwnd;
}
