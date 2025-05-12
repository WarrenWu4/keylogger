#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <shellapi.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <deque>

#pragma comment(lib, "setupapi.lib")

// tray icon initialization and definitions
#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001
NOTIFYICONDATA nid = {};
HMENU hTrayMenu = NULL;
HICON hIcon = NULL;

// guid for keyboard interfaces
DEFINE_GUID(GUID_DEVINTERFACE_KEYBOARD,
    0x884b96c3, 0x56ef, 0x11d1,
    0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd);

// logging and important keyboard vars
std::wofstream logFile("keylogger.log", std::ios::app);
std::vector<PWSTR> keyboardDevicePaths;
std::deque<std::wstring> keyBuffer;
std::unordered_map<DWORD, std::wstring> vkeyToWString = {
    {0x01, L"Left Mouse Button"},
    {0x02, L"Right Mouse Button"},
    {0x03, L"Control-break processing"},
    {0x04, L"Middle Mouse Button"},
    {0x05, L"X1 Mouse Button"},
    {0x06, L"X2 Mouse Button"},
    {0x08, L"Backspace"},
    {0x09, L"Tab"},
    {0x0C, L"Clear"},
    {0x0D, L"Enter"},
    {0x10, L"Shift"},
    {0x11, L"Ctrl"},
    {0x12, L"Alt"},
    {0x13, L"Pause"},
    {0x14, L"Caps Lock"},
    {0x1B, L"Escape"},
    {0x20, L"Space"},
    {0x21, L"Page Up"},
    {0x22, L"Page Down"},
    {0x23, L"End"},
    {0x24, L"Home"},
    {0x25, L"Left Arrow"},
    {0x26, L"Up Arrow"},
    {0x27, L"Right Arrow"},
    {0x28, L"Down Arrow"},
    {0x2C, L"Print Screen"},
    {0x2D, L"Insert"},
    {0x2E, L"Delete"},
    {0x30, L"0"}, {0x31, L"1"}, {0x32, L"2"}, {0x33, L"3"}, {0x34, L"4"},
    {0x35, L"5"}, {0x36, L"6"}, {0x37, L"7"}, {0x38, L"8"}, {0x39, L"9"},
    {0x41, L"A"}, {0x42, L"B"}, {0x43, L"C"}, {0x44, L"D"}, {0x45, L"E"},
    {0x46, L"F"}, {0x47, L"G"}, {0x48, L"H"}, {0x49, L"I"}, {0x4A, L"J"},
    {0x4B, L"K"}, {0x4C, L"L"}, {0x4D, L"M"}, {0x4E, L"N"}, {0x4F, L"O"},
    {0x50, L"P"}, {0x51, L"Q"}, {0x52, L"R"}, {0x53, L"S"}, {0x54, L"T"},
    {0x55, L"U"}, {0x56, L"V"}, {0x57, L"W"}, {0x58, L"X"}, {0x59, L"Y"}, {0x5A, L"Z"},
    {0x5B, L"Left Windows"}, {0x5C, L"Right Windows"}, {0x5D, L"Applications"},
    {0x5F, L"Sleep"},
    {0x60, L"Numpad 0"}, {0x61, L"Numpad 1"}, {0x62, L"Numpad 2"}, {0x63, L"Numpad 3"},
    {0x64, L"Numpad 4"}, {0x65, L"Numpad 5"}, {0x66, L"Numpad 6"}, {0x67, L"Numpad 7"},
    {0x68, L"Numpad 8"}, {0x69, L"Numpad 9"}, {0x6A, L"Multiply"}, {0x6B, L"Add"},
    {0x6C, L"Separator"}, {0x6D, L"Subtract"}, {0x6E, L"Decimal"}, {0x6F, L"Divide"},
    {0x70, L"F1"}, {0x71, L"F2"}, {0x72, L"F3"}, {0x73, L"F4"}, {0x74, L"F5"},
    {0x75, L"F6"}, {0x76, L"F7"}, {0x77, L"F8"}, {0x78, L"F9"}, {0x79, L"F10"},
    {0x7A, L"F11"}, {0x7B, L"F12"}, {0x7C, L"F13"}, {0x7D, L"F14"}, {0x7E, L"F15"},
    {0x7F, L"F16"}, {0x80, L"F17"}, {0x81, L"F18"}, {0x82, L"F19"}, {0x83, L"F20"},
    {0x84, L"F21"}, {0x85, L"F22"}, {0x86, L"F23"}, {0x87, L"F24"},
    {0x90, L"Num Lock"}, {0x91, L"Scroll Lock"},
    {0xA0, L"Left Shift"}, {0xA1, L"Right Shift"}, {0xA2, L"Left Ctrl"},
    {0xA3, L"Right Ctrl"}, {0xA4, L"Left Alt"}, {0xA5, L"Right Alt"},
    {0xC0, L"~"}
};

