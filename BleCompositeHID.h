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
#include "BaseCompositeDevice.h"

#include <vector>

class BleCompositeHID
{
public:
    BleCompositeHID(std::string deviceName = "ESP32 BLE Gamepad", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
    ~BleCompositeHID();
    void begin(const BLEHostConfiguration& config = BLEHostConfiguration());
    void end();

    void setBatteryLevel(uint8_t level);

    void addDevice(BaseCompositeDevice* device);
    bool isConnected();

    uint8_t batteryLevel;
    std::string deviceManufacturer;
    std::string deviceName;

protected:
    virtual void onStarted(NimBLEServer *pServer){};

private:
    static void taskServer(void *pvParameter);

    BLEHostConfiguration _configuration;
    BleConnectionStatus* _connectionStatus;
    NimBLEHIDDevice* _hid;

    std::vector<BaseCompositeDevice*> _devices;
};

#endif // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_MULTI_HID_H
