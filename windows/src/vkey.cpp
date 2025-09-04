#include "vkey.h"

const std::unordered_map<UINT, std::wstring> specialKeys = {
    {VK_SHIFT, L"\u21E7"},
    {VK_CONTROL, L"\u2303"},
    {VK_MENU, L"\u2756"},
    {VK_LWIN, L"\u2756"},
    {VK_RWIN, L"\u2318"},
    {VK_ESCAPE, L"ESC"},
    {VK_INSERT, L"\u2396"},
    {VK_HOME, L"\u2302"},
    {VK_END, L"\u2305"},
    {VK_RETURN, L"\u23CE"},
    {VK_BACK, L"\u232B"},
    {VK_TAB, L"TAB"},
    {VK_SPACE, L"\u2423"},
    {VK_CAPITAL, L"\u21EA"},
    {VK_LEFT, L"\u2190"},
    {VK_UP, L"\u238B"},
    {VK_RIGHT, L"\u2192"},
    {VK_DOWN, L"\u2193"},
    {VK_DELETE, L"DEL"},
};

std::wstring getStrFromVKey(UINT vKey, UINT scanCode, BYTE keyboardState[256]) {
    // handle special keys first
    if (specialKeys.find(vKey) != specialKeys.end()) {
        return specialKeys.at(vKey);
    }
    // if not found in special keys let unicode ex handle it
    wchar_t buffer[4];
    int result = ToUnicodeEx(vKey, scanCode, keyboardState, buffer, 4, 0, GetKeyboardLayout(0));
    if (result == 1) {
        return std::wstring(1, buffer[0]);
    }
    return L"";
}