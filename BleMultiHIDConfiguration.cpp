#include "BleMultiHIDConfiguration.h"

BleMultiHIDConfiguration::BleMultiHIDConfiguration() : _controllerType(CONTROLLER_TYPE_GAMEPAD),
                                                     _autoReport(true),
                                                     _gamepadHIDReportId(GAMEPAD_REPORT_ID),
                                                     _buttonCount(16),
                                                     _hatSwitchCount(1),
                                                     _whichSpecialButtons{false, false, false, false, false, false, false, false},
                                                     _whichAxes{true, true, true, true, true, true, true, true},
                                                     _whichSimulationControls{false, false, false, false, false},
                                                     _vid(0xe502),
                                                     _pid(0xbbab),
													 _guidVersion(0x0110),
                                                     _axesMin(0x0000),
                                                     _axesMax(0x7FFF),
                                                     _simulationMin(0x0000),
                                                     _simulationMax(0x7FFF),
                                                     _modelNumber("1.0.0"),
                                                     _softwareRevision("1.0.0"),
                                                     _serialNumber("0123456789"),
                                                     _firmwareRevision("0.5.2"),
                                                     _hardwareRevision("1.0.0"),
                                                     _useKeyboard(false),
                                                     _useMouse(false),
                                                     _mouseButtonCount(5),
                                                     _mouseAxisCount(4),
                                                     _mouseHIDReportId(MOUSE_REPORT_ID)
{               
}

uint8_t BleMultiHIDConfiguration::getTotalSpecialButtonCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLESPECIALBUTTONS; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t BleMultiHIDConfiguration::getDesktopSpecialButtonCount()
{
    int count = 0;
    for (int i = 0; i < 3; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t BleMultiHIDConfiguration::getConsumerSpecialButtonCount()
{
    int count = 0;
    for (int i = 3; i < 8; i++)
    {
        count += (int)_whichSpecialButtons[i];
    }

    return count;
}

uint8_t BleMultiHIDConfiguration::getAxisCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLEAXES; i++)
    {
        count += (int)_whichAxes[i];
    }

    return count;
}

uint8_t BleMultiHIDConfiguration::getSimulationCount()
{
    int count = 0;
    for (int i = 0; i < POSSIBLESIMULATIONCONTROLS; i++)
    {
        count += (int)_whichSimulationControls[i];
    }

    return count;
}

uint16_t BleMultiHIDConfiguration::getVid(){ return _vid; }
uint16_t BleMultiHIDConfiguration::getPid(){ return _pid; }
uint16_t BleMultiHIDConfiguration::getGuidVersion(){ return _guidVersion; }
int16_t BleMultiHIDConfiguration::getAxesMin(){ return _axesMin; }
int16_t BleMultiHIDConfiguration::getAxesMax(){ return _axesMax; }
int16_t BleMultiHIDConfiguration::getSimulationMin(){ return _simulationMin; }
int16_t BleMultiHIDConfiguration::getSimulationMax(){ return _simulationMax; }
uint8_t BleMultiHIDConfiguration::getControllerType() { return _controllerType; }
uint8_t BleMultiHIDConfiguration::getGamepadHidReportId() { return _gamepadHIDReportId; }
uint16_t BleMultiHIDConfiguration::getButtonCount() { return _buttonCount; }
uint8_t BleMultiHIDConfiguration::getHatSwitchCount() { return _hatSwitchCount; }
bool BleMultiHIDConfiguration::getAutoReport() { return _autoReport; }
bool BleMultiHIDConfiguration::getIncludeStart() { return _whichSpecialButtons[START_BUTTON]; }
bool BleMultiHIDConfiguration::getIncludeSelect() { return _whichSpecialButtons[SELECT_BUTTON]; }
bool BleMultiHIDConfiguration::getIncludeMenu() { return _whichSpecialButtons[MENU_BUTTON]; }
bool BleMultiHIDConfiguration::getIncludeHome() { return _whichSpecialButtons[HOME_BUTTON]; }
bool BleMultiHIDConfiguration::getIncludeBack() { return _whichSpecialButtons[BACK_BUTTON]; }
bool BleMultiHIDConfiguration::getIncludeVolumeInc() { return _whichSpecialButtons[VOLUME_INC_BUTTON]; }
bool BleMultiHIDConfiguration::getIncludeVolumeDec() { return _whichSpecialButtons[VOLUME_DEC_BUTTON]; }
bool BleMultiHIDConfiguration::getIncludeVolumeMute() { return _whichSpecialButtons[VOLUME_MUTE_BUTTON]; }
const bool *BleMultiHIDConfiguration::getWhichSpecialButtons() const { return _whichSpecialButtons; }
bool BleMultiHIDConfiguration::getIncludeXAxis() { return _whichAxes[X_AXIS]; }
bool BleMultiHIDConfiguration::getIncludeYAxis() { return _whichAxes[Y_AXIS]; }
bool BleMultiHIDConfiguration::getIncludeZAxis() { return _whichAxes[Z_AXIS]; }
bool BleMultiHIDConfiguration::getIncludeRxAxis() { return _whichAxes[RX_AXIS]; }
bool BleMultiHIDConfiguration::getIncludeRyAxis() { return _whichAxes[RY_AXIS]; }
bool BleMultiHIDConfiguration::getIncludeRzAxis() { return _whichAxes[RZ_AXIS]; }
bool BleMultiHIDConfiguration::getIncludeSlider1() { return _whichAxes[SLIDER1]; }
bool BleMultiHIDConfiguration::getIncludeSlider2() { return _whichAxes[SLIDER2]; }
const bool *BleMultiHIDConfiguration::getWhichAxes() const { return _whichAxes; }
bool BleMultiHIDConfiguration::getIncludeRudder() { return _whichSimulationControls[RUDDER]; }
bool BleMultiHIDConfiguration::getIncludeThrottle() { return _whichSimulationControls[THROTTLE]; }
bool BleMultiHIDConfiguration::getIncludeAccelerator() { return _whichSimulationControls[ACCELERATOR]; }
bool BleMultiHIDConfiguration::getIncludeBrake() { return _whichSimulationControls[BRAKE]; }
bool BleMultiHIDConfiguration::getIncludeSteering() { return _whichSimulationControls[STEERING]; }
const bool *BleMultiHIDConfiguration::getWhichSimulationControls() const { return _whichSimulationControls; }
char *BleMultiHIDConfiguration::getModelNumber(){ return _modelNumber; }
char *BleMultiHIDConfiguration::getSoftwareRevision(){ return _softwareRevision; }
char *BleMultiHIDConfiguration::getSerialNumber(){ return _serialNumber; }
char *BleMultiHIDConfiguration::getFirmwareRevision(){ return _firmwareRevision; }
char *BleMultiHIDConfiguration::getHardwareRevision(){ return _hardwareRevision; }

