#include "logging.h"

Logger::Logger(const std::wstring& filePath) : logFilePath(filePath) {
    logFile.open(logFilePath.c_str(), std::ios::app);
    if (!logFile.is_open()) {
        MessageBox(NULL, L"Failed to open log file!", L"Error", MB_OK);
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::LogMessageToFile(const std::wstring& message) {
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.flush();
    }
}
