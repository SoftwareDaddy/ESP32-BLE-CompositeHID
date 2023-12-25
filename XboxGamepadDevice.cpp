#include "XboxGamepadDevice.h"
#include "BleCompositeHID.h"


#if defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define LOG_TAG "XboxGamepadDevice"
#else
#include "esp_log.h"
static const char *LOG_TAG = "XboxGamepadDevice";
#endif

XboxGamepadCallbacks::XboxGamepadCallbacks(XboxGamepadDevice* device) : _device(device)
{
}

void XboxGamepadCallbacks::onWrite(NimBLECharacteristic* pCharacteristic)
{    
    // An example packet we might receive from XInput might look like 0x0300002500ff00ff
    uint64_t value = pCharacteristic->getValue<uint64_t>();

    // ESP32 is little endian
    XboxGamepadOutputReportData vibrationData;
    vibrationData.dcEnableActuators = (value & 0xFF);
    vibrationData.leftTriggerMagnitude = (value >> 8) & 0xFF;
    vibrationData.rightTriggerMagnitude = (value >> 16) & 0xFF;
    vibrationData.weakMotorMagnitude = (value >> 24) & 0xFF;
    vibrationData.strongMotorMagnitude = (value >> 32) & 0xFF;
    vibrationData.duration = (value >> 40) & 0xFF;
    vibrationData.startDelay = (value >> 48) & 0xFF;
    vibrationData.loopCount = (value >> 56) & 0xFF;
    
    ESP_LOGD(LOG_TAG, "XboxGamepadCallbacks::onWrite, Size: %d, DC enable: %d, magnitudeWeak: %d, magnitudeStrong: %d, duration: %d, start delay: %d, loop count: %d", 
        pCharacteristic->getValue().size(),
        vibrationData.dcEnableActuators, 
        vibrationData.weakMotorMagnitude, 
        vibrationData.strongMotorMagnitude, 
        vibrationData.duration, 
        vibrationData.startDelay, 
        vibrationData.loopCount
    );

    _device->onVibrate.fire(vibrationData);
}

void XboxGamepadCallbacks::onRead(NimBLECharacteristic* pCharacteristic)
{
    ESP_LOGD(LOG_TAG, "XboxGamepadCallbacks::onRead");
}

void XboxGamepadCallbacks::onNotify(NimBLECharacteristic* pCharacteristic)
{
    ESP_LOGD(LOG_TAG, "XboxGamepadCallbacks::onNotify");
}

void XboxGamepadCallbacks::onStatus(NimBLECharacteristic* pCharacteristic, Status status, int code)
{
    ESP_LOGD(LOG_TAG, "XboxGamepadCallbacks::onStatus, status: %d, code: %d", status, code);
}

// XboxGamepadDevice methods
XboxGamepadDevice::XboxGamepadDevice(XboxGamepadDeviceConfiguration* config) :
    _config(config),
    _extra_input(nullptr),
    _callbacks(nullptr)
{
}

XboxGamepadDevice::~XboxGamepadDevice() {
    if (getOutput() && _callbacks){
        getOutput()->setCallbacks(nullptr);
        delete _callbacks;
    }

    if(_extra_input){
        delete _extra_input;
    }
}

void XboxGamepadDevice::init(NimBLEHIDDevice* hid) {
    /// Create input characteristic to send events to the computer
    auto input = hid->inputReport(XBOX_INPUT_REPORT_ID);
    //_extra_input = hid->inputReport(XBOX_EXTRA_INPUT_REPORT_ID);

    // Create output characteristic to handle events coming from the computer
    auto output = hid->outputReport(XBOX_OUTPUT_REPORT_ID);
    _callbacks = new XboxGamepadCallbacks(this);
    output->setCallbacks(_callbacks);

    setCharacteristics(input, output);
}

BaseCompositeDeviceConfiguration* XboxGamepadDevice::getDeviceConfig() {
    // Return the device configuration
    return _config;
}

void XboxGamepadDevice::resetInputs() {
    memset(&_inputReport, 0, sizeof(XboxGamepadInputReportData));
}

