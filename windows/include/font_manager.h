#pragma once

#include <windows.h>
#include <functional>
#include <unordered_map>
#include <string>
#include "resource.h"

class FontManager {
private:
    HFONT defaultFont = nullptr;
    HANDLE defaultFontRes = nullptr;

    std::unordered_map<std::wstring, std::unordered_map<int, HFONT>> fonts;
public:
    FontManager(HINSTANCE hInstance, HWND hwnd);
    ~FontManager();
    HFONT getFont(std::wstring fontName, int fontSize);
    HFONT createNewFont(int fontSize);
};