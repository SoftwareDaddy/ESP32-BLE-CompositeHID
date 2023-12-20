#ifndef XBOX_GAMEPAD_DEVICE_H
#define XBOX_GAMEPAD_DEVICE_H

#include <NimBLECharacteristic.h>
#include "BaseCompositeDevice.h"
#include "GamepadDevice.h"

#define XBOX_INPUT_REPORT_ID 0x01
#define XBOX_EXTRA_INPUT_REPORT_ID 0x02
#define XBOX_OUTPUT_REPORT_ID 0x03

// Forwards
class XboxGamepadDevice;


class XboxGamepadCallbacks : public NimBLECharacteristicCallbacks
{
public:
    XboxGamepadCallbacks(XboxGamepadDevice* device);

    void onWrite(NimBLECharacteristic* pCharacteristic) override;
    void onRead(NimBLECharacteristic* pCharacteristic) override;
    void onNotify(NimBLECharacteristic* pCharacteristic) override;
    void onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code) override;

private:
    XboxGamepadDevice* _device;
};


class XboxGamepadDeviceConfiguration : public BaseCompositeDeviceConfiguration {
public:
    XboxGamepadDeviceConfiguration(uint8_t reportId);
    uint8_t getDeviceReportSize() override;
    size_t makeDeviceReport(uint8_t* buffer, size_t bufferSize) override;
};


class XboxGamepadDevice : public BaseCompositeDevice {
public:
    XboxGamepadDevice(const XboxGamepadDeviceConfiguration& config);
    ~XboxGamepadDevice();

    void init(NimBLEHIDDevice* hid) override;
    BaseCompositeDeviceConfiguration* getDeviceConfig() override;

private:
    XboxGamepadDeviceConfiguration _config;

    NimBLECharacteristic* _extra_input;
    XboxGamepadCallbacks* _callbacks;
};

#endif // XBOX_GAMEPAD_DEVICE_H
