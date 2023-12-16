#include "BaseCompositeDevice.h"
#include "BleCompositeHID.h"

BaseCompositeDeviceConfiguration::BaseCompositeDeviceConfiguration(uint8_t reportId) : 
    _autoReport(true),
    _reportId(reportId)
{
}

const char* BaseCompositeDeviceConfiguration::getDeviceName() {
    return "BaseCompositeDevice";
}

bool BaseCompositeDeviceConfiguration::getAutoReport() { return _autoReport; }
uint8_t BaseCompositeDeviceConfiguration::getReportId() { return _reportId; }

void BaseCompositeDeviceConfiguration::setAutoReport(bool value) { _autoReport = value; }
void BaseCompositeDeviceConfiguration::setHidReportId(uint8_t value) { _reportId = value; }

// ---------------

BleCompositeHID* BaseCompositeDevice::getParent() { return _parent; }

void BaseCompositeDevice::setCharacteristics(NimBLECharacteristic* input, NimBLECharacteristic* output) {
    _input = input;
    _output = output;
}

NimBLECharacteristic* BaseCompositeDevice::getInput() { return _input; }
NimBLECharacteristic* BaseCompositeDevice::getOutput() { return _output; }
