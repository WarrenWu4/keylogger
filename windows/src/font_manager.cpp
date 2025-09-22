#include "font_manager.h"

FontManager::FontManager() {
    std::wstring fontPath = resourceFinder->getResource(L"JetBrainsMono-Bold.ttf");
    Gdiplus::Status status = fontCollection.AddFontFile(fontPath.c_str());
    if (status != Gdiplus::Ok) {
        MessageBox(NULL, L"Failed to load fonts", L"Error", MB_OK);
        throw std::runtime_error("Failed to load fonts");
    }
}