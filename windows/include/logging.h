#pragma once

#include <cstring>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

class Logger {
private:
  const int BUFFER_SIZE = 8 * 1024; // 8KB buffer size
  std::string filename;
  std::fstream file;
  std::size_t maxFileSize;
  std::vector<char> buffer; // in-memory buffer for batching
public:
  Logger(const std::string& filename, std::size_t maxFileSize);
  ~Logger();
  void write(const std::string& data);
  void flush();
};
