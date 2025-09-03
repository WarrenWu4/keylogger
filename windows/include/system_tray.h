#pragma once

#include <windows.h>
#include "resource.h"

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

class SystemTray {
    private:
        NOTIFYICONDATA nid = {};
        HICON hIcon = nullptr;
        HMENU hMenu = nullptr;
    public:
        SystemTray(HWND hwnd);
        ~SystemTray();
        HMENU getHMenu();
};