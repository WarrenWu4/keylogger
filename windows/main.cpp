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
    // message loop
    LogMessage(L"Entering message loop");
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
