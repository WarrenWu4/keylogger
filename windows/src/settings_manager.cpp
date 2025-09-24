#include "settings_manager.h"

std::string SettingsManager::stringToWstring(const std::wstring& wstr) {
    return std::string(wstr.begin(), wstr.end());
}

std::wstring SettingsManager::wstringToString(const std::string& str) {
    return std::wstring(str.begin(), str.end());
}

Gdiplus::Color SettingsManager::hexToArgb(std::wstring hex, Gdiplus::Color defaultColor) {
    if (hex.size() != 7 || hex[0] != L'#') { return defaultColor; }
    for (size_t i = 1; i < hex.size(); i++) {
        if (!iswxdigit(hex[i])) { return defaultColor; }
    }
    return Gdiplus::Color(
        std::stoi(hex.substr(1, 2), nullptr, 16),
        std::stoi(hex.substr(3, 2), nullptr, 16),
        std::stoi(hex.substr(5, 2), nullptr, 16)
    );
}

std::wstring SettingsManager::argbToHex(Gdiplus::Color color) {
    int r = color.GetR();
    int g = color.GetG();
    int b = color.GetB();
    wchar_t buffer[8];
    swprintf(buffer, 8, L"#%02X%02X%02X", r, g, b);
    return std::wstring(buffer);
}

SettingsManager::SettingsManager() {
    std::wstring settingsPath = resourceFinder->getResource(L"settings.json");
    JsonValue res = jsonParser->parseFromFile(stringToWstring(settingsPath));
    JsonObject obj = res.getObject();

    appSettings->fontSize = static_cast<int>(obj.at("fontSize").getNumber());
    appSettings->fontFamily = wstringToString(obj.at("fontFamily").getString());
    appSettings->textColor = hexToArgb(wstringToString(obj.at("textColor").getString()));
    appSettings->backgroundColor = hexToArgb(wstringToString(obj.at("backgroundColor").getString()));
    appSettings->textBufferLength = static_cast<int>(obj.at("textBufferLength").getNumber());
    appSettings->inactiveTimeout = static_cast<int>(obj.at("inactiveTimeout").getNumber());

    JsonObject posObj = obj.at("windowPosition").getObject();
    appSettings->windowPosition.x = static_cast<int>(posObj.at("x").getNumber());
    appSettings->windowPosition.y = static_cast<int>(posObj.at("y").getNumber());

    JsonObject padObj = obj.at("windowPadding").getObject();
    appSettings->windowPadding.x = static_cast<int>(padObj.at("x").getNumber());
    appSettings->windowPadding.y = static_cast<int>(padObj.at("y").getNumber());
}

// ! UNTESTED
// ! stringify works but other parts might not
void SettingsManager::saveSettings() {
    std::string stringifyRes = "{\n\t\"fontSize\": " + std::to_string(appSettings->fontSize) + ",\n";
    stringifyRes += "\t\"fontFamily\": \"" + stringToWstring(appSettings->fontFamily) + "\",\n";
    stringifyRes += "\t\"textColor\": \"" + stringToWstring(argbToHex(appSettings->textColor)) + "\",\n";
    stringifyRes += "\t\"backgroundColor\": \"" + stringToWstring(argbToHex(appSettings->backgroundColor)) + "\",\n";
    stringifyRes += "\t\"textBufferLength\": " + std::to_string(appSettings->textBufferLength) + ",\n";
    stringifyRes += "\t\"inactiveTimeout\": " + std::to_string(appSettings->inactiveTimeout) + ",\n";
    stringifyRes += "\t\"windowPosition\": {\n";
    stringifyRes += "\t\t\"x\": " + std::to_string(appSettings->windowPosition.x) + ",\n";
    stringifyRes += "\t\t\"y\": " + std::to_string(appSettings->windowPosition.y) + "\n";
    stringifyRes += "\t},\n";
    stringifyRes += "\t\"windowPadding\": {\n";
    stringifyRes += "\t\t\"x\": " + std::to_string(appSettings->windowPadding.x) + ",\n";
    stringifyRes += "\t\t\"y\": " + std::to_string(appSettings->windowPadding.y) + "\n";
    stringifyRes += "\t}\n";
    stringifyRes += "}";
    std::wstring settingsPath = resourceFinder->getResource(L"settings.json");
    jsonParser->writeToFile(stringifyRes, stringToWstring(settingsPath));
}