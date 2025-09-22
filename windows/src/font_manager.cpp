#include "font_manager.h"

std::unordered_map<FontProperties, std::shared_ptr<Gdiplus::Font>> FontManager::fontCache;

FontManager::FontManager() {
    std::wstring fontPath = resourceFinder->getResource(L"JetBrainsMono-Bold.ttf");
    Gdiplus::Status status = fontCollection.AddFontFile(fontPath.c_str());
    if (status != Gdiplus::Ok) {
        MessageBox(NULL, L"Failed to load fonts", L"Error", MB_OK);
        throw std::runtime_error("Failed to load fonts");
    }
}

std::shared_ptr<Gdiplus::Font> FontManager::getFont(FontProperties fp) {
    if (fontCache.find(fp) != fontCache.end()) {
        return fontCache[fp];
    }
    createFont(fp);
    return fontCache[fp];
}

void FontManager::createFont(FontProperties fp) {
    Gdiplus::FontFamily fontFamily(fp.fontFamily.c_str(), &fontCollection);
    fontCache[fp] = std::make_shared<Gdiplus::Font>(&fontFamily, fp.fontSize, fp.fontStyle, Gdiplus::UnitPixel);
}