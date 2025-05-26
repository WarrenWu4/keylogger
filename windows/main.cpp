#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <shellapi.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <vector>
#include <deque>

// user created files
#include "include/vkey.h"
#include "include/display.h"
#include "include/logging.h"

// helper classes
Logger errorLog(L"error.log");
FixedSizeLogger keyLog(L"keys.log", 10240, 1024);

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

// keyboard vars
std::vector<PWSTR> keyboardDevicePaths;
std::deque<std::wstring> keyBuffer;

// window variables (fonts, etc.)
HFONT hFont = CreateFontW(
    28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
);

void AddTrayIcon(HWND hwnd) {
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    hIcon = (HICON)LoadImage(NULL, L"resources/logo.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
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
        case WM_DESTROY: {
            Shell_NotifyIcon(NIM_DELETE, &nid);
            if (hIcon) {
                DestroyIcon(hIcon);
            }
            PostQuitMessage(0);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            SetBkColor(hdc, RGB(0, 0, 0));
            SetTextColor(hdc, RGB(255, 255, 255));
            // SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
            HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
            // write initial program message
            RECT rect;
            std::wstring keys = L"";
            for (const auto& key : keyBuffer) {
                keys += key + L"";
            }
            GetClientRect(hwnd, &rect);
            DrawText(hdc, keys.c_str(), -1, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
            SelectObject(hdc, oldFont);
            DeleteObject(hFont);
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
                        keyLog.addRecord(&raw->data.keyboard.VKey);
                        keyBuffer.push_back(GetKeyNameFromVkey(raw->data.keyboard.VKey));
                        if (keyBuffer.size() > 60) {
                            keyBuffer.pop_front();
                        }
                        InvalidateRect(hwnd, NULL, TRUE);
                        ResizeWindowToText(hwnd, hFont, keyBuffer);
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
    /*
    // register window class
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
    // set windowp position
    RECT desktopRect;
    if (!GetWindowRect(GetDesktopWindow(), &desktopRect)) {
        MessageBox(NULL, L"Failed to get desktop window rect!", L"Error", MB_OK);
        return 1;
    }
    int padding = 64;
    int windowWidth = 120;
    int windowHeight = 48;
    int x = desktopRect.right - windowWidth - padding;
    int y = desktopRect.bottom - windowHeight - padding;
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, windowWidth, windowHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    // message loop
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    */

    return 0;
}
