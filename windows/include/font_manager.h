#pragma once

#include <windows.h>
#include <functional>
#include <unordered_map>
#include "resource.h"

class FontManager {
private:
    HFONT defaultFont = nullptr;
    HANDLE defaultFontRes = nullptr;
    std::unordered_map<int, HFONT> fonts;
public:
    FontManager(HINSTANCE hInstance, HWND hwnd);
    ~FontManager();
    HFONT KCreateFont(int fontSize);
    HFONT GetFont(int fontId);
};