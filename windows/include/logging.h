#pragma once

#include <windows.h>
#include <fstream>

class Logger {
private:
    std::wofstream logFile;
    std::wstring logFilePath;

public:
    Logger(const std::wstring& filePath);
    ~Logger();
    
    void LogMessageToFile(const std::wstring& message);
};
