#pragma once

#include <windows.h>
#include <string>
#include <deque>

void ResizeWindowToText(HWND hwnd, HFONT hFont, std::deque<std::wstring>& keyBuffer);


