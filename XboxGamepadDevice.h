#ifndef XBOX_GAMEPAD_DEVICE_H
#define XBOX_GAMEPAD_DEVICE_H

#include <NimBLECharacteristic.h>
#include <Callback.h>

#include "BLEHostConfiguration.h"
#include "BaseCompositeDevice.h"
#include "GamepadDevice.h"

#define XBOX_INPUT_REPORT_ID 0x01
#define XBOX_EXTRA_INPUT_REPORT_ID 0x02
#define XBOX_OUTPUT_REPORT_ID 0x03

// Button bitmasks
#define XBOX_BUTTON_A 0x01
#define XBOX_BUTTON_B 0x02
// UNUSED - 0x04
#define XBOX_BUTTON_X 0x08 
#define XBOX_BUTTON_Y 0x10
// UNUSED - 0x20
#define XBOX_BUTTON_LB 0x40
#define XBOX_BUTTON_RB 0x80
// UNUSED - 0x100
// UNUSED - 0x200
// UNUSED - 0x400
#define XBOX_BUTTON_START 0x800
#define XBOX_BUTTON_HOME 0x1000
#define XBOX_BUTTON_LS 0x2000
#define XBOX_BUTTON_RS 0x4000

// Select bitmask
// The select button lives in its own byte at the end of the input report
#define XBOX_BUTTON_SELECT 0x01

// Dpad bitmasks
#define XBOX_BUTTON_DPAD_NORTH 0x01
#define XBOX_BUTTON_DPAD_NORTHEAST 0x02
#define XBOX_BUTTON_DPAD_EAST 0x03
#define XBOX_BUTTON_DPAD_SOUTHEAST 0x04
#define XBOX_BUTTON_DPAD_SOUTH 0x05
#define XBOX_BUTTON_DPAD_SOUTHWEST 0x06
#define XBOX_BUTTON_DPAD_WEST 0x07
#define XBOX_BUTTON_DPAD_NORTHWEST 0x08

// Trigger range
#define XBOX_TRIGGER_MIN 0
#define XBOX_TRIGGER_MAX 1023

// Thumbstick range
#define XBOX_STICK_MIN -32768
#define XBOX_STICK_MAX 32767


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
public:
    uint8_t dcEnableActuators = 0x00;   // 4bits for DC Enable Actuators, 4bits padding
    uint8_t leftTriggerMagnitude = 0;
    uint8_t rightTriggerMagnitude = 0; 
    uint8_t weakMotorMagnitude = 0;
    uint8_t strongMotorMagnitude = 0; 
    uint8_t duration = 0;               // UNUSED
    uint8_t startDelay = 0;             // UNUSED
    uint8_t loopCount = 0;              // UNUSED
};

struct XboxGamepadInputReportData {
    uint16_t x = 0;
    uint16_t y = 0;
    uint16_t z = 0;
    uint16_t rz = 0;
    uint16_t brake = 0;         // 10 bits for brake + 6 bit padding (2 bytes)
    uint16_t accelerator = 0;   // 10 bits for accelerator + 6bit padding
    uint8_t hat = 0x00;         // 4bits for hat switch + 4 bit padding (1 byte) 
    uint16_t buttons = 0x00;    // 15 * 1bit for buttons + 1 bit padding (2 bytes)
    uint8_t select = 0x00;      // 1 bits for select button + 7 bit padding (1 byte)
};

class XboxGamepadDeviceConfiguration : public BaseCompositeDeviceConfiguration {
public:
    XboxGamepadDeviceConfiguration(uint8_t reportId = XBOX_INPUT_REPORT_ID);
    uint8_t getDeviceReportSize() override;
    size_t makeDeviceReport(uint8_t* buffer, size_t bufferSize) override;
};


class XboxGamepadDevice : public BaseCompositeDevice {
public:
    XboxGamepadDevice(const XboxGamepadDeviceConfiguration& config = XboxGamepadDeviceConfiguration());
    ~XboxGamepadDevice();

    static BLEHostConfiguration getFakedHostConfiguration();

    void init(NimBLEHIDDevice* hid) override;
    BaseCompositeDeviceConfiguration* getDeviceConfig() override;

    Signal<XboxGamepadOutputReportData> onVibrate;

    // Input Controls
    void resetInputs();
    void press(uint16_t button = XBOX_BUTTON_A);    
    void release(uint16_t button = XBOX_BUTTON_A); 
    bool isPressed(uint16_t button = XBOX_BUTTON_A);
    void setLeftThumb(int16_t x = 0, int16_t y = 0);
    void setRightThumb(int16_t z = 0, int16_t rZ = 0);
    void setLeftTrigger(uint16_t rX = 0);
    void setRightTrigger(uint16_t rY = 0);
    void setTriggers(uint16_t rX = 0, uint16_t rY = 0);
    void pressDPadDirection(uint8_t direction = 0);
    void releaseDPadDirection(uint8_t direction = 0);
    bool isDPadPressed(uint8_t direction = 0);
    void pressSelect();
    void releaseSelect();
    
    void sendGamepadReport();


private:
    XboxGamepadDeviceConfiguration _config;
    XboxGamepadInputReportData _inputReport;

    NimBLECharacteristic* _extra_input;
    XboxGamepadCallbacks* _callbacks;
};

#endif // XBOX_GAMEPAD_DEVICE_H
