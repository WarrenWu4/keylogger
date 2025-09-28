#include "system_tray.h"

SystemTray::SystemTray(HINSTANCE hInstance, HWND hwnd) {
    // notify icon data struct
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    // icon for system tray
    hIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    nid.hIcon = (hIcon) ? hIcon : LoadIcon(hInstance, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"Keylogger");
    Shell_NotifyIcon(NIM_ADD, &nid);
    // system tray menu
    hMenu = CreatePopupMenu();
    // ! temp disable settings since it's not implemented yet
    AppendMenu(hMenu, MF_STRING, ID_TRAY_SETTINGS, L"Settings");
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