#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <stdexcept>

class GdiplusContext {
private:
    ULONG_PTR gdiplusToken = 0;
public:
    GdiplusContext() {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok) {
            throw std::runtime_error("Failed to initialize GDI+");
            return;
        }
    }

    ~GdiplusContext() {
        MessageBoxA(NULL, "Shutting down GDI+", "Info", MB_OK);
        if (gdiplusToken) {
            Gdiplus::GdiplusShutdown(gdiplusToken);
            gdiplusToken = 0;
        }
    }
};