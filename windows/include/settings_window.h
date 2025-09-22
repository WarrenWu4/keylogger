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

#define IDC_EDIT 201
#define IDC_SPIN 202

struct Settings {
    int fontSize = 16;
    int textBufferLength = 20;
    int inactiveTimeout = -1; // -1 for never
    double transparency = 1.0;
    COLORREF textColor = RGB(255, 255, 255);
    COLORREF backgroundColor = RGB(0, 0, 0);
    std::pair<int, int> windowPosition = {0, 0};
};

class SettingsWindow {
private:
    HWND hwnd = nullptr;
    Settings appSettings;
    std::pair<int, int> windowSize = {600, 600};
    std::shared_ptr<Element> root = std::make_shared<Element>();

    std::shared_ptr<ResourceFinder> resourceFinder = std::make_shared<ResourceFinder>();
    std::shared_ptr<Json> jsonParser = std::make_shared<Json>();
    std::shared_ptr<KeyWindow> display = nullptr;

    void loadSettings();
    void saveSettings();
    // void updateSettings(const std::function<void(Settings&)>& appSettings) { updateSettings(appSettings); saveSettings(); }
    
    COLORREF hexToRgb(std::wstring hex, COLORREF defaultColor = RGB(0, 0, 0));
    std::wstring rgbToHex(COLORREF color);

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
    SettingsWindow(HINSTANCE hInstance, std::shared_ptr<KeyWindow> display);
    ~SettingsWindow();
    Settings getSettings() { return appSettings; }
    HWND GetHwnd() { return hwnd; }
};


