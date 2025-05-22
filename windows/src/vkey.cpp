#include "vkey.h"

const std::unordered_map<DWORD,std::wstring> vkeyToWString = {
    // number keys and the keypad equivalent 
    {0x30, L"0"}, {0x60, L"0"},
    {0x31, L"1"}, {0x61, L"1"},
    {0x32, L"2"}, {0x62, L"2"},
    {0x33, L"3"}, {0x63, L"3"},
    {0x34, L"4"}, {0x64, L"4"},
    {0x35, L"5"}, {0x65, L"5"},
    {0x36, L"6"}, {0x66, L"6"},
    {0x37, L"7"}, {0x67, L"7"},
    {0x38, L"8"}, {0x68, L"8"},
    {0x39, L"9"}, {0x69, L"9"},
    // letters
    {0x41, L"a"}, {0x42, L"b"}, {0x43, L"c"}, {0x44, L"d"}, {0x45, L"e"},
    {0x46, L"f"}, {0x47, L"g"}, {0x48, L"h"}, {0x49, L"i"}, {0x4A, L"j"},
    {0x4B, L"k"}, {0x4C, L"l"}, {0x4D, L"m"}, {0x4E, L"n"}, {0x4F, L"o"},
    {0x50, L"p"}, {0x51, L"q"}, {0x52, L"r"}, {0x53, L"s"}, {0x54, L"t"},
    {0x55, L"u"}, {0x56, L"v"}, {0x57, L"w"}, {0x58, L"x"}, {0x59, L"y"}, 
    {0x5A, L"z"},
    // special characters
    {0x08, L"\u232B"}, // backspace
    {0x09, L"\u21E5"}, // tab
    {0x0D, L"\u23CE"}, // enter
    {0x10, L"\u21E7"}, {0xA0, L"\u21E7"}, {0xA1, L"\u21E7"}, // shift, left shift, right shift
    {0x11, L"\u2303"}, {0xA2, L"\u2303"}, {0xA3, L"\u2303"}, // ctrl, left ctrl, right ctrl
    {0x12, L"\u2387"}, {0xA4, L"\u2387"}, {0xA5, L"\u2387"}, // alt, left alt, right, alt
    {0x14, L"\u21EA"}, // caps lock
    {0x1B, L"ESC"}, // escape
    {0x20, L"\u2423"}, // spacebar
    {0x25, L"\u2190"}, {0x26, L"\u238B"}, {0x27, L"\u2192"}, {0x28, L"\u2193"}, // left, up, right, down arrow
    {0x2E, L"DEL"}, // delete
    {0x5B, L"\u2756"}, {0x5C, L"\u2756"}, // windows keys
    // multiply
    // add
    // separator
    // subtract
    // decimal
    // divide
    {0xC0, L"~"}, // tilda
    {0xDE, L"'"}, // apostrophe

    // f1-f24
    {0x70, L"F1"}, {0x71, L"F2"}, {0x72, L"F3"}, {0x73, L"F4"}, {0x74, L"F5"},
    {0x75, L"F6"}, {0x76, L"F7"}, {0x77, L"F8"}, {0x78, L"F9"}, {0x79, L"F10"},
    {0x7A, L"F11"}, {0x7B, L"F12"}, {0x7C, L"F13"}, {0x7D, L"F14"}, {0x7E, L"F15"},
    {0x7F, L"F16"}, {0x80, L"F17"}, {0x81, L"F18"}, {0x82, L"F19"}, {0x83, L"F20"},
    {0x84, L"F21"}, {0x85, L"F22"}, {0x86, L"F23"}, {0x87, L"F24"},

    // keys i dont really ever use
    {0xFA, L"Play"}, {0xFB, L"Zoom"}, {0xFE, L"Clear"} // miscellaneous keys
};

std::wstring GetKeyNameFromVkey(DWORD vkey) {
    auto it = vkeyToWString.find(vkey);
    if (it != vkeyToWString.end()) {
        return it->second;
    }
    return L"[" + std::to_wstring(vkey) + L"]";
}
