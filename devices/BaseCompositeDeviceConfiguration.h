#ifndef COMPOSITE_CONFIG_H
#define COMPOSITE_CONFIG_H

#include <Arduino.h>
#include <HIDKeyboardTypes.h>

class BaseCompositeDeviceConfiguration 
{
public:
    CompositeDeviceConfiguration(uint8_t reportId);

    bool getAutoReport();
    uint8_t getReportId();

    void setAutoReport(bool value);
    void setHidReportId(uint8_t value);

    virtual uint8_t getDeviceReportSize() = 0;
    virtual void makeDeviceReport(uint8* buffer, size_t size) = 0;

private:
    bool _autoReport;
    uint8_t _reportId;
}
