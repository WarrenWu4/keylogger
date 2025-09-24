#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <shellapi.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <gdiplus.h>
#include <vector>
#include <utility>
#include <queue>

#include "vkey.h"
#include "display.h"
#include "system_tray.h"
#include "settings_window.h"
#include "gdiplus_context.h"
#include "settings_manager.h"

#define IDT_INACTIVE_TIMER 1

std::unique_ptr<GdiplusContext> gdiplusContext = nullptr;
std::unique_ptr<SettingsManager> settingsManager = nullptr;
std::shared_ptr<KeyWindow> display = nullptr;
std::shared_ptr<SystemTray> tray = nullptr;

HHOOK hKeyboardHook = nullptr;
std::wstring textBuffer = L"";

void cleanup() {
    if (hKeyboardHook) {
        UnhookWindowsHookEx(hKeyboardHook);
        hKeyboardHook = nullptr;
    }
    if (display) {
        KillTimer(display->getHwnd(), IDT_INACTIVE_TIMER);
    }
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT *pKeyStruct = (KBDLLHOOKSTRUCT *)lParam;
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            UINT vKey = pKeyStruct->vkCode;
            UINT scanCode = pKeyStruct->scanCode;
            BYTE keyboardState[256] = {0};
            keyboardState[VK_SHIFT] = (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? 0x80 : 0;
            textBuffer += getStrFromVKey(vKey, scanCode, keyboardState); 
            if (textBuffer.size() > static_cast<size_t>(settingsManager->getSettings()->textBufferLength)) {
                textBuffer.erase(0, textBuffer.size() - settingsManager->getSettings()->textBufferLength);
            }
            display->setText(textBuffer);
            ShowWindow(display->getHwnd(), SW_SHOW);
            // InvalidateRect(display->getHwnd(), NULL, TRUE);
            // PostMessage(display->getHwnd(), WM_PAINT, 0, 0);
            if (settingsManager->getSettings()->inactiveTimeout > 0) {
                KillTimer(display->getHwnd(), IDT_INACTIVE_TIMER);
                SetTimer(
                    display->getHwnd(), 
                    IDT_INACTIVE_TIMER,
                    settingsManager->getSettings()->inactiveTimeout * 1000,
                    NULL
                );
            }
        }
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TIMER: {
            if (wParam == IDT_INACTIVE_TIMER) {
                textBuffer = L"";
                display->setText(textBuffer);
                ShowWindow(display->getHwnd(), SW_HIDE);
                KillTimer(display->getHwnd(), IDT_INACTIVE_TIMER);
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            ScreenToClient(display->getHwnd(), &cursorPos);
            RECT temp = display->getRect();
            if (PtInRect(&temp, cursorPos)) {
                display->dragging = true;
                SetCapture(display->getHwnd());
            }
            break;
        }
        case WM_MOUSEMOVE: {
            if (display->dragging) {
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                display->updateWindowPos(cursorPos.x, cursorPos.y);
            }
            break;
        }
        case WM_LBUTTONUP: {
            if (display->dragging) {
                display->dragging = false;
                ReleaseCapture();
            }
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            display->drawText(hdc); 
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_TRAYICON: {
            if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONDOWN) {
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd); // Required for menu to work right
                TrackPopupMenu(tray->getHMenu(), TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
            }
            break;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_TRAY_EXIT) {
                PostQuitMessage(0);
                break;
            } else if (LOWORD(wParam) == ID_TRAY_SETTINGS) {
                ShowWindow(settingsWindow->GetHwnd(), SW_SHOW);
                break;
            }
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    gdiplusContext = std::make_unique<GdiplusContext>();
    settingsManager = std::make_unique<SettingsManager>();

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Key Display";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Failed to register window class!", L"Error", MB_OK);
        cleanup();
        return 1;
    }

    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    if (!hKeyboardHook) {
        MessageBox(NULL, L"Failed to set keyboard hook!", L"Error", MB_OK);
        cleanup();
        return 1;
    }

    display = std::make_shared<KeyWindow>(hInstance, settingsManager->getSettings());
    tray = std::make_shared<SystemTray>(hInstance, display->getHwnd());
    ShowWindow(display->getHwnd(), SW_SHOW);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    cleanup();
    return 0;
}
