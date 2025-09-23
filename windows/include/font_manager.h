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

    FontProperties() {
        fontFamily = L"JetBrains Mono";
        fontSize = 12;
        fontStyle = Gdiplus::FontStyleBold;
    }

    FontProperties(std::wstring ff, int fs, Gdiplus::FontStyle fst) {
        fontFamily = ff;
        fontSize = fs;
        fontStyle = fst;
    }

    bool operator==(const FontProperties& other) const {
        return fontFamily == other.fontFamily &&
               fontSize == other.fontSize &&
               fontStyle == other.fontStyle;
    }
};

namespace std {
    template <>
    struct hash<FontProperties> {
        std::size_t operator()(const FontProperties& p) const noexcept {
            std::size_t h1 = std::hash<std::wstring>{}(p.fontFamily);
            std::size_t h2 = std::hash<int>{}(p.fontSize);
            std::size_t h3 = std::hash<int>{}(p.fontStyle);
            std::size_t seed = h1;
            seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            return seed;
        }
    };
}

class FontManager {
private:
    std::shared_ptr<ResourceFinder> resourceFinder = std::make_shared<ResourceFinder>();
    Gdiplus::PrivateFontCollection fontCollection;
    static std::unordered_map<FontProperties, std::shared_ptr<Gdiplus::Font>> fontCache;

    void createFont(FontProperties fp);

public:
    FontManager();
    std::shared_ptr<Gdiplus::Font> getFont(FontProperties fp);
};