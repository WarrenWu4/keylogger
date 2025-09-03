#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <shellapi.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <vector>
#include <utility>
#include <queue>

// user created files
#include "include/vkey.h"
#include "include/display.h"
#include "include/logging.h"
#include "include/system_tray.h"

typedef std::pair<int, int> Vector2;

#pragma comment(lib, "setupapi.lib")

// helper classes
KeyWindow* display = nullptr;
SystemTray* tray = nullptr;
Logger errorLog(L"error.log");
FixedSizeLogger keyLog(L"keys.log", 10240);
FixedSizeLogger debugLog(L"debug.log", 10240);


// guid for keyboard interfaces
DEFINE_GUID(GUID_DEVINTERFACE_KEYBOARD,
    0x884b96c3, 0x56ef, 0x11d1,
    0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd);

// keyboard vars
std::vector<PWSTR> keyboardDevicePaths;
std::queue<std::wstring> keyStrokes = {};

// window variables (fonts, etc.)
HFONT hFont = CreateFontW(
    36, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
);

void cleanup() {
    if (display) {
        delete display;
        display = nullptr;
    }
    if (tray) {
        delete tray;
        tray = nullptr;
    }
}

std::wstring KeysStringFromStrokes(const std::queue<std::wstring>& keyStrokes) {
    std::wstring result;
    std::queue<std::wstring> tempQueue = keyStrokes; // copy to avoid modifying original queue
    while (!tempQueue.empty()) {
        result += tempQueue.front();
        tempQueue.pop();
    }
    return result;
}

void DrawKeyStrokes(HDC hdc, int xStart, int yStart) {
    int x = xStart;
    std::queue<std::wstring> keyStrokesCopy = keyStrokes; // copy to avoid modifying original queue
    while (!keyStrokesCopy.empty()) {
        std::wstring key = keyStrokesCopy.front();
        keyStrokesCopy.pop();
        SIZE size;
        GetTextExtentPoint32W(hdc, key.c_str(), static_cast<int>(key.length()), &size);

        TextOutW(hdc, x, yStart, key.c_str(), static_cast<int>(key.length()));
        x += size.cx + 10;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            SetBkColor(hdc, RGB(0, 0, 0));
            SetTextColor(hdc, RGB(255, 255, 255));
            HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
            RECT rect;
            GetClientRect(hwnd, &rect);
            std::wstring keyString = KeysStringFromStrokes(keyStrokes);
            DrawText(hdc, keyString.c_str(), -1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            // DrawKeyStrokes(hdc, rect.left + 10, rect.top + 10);
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
                        keyLog.write(L"Key pressed: " + std::to_wstring(raw->data.keyboard.VKey) + L"\n");
                        keyStrokes.push(GetKeyNameFromVkey(raw->data.keyboard.VKey));
                        if (keyStrokes.size() > 10) {
                            keyStrokes.pop(); // remove oldest key if more than 10 keys
                        }
                        /*
                        InvalidateRect(hwnd, NULL, TRUE);
                        if (display.GetHWND() != NULL) {
                            display.WriteText(KeysStringFromStrokes(keyStrokes).c_str());
                        }
                        */
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
                TrackPopupMenu(tray->getHMenu(), TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
            }
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                PostQuitMessage(0);
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Key Display";
    if (!RegisterClass(&wc)) {
        errorLog.LogMessageToFile(L"Window class registration failed.");
        cleanup();
        return 1;
    }

    display = new KeyWindow(
        hInstance,
        Vector2(120, 48), // size
        Vector2(0, 0), // position
        Vector2(60, 60), // padding
        Vector2(10, 10) // margin
    );
    ShowWindow(display->GetHWND(), SW_SHOW);
    UpdateWindow(display->GetHWND());
    tray = new SystemTray(display->GetHWND());

    // update and show window
    // register raw input from keyboards
    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = 0x01; // generic desktop controls
    rid[0].usUsage = 0x06; // keyboard
    rid[0].dwFlags = RIDEV_INPUTSINK | RIDEV_DEVNOTIFY; // receive input even when not in foreground
    rid[0].hwndTarget = display->GetHWND();
    if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0]))) {
        MessageBox(NULL, L"Failed to register raw input devices!", L"Error", MB_OK);
        return 1;
    }

    // message loop
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    cleanup();
    return 0;
}
