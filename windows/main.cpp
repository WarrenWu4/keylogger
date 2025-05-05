#include <windows.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <stdio.h>
#include <cfgmgr32.h>
#include <string>
#include <vector>
#include <iostream>

#pragma comment(lib, "setupapi.lib")

DEFINE_GUID(GUID_DEVINTERFACE_KEYBOARD,
    0x884b96c3, 0x56ef, 0x11d1,
    0xbc, 0x8c, 0x00, 0xa0, 0xc9, 0x14, 0x05, 0xdd);

std::vector<std::string> ListKeyboardDevices() {
  std::vector<std::string> keyboardDevicePaths;
  HDEVINFO deviceInfo = SetupDiGetClassDevs(
    &GUID_DEVINTERFACE_KEYBOARD,
    NULL,
    NULL,
    DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
  );
  if (deviceInfo == INVALID_HANDLE_VALUE) {
    std::cerr << "SetupDiGetClassDevs failed\n";
    return keyboardDevicePaths;
  }
  SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
  deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
  for (DWORD i = 0; SetupDiEnumDeviceInterfaces(deviceInfo, NULL, &GUID_DEVINTERFACE_KEYBOARD, i, &deviceInterfaceData); ++i) {
    DWORD requiredSize = 0;
    SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);
    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
    if (!deviceDetail) { continue; }
    deviceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    if (SetupDiGetDeviceInterfaceDetail(deviceInfo, &deviceInterfaceData, deviceDetail, requiredSize, NULL, NULL)) {
      std::string path = deviceDetail->DevicePath;
      std::cout << "Keyboard Device Found: " << path << "\n";
      keyboardDevicePaths.push_back(path);
    }
    free(deviceDetail);
  }
  SetupDiDestroyDeviceInfoList(deviceInfo);
  return keyboardDevicePaths;
}

int main( void )
{
  std::cout << "Listing Keyboard Devices:\n";
  std::vector<std::string> keyboardDevices = ListKeyboardDevices();

  return 0;
}
