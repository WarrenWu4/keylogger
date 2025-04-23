#include <iostream>
// #include <string>
#include <vector>

#include "include/keylogger.h"
#include "include/display.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define RD O_RDONLY

/*
void log_event(std::string kb_device) {
}
*/

int main() {
#ifdef __linux__
  // fix wsl bullshit
  std::cout << "OS: Linux\n";
  char message[80];
  // keylogging stuff
  Keylogger kl = Keylogger();
  std::string kb_device = kl.get_kb_device();
  // kl.log_event(kb_device);
  // display stuff
  /*
  KeyloggerDisplay kl_display = KeyloggerDisplay();
  for (int i = 0; i < 80; i++) {
    message[i] = 'a';
  }
  kl_display.display();
  */
#endif
#ifdef _WIN32
  std::cout << "OS: Windows\n";
  UINT numDevices = 0;
  GetRawInputDeviceList(nullptr, &numDevices, sizeof(RAWINPUTDEVICELIST));

  std::vector<RAWINPUTDEVICELIST> deviceList(numDevices);
  GetRawInputDeviceList(deviceList.data(), &numDevices, sizeof(RAWINPUTDEVICELIST));

  for (UINT i = 0; i < numDevices; ++i) {
    UINT nameSize = 0;
    GetRawInputDeviceInfo(deviceList[i].hDevice, RIDI_DEVICENAME, nullptr, &nameSize);

    std::vector<wchar_t> name(nameSize);
    if (GetRawInputDeviceInfo(deviceList[i].hDevice, RIDI_DEVICENAME, name.data(), &nameSize) > 0) {
      std::wcout << L"Device " << i << L": " << name.data() << L"\n";
    }

    RID_DEVICE_INFO deviceInfo;
    deviceInfo.cbSize = sizeof(deviceInfo);
    UINT infoSize = sizeof(deviceInfo);
    if (GetRawInputDeviceInfo(deviceList[i].hDevice, RIDI_DEVICEINFO, &deviceInfo, &infoSize) > 0) {
      switch (deviceInfo.dwType) {
        case RIM_TYPEMOUSE:
          std::wcout << L"  Type: Mouse\n";
          break;
        case RIM_TYPEKEYBOARD:
          std::wcout << L"  Type: Keyboard\n";
          break;
        case RIM_TYPEHID:
          std::wcout << L"  Type: HID\n";
          break;
      }
    }
  }
#endif
  return 0;
}
