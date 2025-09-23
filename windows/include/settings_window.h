#pragma once

#include <string>
#include <windows.h>
#include <utility>
#include <commctrl.h>
#include <tchar.h>
#include <locale>
#include <codecvt>
#include "ui.h"
#include "font_manager.h"
#include "display.h"
#include "resource_finder.h"
#include "jsonge.h"
#include "settings_manager.h"

#define IDC_EDIT 201
#define IDC_SPIN 202

class SettingsWindow {
private:
    HWND hwnd = nullptr;
    Settings appSettings;
    std::pair<int, int> windowSize = {600, 600};
    std::shared_ptr<Element> root = std::make_shared<Element>();

    std::shared_ptr<ResourceFinder> resourceFinder = std::make_shared<ResourceFinder>();
    std::shared_ptr<Json> jsonParser = std::make_shared<Json>();
    std::shared_ptr<KeyWindow> display = nullptr;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
    SettingsWindow(HINSTANCE hInstance, std::shared_ptr<KeyWindow> display);
    ~SettingsWindow();
    Settings getSettings() { return appSettings; }
    HWND GetHwnd() { return hwnd; }
};


