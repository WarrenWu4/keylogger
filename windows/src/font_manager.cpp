#include "font_manager.h"

FontManager::FontManager(HINSTANCE hInstance, HWND hwnd) {
    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(IDI_APP_FONT), RT_FONT);
    HGLOBAL hMem = LoadResource(hInstance, hRes);
    void* pFontData = LockResource(hMem);
    DWORD fontSize = SizeofResource(hInstance, hRes);
    DWORD nFonts = 0;
    defaultFontRes = AddFontMemResourceEx(pFontData, fontSize, NULL, &nFonts);
    HDC hdc = GetDC(hwnd);
    int logPixelsY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    int fontHeight = -MulDiv(
        16, // actual font size
        logPixelsY, 
        72
    );
    defaultFont = CreateFont(
        fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_MODERN, L"JetBrains Mono"
    );
    fonts[0] = defaultFont;
    fonts[1] = FontManager::KCreateFont(12);
}

FontManager::~FontManager() {
    if (defaultFont) {
        DeleteObject(defaultFont);
        defaultFont = nullptr;
    }
    if (defaultFontRes) {
        RemoveFontMemResourceEx(defaultFontRes);
        defaultFontRes = nullptr;
    }
    for (auto& pair : fonts) {
        if (pair.second) {
            DeleteObject(pair.second);
        }
    }
}

HFONT FontManager::KCreateFont(int fontSize) {
    HDC hdc = GetDC(NULL);
    int logPixelsY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    int fontHeight = -MulDiv(
        fontSize,
        logPixelsY,
        72
    );
    return CreateFont(
        fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_MODERN, L"JetBrains Mono"
    );
}

HFONT FontManager::GetFont(int fontId) {
    if (fonts.find(fontId) != fonts.end()) {
        return fonts[fontId];
    }
    return defaultFont;
}