std::wstring VKeyToWString(DWORD vkey) {
    auto it = vkeyToWString.find(vkey);
    if (it != vkeyToWString.end()) {
        return it->second;
    }
    return L" ";
}

void LogMessage(const std::wstring& message) {
    // flush to make sure message is written immediately
    logFile << message << std::endl;
    logFile.flush();
}

void AddTrayIcon(HWND hwnd) {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    hIcon = (HICON)LoadImage(NULL, L"logo.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    if (hIcon) {
        nid.hIcon = hIcon;
    } else {
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    }
    wcscpy_s(nid.szTip, L"Keylogger");
    Shell_NotifyIcon(NIM_ADD, &nid);
    hTrayMenu = CreatePopupMenu();
    AppendMenu(hTrayMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            if (hIcon) {
                DestroyIcon(hIcon);
            }
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            SetBkColor(hdc, RGB(0, 0, 0));
            SetTextColor(hdc, RGB(255, 255, 255));
            SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
            // write initial program message
            RECT rect;
            std::wstring keys = L"";
            for (const auto& key : keyBuffer) {
                keys += key + L"";
            }
            GetClientRect(hwnd, &rect);
            DrawText(hdc, keys.c_str(), -1, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_INPUT: {
            UINT dwSize = 0;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
            LPBYTE lpb = new BYTE[dwSize];
            if (!lpb) { return 0; }
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
                RAWINPUT* raw = (RAWINPUT*)lpb;
                if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                    if (!(raw->data.keyboard.Flags & RI_KEY_BREAK) && raw->data.keyboard.Message == WM_KEYDOWN) {
                        LogMessage(L"Key Pressed: " + VKeyToWString(raw->data.keyboard.VKey) + L" " + std::to_wstring(raw->data.keyboard.VKey));
                        keyBuffer.push_back(VKeyToWString(raw->data.keyboard.VKey));
                        if (keyBuffer.size() > 10) {
                            keyBuffer.pop_front();
                        }
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                }
            }
            delete[] lpb;
            return 0;
        }
        case WM_TRAYICON: {
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd); // Required for menu to work right
                TrackPopupMenu(hTrayMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
            }
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                Shell_NotifyIcon(NIM_DELETE, &nid);
                if (hTrayMenu) {
                    DestroyMenu(hTrayMenu);
                }
                PostQuitMessage(0);
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    LogMessage(L"Starting program");
    // register window class
    LogMessage(L"Registering window class");
    const wchar_t CLASS_NAME[] = L"KeyDisplay";
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Window class registration failed!", L"Error", MB_OK);
        return 1;
    }
    // create window
    LogMessage(L"Creating window");
    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_NOACTIVATE, CLASS_NAME, L"Keylogger", WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 40,
        NULL, NULL, hInstance, NULL
    );
    if (hwnd == NULL) {
        MessageBox(NULL, L"Window creation failed!", L"Error", MB_OK);
        return 1;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    // register raw input from keyboards
    LogMessage(L"Registering raw input");
    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = 0x01; // generic desktop controls
    rid[0].usUsage = 0x06; // keyboard
    rid[0].dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY; // receive input even when not in foreground
    rid[0].hwndTarget = hwnd;
    if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
        MessageBox(NULL, L"Failed to register raw input devices!", L"Error", MB_OK);
        return 1;
    }
    // add tray icon
    AddTrayIcon(hwnd);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 100, 600, 40, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    // message loop
    LogMessage(L"Entering message loop");
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
