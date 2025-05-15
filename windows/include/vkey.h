#pragma once

#include <windows.h>
#include <string>
#include <unordered_map>

std::wstring GetKeyNameFromVkey(DWORD vkey);
extern const std::unordered_map<DWORD,std::wstring> vkeyToWString;

