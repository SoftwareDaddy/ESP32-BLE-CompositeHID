#ifndef ESP32_BLE_MULTI_HID_H
#define ESP32_BLE_MULTI_HID_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "nimconfig.h"
#if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)

#include "BleConnectionStatus.h"
#include "NimBLEHIDDevice.h"
#include "NimBLECharacteristic.h"

#include "BLEHostConfiguration.h"

class BleCompositeHID
{
    BLEHostConfiguration configuration;

    BleConnectionStatus *connectionStatus;

    NimBLEHIDDevice *hid;
    
    static void taskServer(void *pvParameter);

public:
    BleCompositeHID(std::string deviceName = "ESP32 BLE Gamepad", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
    void begin(BLEHostConfiguration *config = new BLEHostConfiguration());
    void end(void);
    
    uint8_t batteryLevel;
    std::string deviceManufacturer;
    std::string deviceName;

protected:
    virtual void onStarted(NimBLEServer *pServer){};
};

#endif // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_MULTI_HID_H
