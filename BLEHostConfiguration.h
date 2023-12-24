#ifndef ESP32_BLE_HOST_CONFIG_H
#define ESP32_BLE_HOST_CONFIG_H

#include <Arduino.h>
#include <HIDKeyboardTypes.h>

class BLEHostConfiguration
{
private:
    
    uint8_t _vidSource;
    uint16_t _vid;
    uint16_t _pid;
	uint16_t _guidVersion;
    char *_modelNumber;
    char *_softwareRevision;
    char *_serialNumber;
    char *_firmwareRevision;
    char *_hardwareRevision;
    char *_systemID;

public:
    BLEHostConfiguration();
    uint16_t getVidSource();
    uint16_t getVid();
    uint16_t getPid();
	uint16_t getGuidVersion();
    char *getModelNumber();
    char *getSoftwareRevision();
    char *getSerialNumber();
    char *getFirmwareRevision();
    char *getHardwareRevision();
    char *getSystemID();

    void setVidSource(uint8_t value);
    void setVid(uint16_t value);
    void setPid(uint16_t value);
	void setGuidVersion(uint16_t value);
    void setModelNumber(char *value);
    void setSoftwareRevision(char *value);
    void setSerialNumber(char *value);
    void setFirmwareRevision(char *value);
    void setHardwareRevision(char *value);
};

#endif
