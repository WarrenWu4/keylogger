#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <functional>
#include "jsonge.h"
#include "resource_finder.h"

struct Settings {
    int fontSize = 16;
    std::wstring fontFamily = L"JetBrains Mono";
    Gdiplus::Color textColor = Gdiplus::Color(255, 255, 255);
    int textBufferLength = 10;
    Gdiplus::Color backgroundColor = Gdiplus::Color(0, 0, 0);
    int inactiveTimeout = -1; // -1 for never
    POINT windowPosition = {0, 0};
    POINT windowPadding = {8, 8};
};

class SettingsManager {
private:
    std::shared_ptr<Settings> appSettings = std::make_shared<Settings>();

    std::shared_ptr<ResourceFinder> resourceFinder = std::make_shared<ResourceFinder>();
    std::shared_ptr<Json> jsonParser = std::make_shared<Json>();

    std::string stringToWstring(const std::wstring& wstr);
    std::wstring wstringToString(const std::string& str);
    Gdiplus::Color hexToArgb(std::wstring hex, Gdiplus::Color defaultColor = Gdiplus::Color(0, 0, 0));
    std::wstring argbToHex(Gdiplus::Color color);

public:
    SettingsManager();
    void saveSettings();

    std::shared_ptr<Settings> getSettings() { return appSettings; }
};