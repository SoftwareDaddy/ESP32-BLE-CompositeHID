#ifndef XBOX_GAMEPAD_CONFIGURATION_H
#define XBOX_GAMEPAD_CONFIGURATION_H

#include "XboxDescriptors.h"
#include "BaseCompositeDevice.h"

class XboxGamepadDeviceConfiguration : public BaseCompositeDeviceConfiguration {
public:
    XboxGamepadDeviceConfiguration(uint8_t reportId = XBOX_INPUT_REPORT_ID);
};


class XboxOneSControllerDeviceConfiguration : public XboxGamepadDeviceConfiguration {
    BLEHostConfiguration getIdealHostConfiguration() override;
    uint8_t getDeviceReportSize() override;
    size_t makeDeviceReport(uint8_t* buffer, size_t bufferSize) override;
};


class XboxSeriesXControllerDeviceConfiguration : public XboxGamepadDeviceConfiguration {
    BLEHostConfiguration getIdealHostConfiguration() override;
    uint8_t getDeviceReportSize() override;
    size_t makeDeviceReport(uint8_t* buffer, size_t bufferSize) override;
};

#endif // XBOX_GAMEPAD_CONFIGURATION_H
