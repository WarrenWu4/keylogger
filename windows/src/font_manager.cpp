#include "font_manager.h"

FontManager::FontManager() {
    std::wstring fontPath = resourceFinder->getResource(L"JetBrainsMono-Bold.ttf");
    Gdiplus::Status status = fontCollection.AddFontFile(fontPath.c_str());
    if (status != Gdiplus::Ok) {
        MessageBox(NULL, L"Failed to load fonts", L"Error", MB_OK);
        throw std::runtime_error("Failed to load fonts");
    }
}

std::shared_ptr<Gdiplus::Font> FontManager::createNewFont(int fontSize) {
    Gdiplus::FontFamily fontFamily(L"JetBrains Mono", &fontCollection);
    fonts[L"JetBrains Mono"][fontSize] = std::make_shared<Gdiplus::Font>(&fontFamily, fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    return fonts[L"JetBrains Mono"][fontSize];
}

std::shared_ptr<Gdiplus::Font> FontManager::getFont(std::wstring fontName, int fontSize) {
    if (fonts.find(fontName) != fonts.end()) {
        if (fonts[fontName].find(fontSize) != fonts[fontName].end()) {
            return fonts[fontName][fontSize];
        }
    }
    return createNewFont(fontSize);
}
