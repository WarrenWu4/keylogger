#pragma once

#include <windows.h>
#include <string>
#include <filesystem>


class ResourceFinder {
private:
    std::wstring resourcePath = L"";

    std::wstring findResourceFolder() {
        wchar_t buffer[MAX_PATH];
        GetModuleFileNameW(NULL, buffer, MAX_PATH);
        std::filesystem::path exePath(buffer);
        std::filesystem::path projectRoot = exePath.parent_path();
        std::filesystem::path resourcesDir = projectRoot / "resources";
        if (std::filesystem::exists(resourcesDir) && std::filesystem::is_directory(resourcesDir)) {
            return resourcesDir.wstring() + L"\\";
        }
        return L".\\resources\\";
    }

public:
    ResourceFinder() {}
    ~ResourceFinder() {}

    std::wstring getResource(const std::wstring& resourceName) {
        if (resourcePath.empty()) {
            resourcePath = findResourceFolder();
        }
        return resourcePath + resourceName;
    }
};