#pragma once

#include <windows.h>
#include <fstream>
#include <iostream>

class Logger {
private:
    std::wofstream file;
    std::wstring filePath;
    std::size_t maxFileSize;

    std::size_t GetFileSize();
    void RotateFile();
    void PrintErrorAndExit(const std::wstring& error, DWORD errorCode);
public:
    // 10KB default size
    Logger(const std::wstring& filePath, std::size_t maxSize = 10240);
    ~Logger();
    
    void LogMessageToFile(const std::wstring& message);
};
