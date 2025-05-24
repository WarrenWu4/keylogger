#pragma once

#include <vector>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <stdexcept>

class Logger {
private:
    std::wofstream file;
    std::wstring filePath;
    std::size_t maxFileSize;

    std::size_t GetFileSize();
    void TruncateStart(std::size_t bytes);
    void PrintErrorAndExit(const std::wstring& error, DWORD errorCode);
public:
    // 10KB default size
    Logger(const std::wstring& filePath, std::size_t maxSize = 10240);
    ~Logger();
    
    void LogMessageToFile(const std::wstring& message);
};

class FixedSizeLogger { 
private:
    std::fstream file;
    std::wstring filename;
    std::size_t maxFileSize;
    std::size_t recordSize;
    std::size_t maxRecords;
    std::size_t currentRecords;
    std::size_t headPos;
    std::vector<char> buffer;
    std::size_t bufferPos;
    // 64 KB buffer
    static constexpr size_t BUFFER_SIZE = 64 * 1024;
public:
    FixedSizeLogger(const std::wstring& _filename, std::size_t _maxFileSize, std::size_t _recordSize): filename(_filename), maxFileSize(_maxFileSize), recordSize(_recordSize), maxRecords(_maxFileSize / _recordSize), currentRecords(0), headPos(0), buffer(BUFFER_SIZE), bufferPos(0) {
        if (recordSize == 0 || _maxFileSize < recordSize) {
            throw std::invalid_argument("Invalid size parameters");
        }
        file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
        // create file if it doesn't exist
        if (!file) {
            file.clear();
            file.open(filename, std::ios::binary | std::ios::out);
            file.close();
            file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
        }
        if (!file) {
            throw std::runtime_error("Cannot open file");
        }
        file.seekg(0, std::ios::end);
        std::size_t fileSize = file.tellg();
        currentRecords = fileSize / recordSize;
        if (currentRecords >= maxRecords) {
            currentRecords = maxRecords;
        }
        file.seekp(0, std::ios::beg);
    }

    ~FixedSizeLogger() {
        flush();
    }

    void addRecord(const void* data) {
        if (bufferPos + recordSize > BUFFER_SIZE) {
            flush();
        }
        std::memcpy(buffer.data() + bufferPos, data, recordSize);
        bufferPos += recordSize;
        if (currentRecords < maxRecords) {
            currentRecords++;
        } else {
            headPos = (headPos + recordSize) % maxRecords;
        }
    }

    void flush() {
        if (bufferPos == 0) { return; }
        std::size_t recordsToWrite = bufferPos / recordSize;
        for (std::size_t i = 0; i < recordsToWrite; i++) {
            std::size_t writePos;
            if (currentRecords <= maxRecords) {
                writePos = ((headPos + currentRecords - recordsToWrite + i) % maxRecords) * recordSize;
            } else {
                writePos = ((headPos + i) % maxRecords) * recordSize;
            }
            file.seekp(writePos);
            file.write(buffer.data()+i*recordSize, recordSize);
        }
        file.flush();
        bufferPos = 0;
    }

    std::size_t size() const {
        return currentRecords;
    }

    // get max capacity
    std::size_t capacity() const {
        return maxRecords;
    }

    // check if buffer is at capacity
    bool isFull() const {
        return currentRecords >= maxRecords;
    }

    void clear() {
        flush();
        file.close();
        std::ofstream truncate(filename, std::ios::trunc);
        truncate.close();
        file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
        currentRecords = 0;
        headPos = 0;
        bufferPos = 0;
    }
};
