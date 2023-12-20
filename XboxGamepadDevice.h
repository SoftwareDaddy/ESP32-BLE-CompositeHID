#ifndef XBOX_GAMEPAD_DEVICE_H
#define XBOX_GAMEPAD_DEVICE_H

#include <NimBLECharacteristic.h>
#include <Callback.h>
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

struct XboxGamepadOutputReportData {
    uint8_t dcEnableActuators;  // 4bit for DC Enable Actuators + 4bit padding (1 byte)
    uint32_t magnitude;
    uint8_t duration;
    uint8_t startDelay;
    uint8_t loopCount;
};


struct XboxGamepadInputReportData {
    uint16_t x;
    uint16_t y;
    uint16_t z;
    uint16_t rz;
    uint16_t brake;         // 10 bits for brake + 6 bit padding (2 bytes)
    uint16_t accelerator;   // 10 bits for accelerator + 6bit padding
    uint8_t hat;            // 4bits for hat switch + 4 bit padding (1 byte) 
    uint16_t buttons;       // 15 * 1bit for buttons + 1 bit padding (2 bytes)
    uint8_t record;         // 1 bits for record button + 7 bit padding (1 byte)
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

    Signal<XboxGamepadOutputReportData> onVibrate;

private:
    XboxGamepadDeviceConfiguration _config;
    XboxGamepadInputReportData _inputReport;

    NimBLECharacteristic* _extra_input;
    XboxGamepadCallbacks* _callbacks;
};

#endif // XBOX_GAMEPAD_DEVICE_H
