#include "../include/system_tray.h"

SystemTray::SystemTray(HWND hwnd) {
    // notify icon data struct
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    // icon for system tray
    hIcon = (HICON)LoadIcon(NULL, MAKEINTRESOURCE(IDI_APP_ICON));
    nid.hIcon = (hIcon) ? hIcon : LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"Keylogger");
    Shell_NotifyIcon(NIM_ADD, &nid);
    // system tray menu
    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
}

SystemTray::~SystemTray() {
    Shell_NotifyIcon(NIM_DELETE, &nid);
    if (hIcon) {
        DestroyIcon(hIcon);
        hIcon = nullptr;
    }
    if (hMenu) {
        DestroyMenu(hMenu);
        hMenu = nullptr;
    }
}

HMENU SystemTray::getHMenu() {
    return hMenu;
}