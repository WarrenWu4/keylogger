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

    std::shared_ptr<FontManager> fontManager = nullptr;

    void LoadSettings(const std::wstring& path);
    void SaveSettings();
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
    SettingsWindow(HINSTANCE hInstance, std::shared_ptr<FontManager> fontManager);
    ~SettingsWindow();
    HWND GetHwnd();
};