// Mouse
uint8_t BleMultiHIDConfiguration::getMouseHidReportId() { return _mouseHIDReportId; }
bool BleMultiHIDConfiguration::getUseMouse(){ return _useMouse; }
uint16_t BleMultiHIDConfiguration::getMouseButtonCount(){ return _mouseButtonCount; }
uint16_t BleMultiHIDConfiguration::getMouseAxisCount(){ return _mouseAxisCount; }

void BleMultiHIDConfiguration::setWhichSpecialButtons(bool start, bool select, bool menu, bool home, bool back, bool volumeInc, bool volumeDec, bool volumeMute)
{
    _whichSpecialButtons[START_BUTTON] = start;
    _whichSpecialButtons[SELECT_BUTTON] = select;
    _whichSpecialButtons[MENU_BUTTON] = menu;
    _whichSpecialButtons[HOME_BUTTON] = home;
    _whichSpecialButtons[BACK_BUTTON] = back;
    _whichSpecialButtons[VOLUME_INC_BUTTON] = volumeInc;
    _whichSpecialButtons[VOLUME_DEC_BUTTON] = volumeDec;
    _whichSpecialButtons[VOLUME_MUTE_BUTTON] = volumeMute;
}

void BleMultiHIDConfiguration::setWhichAxes(bool xAxis, bool yAxis, bool zAxis, bool rxAxis, bool ryAxis, bool rzAxis, bool slider1, bool slider2)
{
    _whichAxes[X_AXIS] = xAxis;
    _whichAxes[Y_AXIS] = yAxis;
    _whichAxes[Z_AXIS] = zAxis;
    _whichAxes[RX_AXIS] = rxAxis;
    _whichAxes[RY_AXIS] = ryAxis;
    _whichAxes[RZ_AXIS] = rzAxis;
    _whichAxes[SLIDER1] = slider1;
    _whichAxes[SLIDER2] = slider2;
}

void BleMultiHIDConfiguration::setWhichSimulationControls(bool rudder, bool throttle, bool accelerator, bool brake, bool steering)
{
    _whichSimulationControls[RUDDER] = rudder;
    _whichSimulationControls[THROTTLE] = throttle;
    _whichSimulationControls[ACCELERATOR] = accelerator;
    _whichSimulationControls[BRAKE] = brake;
    _whichSimulationControls[STEERING] = steering;
}

