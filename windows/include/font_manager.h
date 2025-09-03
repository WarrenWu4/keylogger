#pragma once

#include <windows.h>
#include <functional>
#include "resource.h"

class FontManager {
private:
    HFONT hFont = nullptr;
    HANDLE hFontRes = nullptr;
public:
    FontManager(HINSTANCE hInstance, HWND hwnd);
    ~FontManager();
    HFONT getFont();
};