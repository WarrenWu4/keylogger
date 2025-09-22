#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <memory>

class FontManager {
private:
    Gdiplus::PrivateFontCollection fontCollection;
    std::unordered_map<std::wstring, std::unordered_map<int, std::shared_ptr<Gdiplus::Font>>> fonts;
public:
    FontManager();
    std::shared_ptr<Gdiplus::Font> getFont(std::wstring fontName, int fontSize);
    std::shared_ptr<Gdiplus::Font> createNewFont(int fontSize);
};