void BleMultiHIDConfiguration::setControllerType(uint8_t value) { _controllerType = value; }
void BleMultiHIDConfiguration::setGamepadHidReportId(uint8_t value) { _gamepadHIDReportId = value; }
void BleMultiHIDConfiguration::setButtonCount(uint16_t value) { _buttonCount = value; }
void BleMultiHIDConfiguration::setHatSwitchCount(uint8_t value) { _hatSwitchCount = value; }
void BleMultiHIDConfiguration::setAutoReport(bool value) { _autoReport = value; }
void BleMultiHIDConfiguration::setIncludeStart(bool value) { _whichSpecialButtons[START_BUTTON] = value; }
void BleMultiHIDConfiguration::setIncludeSelect(bool value) { _whichSpecialButtons[SELECT_BUTTON] = value; }
void BleMultiHIDConfiguration::setIncludeMenu(bool value) { _whichSpecialButtons[MENU_BUTTON] = value; }
void BleMultiHIDConfiguration::setIncludeHome(bool value) { _whichSpecialButtons[HOME_BUTTON] = value; }
void BleMultiHIDConfiguration::setIncludeBack(bool value) { _whichSpecialButtons[BACK_BUTTON] = value; }
void BleMultiHIDConfiguration::setIncludeVolumeInc(bool value) { _whichSpecialButtons[VOLUME_INC_BUTTON] = value; }
void BleMultiHIDConfiguration::setIncludeVolumeDec(bool value) { _whichSpecialButtons[VOLUME_DEC_BUTTON] = value; }
void BleMultiHIDConfiguration::setIncludeVolumeMute(bool value) { _whichSpecialButtons[VOLUME_MUTE_BUTTON] = value; }
void BleMultiHIDConfiguration::setIncludeXAxis(bool value) { _whichAxes[X_AXIS] = value; }
void BleMultiHIDConfiguration::setIncludeYAxis(bool value) { _whichAxes[Y_AXIS] = value; }
void BleMultiHIDConfiguration::setIncludeZAxis(bool value) { _whichAxes[Z_AXIS] = value; }
void BleMultiHIDConfiguration::setIncludeRxAxis(bool value) { _whichAxes[RX_AXIS] = value; }
void BleMultiHIDConfiguration::setIncludeRyAxis(bool value) { _whichAxes[RY_AXIS] = value; }
void BleMultiHIDConfiguration::setIncludeRzAxis(bool value) { _whichAxes[RZ_AXIS] = value; }
void BleMultiHIDConfiguration::setIncludeSlider1(bool value) { _whichAxes[SLIDER1] = value; }
void BleMultiHIDConfiguration::setIncludeSlider2(bool value) { _whichAxes[SLIDER2] = value; }
void BleMultiHIDConfiguration::setIncludeRudder(bool value) { _whichSimulationControls[RUDDER] = value; }
void BleMultiHIDConfiguration::setIncludeThrottle(bool value) { _whichSimulationControls[THROTTLE] = value; }
void BleMultiHIDConfiguration::setIncludeAccelerator(bool value) { _whichSimulationControls[ACCELERATOR] = value; }
void BleMultiHIDConfiguration::setIncludeBrake(bool value) { _whichSimulationControls[BRAKE] = value; }
void BleMultiHIDConfiguration::setIncludeSteering(bool value) { _whichSimulationControls[STEERING] = value; }
void BleMultiHIDConfiguration::setVid(uint16_t value) { _vid = value; }
void BleMultiHIDConfiguration::setPid(uint16_t value) { _pid = value; }
void BleMultiHIDConfiguration::setGuidVersion(uint16_t value) { _guidVersion = value; }
void BleMultiHIDConfiguration::setAxesMin(int16_t value) { _axesMin = value; }
void BleMultiHIDConfiguration::setAxesMax(int16_t value) { _axesMax = value; }
void BleMultiHIDConfiguration::setSimulationMin(int16_t value) { _simulationMin = value; }
void BleMultiHIDConfiguration::setSimulationMax(int16_t value) { _simulationMax = value; }
void BleMultiHIDConfiguration::setModelNumber(char *value) { _modelNumber = value; }
void BleMultiHIDConfiguration::setSoftwareRevision(char *value) { _softwareRevision = value; }
void BleMultiHIDConfiguration::setSerialNumber(char *value) { _serialNumber = value; }
void BleMultiHIDConfiguration::setFirmwareRevision(char *value) { _firmwareRevision = value; }
void BleMultiHIDConfiguration::setHardwareRevision(char *value) { _hardwareRevision = value; }

// Mouse
void BleMultiHIDConfiguration::setMouseHidReportId(uint8_t value) { _mouseHIDReportId = value; }
void BleMultiHIDConfiguration::setUseMouse(bool value) { _useMouse = value; }
void BleMultiHIDConfiguration::setMouseButtonCount(uint16_t value) { _mouseButtonCount = value; }
void BleMultiHIDConfiguration::setMouseAxisCount(uint16_t value) { _mouseAxisCount = value; }

// Keyboard 
void BleMultiHIDConfiguration::setUseKeyboard(bool value) { _useKeyboard = value; }