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
    fonts[L"JetBrains Mono"][16] = defaultFont;
    fonts[L"JetBrains Mono"][12] = FontManager::createNewFont(12);
    fonts[L"JetBrains Mono"][8] = FontManager::createNewFont(8);
}

FontManager::~FontManager() {
    for (auto& pair : fonts) {
        for (auto& sizePair : pair.second) {
            if (sizePair.second) {
                DeleteObject(sizePair.second);
            }
        }
    }
}

HFONT FontManager::createNewFont(int fontSize) {
    HDC hdc = GetDC(NULL);
    int logPixelsY = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    int fontHeight = -MulDiv(
        fontSize,
        logPixelsY,
        72
    );
    fonts[L"JetBrains Mono"][fontSize] = CreateFont(
        fontHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_MODERN, L"JetBrains Mono"
    );
    return fonts[L"JetBrains Mono"][fontSize];
}

HFONT FontManager::getFont(std::wstring fontName, int fontSize) {
    if (fonts.find(fontName) != fonts.end()) {
        if (fonts[fontName].find(fontSize) != fonts[fontName].end()) {
            return fonts[fontName][fontSize];
        }
    }
    return defaultFont;
}
