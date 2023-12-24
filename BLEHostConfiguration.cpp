#include "BLEHostConfiguration.h"

BLEHostConfiguration::BLEHostConfiguration() :
    _vidSource(0x01),
    _vid(0xe502),
    _pid(0xbbab),
    _guidVersion(0x0110),
    _modelNumber("1.0.0"),
    _softwareRevision("1.0.0"),
    _serialNumber("0123456789"),
    _firmwareRevision("0.5.2"),
    _hardwareRevision("1.0.0"),
    _systemID("")
{               
}

uint16_t BLEHostConfiguration::getVidSource(){ return _vidSource; }
uint16_t BLEHostConfiguration::getVid(){ return _vid; }
uint16_t BLEHostConfiguration::getPid(){ return _pid; }
uint16_t BLEHostConfiguration::getGuidVersion(){ return _guidVersion; }

char *BLEHostConfiguration::getModelNumber(){ return _modelNumber; }
char *BLEHostConfiguration::getSoftwareRevision(){ return _softwareRevision; }
char *BLEHostConfiguration::getSerialNumber(){ return _serialNumber; }
char *BLEHostConfiguration::getFirmwareRevision(){ return _firmwareRevision; }
char *BLEHostConfiguration::getHardwareRevision(){ return _hardwareRevision; }
char *BLEHostConfiguration::getSystemID(){ return _systemID; }

void BLEHostConfiguration::setVidSource(uint8_t value) { _vidSource = value; }
void BLEHostConfiguration::setVid(uint16_t value) { _vid = value; }
void BLEHostConfiguration::setPid(uint16_t value) { _pid = value; }
void BLEHostConfiguration::setGuidVersion(uint16_t value) { _guidVersion = value; }

void BLEHostConfiguration::setModelNumber(char *value) { _modelNumber = value; }
void BLEHostConfiguration::setSoftwareRevision(char *value) { _softwareRevision = value; }
void BLEHostConfiguration::setSerialNumber(char *value) { _serialNumber = value; }
void BLEHostConfiguration::setFirmwareRevision(char *value) { _firmwareRevision = value; }
void BLEHostConfiguration::setHardwareRevision(char *value) { _hardwareRevision = value; }
