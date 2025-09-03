#include "font_manager.h"

FontManager::FontManager(HINSTANCE hInstance, HWND hwnd) {
    HRSRC hRes = FindResource(hInstance, MAKEINTRESOURCE(IDI_APP_FONT), RT_FONT);
    HGLOBAL hMem = LoadResource(hInstance, hRes);
    void* pFontData = LockResource(hMem);
    DWORD fontSize = SizeofResource(hInstance, hRes);
    DWORD nFonts = 0;
    hFontRes = AddFontMemResourceEx(pFontData, fontSize, NULL, &nFonts);
    HDC hdc = GetDC(hwnd);
    int logPixelsY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(hwnd, hdc);
    int fontHeight = -MulDiv(10, logPixelsY, 72);
    hFont = CreateFont(
        fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_MODERN, L"JetBrains Mono"
    );
}

FontManager::~FontManager() {
    if (hFont) {
        DeleteObject(hFont);
        hFont = nullptr;
    }
    if (hFontRes) {
        RemoveFontMemResourceEx(hFontRes);
        hFontRes = nullptr;
    }
}

HFONT FontManager::getFont() {
    return hFont;
}
