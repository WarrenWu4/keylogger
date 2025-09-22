#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <memory>
#include "resource_finder.h"

struct FontProperties {
    std::wstring fontFamily = L"JetBrains Mono";
    int fontSize = 12;
    Gdiplus::FontStyle fontStyle = Gdiplus::FontStyleBold;
};

class FontManager {
private:
    std::shared_ptr<ResourceFinder> resourceFinder = std::make_shared<ResourceFinder>();
    Gdiplus::PrivateFontCollection fontCollection;
    FontProperties fontProperties = FontProperties();
public:
    FontManager();
    Gdiplus::PrivateFontCollection& getFontCollection() { return fontCollection; }
    FontProperties getFontProperties() { return fontProperties; }
};