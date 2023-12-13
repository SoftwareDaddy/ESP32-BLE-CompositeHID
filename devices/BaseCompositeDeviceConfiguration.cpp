#include "BaseCompositeDeviceConfiguration.h"

BaseCompositeDeviceConfiguration::CompositeDeviceConfiguration(reportId) : 
    _autoReport(true),
    _reportId(reportId)
{               
}

bool BaseCompositeDeviceConfiguration::getAutoReport() { return _autoReport; }
uint8_t BaseCompositeDeviceConfiguration::getReportId() { return _gamepadHIDReportId; }

void BaseCompositeDeviceConfiguration::setAutoReport(bool value) { _autoReport = value; }
void BaseCompositeDeviceConfiguration::setReportId(uint8_t value) { _gamepadHIDReportId = value; }