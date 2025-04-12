#include <iostream>
#include <string>

#include "include/keylogger.h"
#include "include/display.h"

#define RD O_RDONLY


void log_event(std::string kb_device) {
}

int main() {
#ifdef __linux__
  std::cout << "OS: Linux\n";
  char message[80];
  // keylogging stuff
  /*
  Keylogger kl = Keylogger();
  std::string kb_device = kl.get_kb_device();
  kl.log_event(kb_device);
  */
  // display stuff
  KeyloggerDisplay kl_display = KeyloggerDisplay();
  for (int i = 0; i < 80; i++) {
    message[i] = 'a';
  }
  kl_display.display();
#endif
  return 0;
}
