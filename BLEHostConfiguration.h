#ifndef ESP32_BLE_HOST_CONFIG_H
#define ESP32_BLE_HOST_CONFIG_H

#include <Arduino.h>
#include <string>
#include <HIDKeyboardTypes.h>

class BLEHostConfiguration
{
private:
    
    uint8_t _vidSource;
    uint16_t _vid;
    uint16_t _pid;
	uint16_t _guidVersion;
    std::string _modelNumber;
    std::string _softwareRevision;
    std::string _serialNumber;
    std::string _firmwareRevision;
    std::string _hardwareRevision;
    std::string _systemID;

public:
    BLEHostConfiguration();
    uint16_t getVidSource();
    uint16_t getVid();
    uint16_t getPid();
	uint16_t getGuidVersion();
    const char* getModelNumber();
    const char* getSoftwareRevision();
    const char* getSerialNumber();
    const char* getFirmwareRevision();
    const char* getHardwareRevision();
    const char* getSystemID();

    void setVidSource(uint8_t value);
    void setVid(uint16_t value);
    void setPid(uint16_t value);
	void setGuidVersion(uint16_t value);
    void setModelNumber(const char *value);
    void setSoftwareRevision(const char *value);
    void setSerialNumber(const char *value);
    void setFirmwareRevision(const char *value);
    void setHardwareRevision(const char *value);
};

#endif