void XboxGamepadDevice::press(uint16_t button) {
    // Avoid double presses
    if (!isPressed(button))
    {
        _inputReport.buttons |= button;
        if (_config->getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::release(uint16_t button) {
    // Avoid double presses
    if (isPressed(button))
    {
        _inputReport.buttons ^= button;
        if (_config->getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

bool XboxGamepadDevice::isPressed(uint16_t button) {
    return (bool)(_inputReport.buttons & button);
}

void XboxGamepadDevice::setLeftThumb(int16_t x, int16_t y) {
    x = constrain(x, XBOX_STICK_MIN, XBOX_STICK_MAX);
    y = constrain(y, XBOX_STICK_MIN, XBOX_STICK_MAX);

    if(_inputReport.x != x || _inputReport.y != y){
        _inputReport.x = (uint16_t)(x + 0x8000);
        _inputReport.y = (uint16_t)(y + 0x8000);
        if (_config->getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::setRightThumb(int16_t z, int16_t rZ) {
    z = constrain(z, XBOX_STICK_MIN, XBOX_STICK_MAX);
    rZ = constrain(rZ, XBOX_STICK_MIN, XBOX_STICK_MAX);

    if(_inputReport.z != z || _inputReport.rz != rZ){
        _inputReport.z = (uint16_t)(z + 0x8000);
        _inputReport.rz = (uint16_t)(rZ+ 0x8000);
        if (_config->getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::setLeftTrigger(uint16_t value) {
    value = constrain(value, XBOX_TRIGGER_MIN, XBOX_TRIGGER_MAX);

    if (_inputReport.brake != value) {
        _inputReport.brake = value;
        if (_config->getAutoReport()) {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::setRightTrigger(uint16_t value) {
    value = constrain(value, XBOX_TRIGGER_MIN, XBOX_TRIGGER_MAX);

    if (_inputReport.accelerator != value) {
        _inputReport.accelerator = value;
        if (_config->getAutoReport()) {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::setTriggers(uint16_t left, uint16_t right) {
    left = constrain(left, XBOX_TRIGGER_MIN, XBOX_TRIGGER_MAX);
    right = constrain(right, XBOX_TRIGGER_MIN, XBOX_TRIGGER_MAX);

    if (_inputReport.brake != left || _inputReport.accelerator != right) {
        _inputReport.brake = left;
        _inputReport.accelerator = right;
        if (_config->getAutoReport()) {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::pressDPadDirection(uint8_t direction) {
    // Avoid double presses
    if (!isDPadPressed(direction))
    {
        _inputReport.hat |= direction;
        if (_config->getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::releaseDPadDirection(uint8_t direction) {
    if (isDPadPressed(direction))
    {
        _inputReport.hat ^= direction;
        if (_config->getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

bool XboxGamepadDevice::isDPadPressed(uint8_t direction) {
    return (bool)(_inputReport.hat & direction);
}

void XboxGamepadDevice::pressShare() {
    // Avoid double presses
    if (!(_inputReport.share & XBOX_BUTTON_SHARE))
    {
        _inputReport.share |= XBOX_BUTTON_SHARE;
        if (_config->getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::releaseShare() {
    if (_inputReport.share & XBOX_BUTTON_SHARE)
    {
        _inputReport.share ^= XBOX_BUTTON_SHARE;
        if (_config->getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::sendGamepadReport(){
    auto input = getInput();
    auto parentDevice = this->getParent();

    if (!input || !parentDevice)
        return;

    if(!parentDevice->isConnected())
        return;

    size_t packedSize = _config->getDeviceReportSize();
    uint8_t packedData[packedSize] = {
        (uint8_t)(_inputReport.x & 0xff), (uint8_t)(_inputReport.x >> 8),
        (uint8_t)(_inputReport.y & 0xff), (uint8_t)(_inputReport.y >> 8),
        (uint8_t)(_inputReport.z & 0xff), (uint8_t)(_inputReport.z >> 8),
        (uint8_t)(_inputReport.rz & 0xff), (uint8_t)(_inputReport.rz >> 8),
        (uint8_t)(_inputReport.brake & 0xff), (uint8_t)(_inputReport.brake >> 8),
        (uint8_t)(_inputReport.accelerator & 0xff), (uint8_t)(_inputReport.accelerator >> 8),
        (uint8_t)_inputReport.hat,
        (uint8_t)(_inputReport.buttons & 0xff), (uint8_t)(_inputReport.buttons >> 8),
        (uint8_t)_inputReport.share
    };

    input->setValue(packedData, packedSize);
    input->notify();
}
