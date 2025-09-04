#pragma once

#include <windows.h>
#include <string>
#include <unordered_map>

extern const std::unordered_map<UINT, std::wstring> specialKeys;
std::wstring getStrFromVKey(UINT vKey, UINT scanCode, BYTE keyboardState[256]);
