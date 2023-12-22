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

// XboxGamepadDeviceConfiguration methods
XboxGamepadDeviceConfiguration::XboxGamepadDeviceConfiguration(uint8_t reportId) : BaseCompositeDeviceConfiguration(reportId) {
    
}

uint8_t XboxGamepadDeviceConfiguration::getDeviceReportSize() {
    // Return the size of the device report
    
    // Input
    // 2 * 16bit (2 bytes) for X and Y inclusive                = 4 bytes
    // 2 * 16bit (2 bytes) for Z and Rz inclusive               = 4 bytes
    // 1 * 10bit for brake + 6bit padding (2 bytes)             = 2 bytes
    // 1 * 10bit for accelerator + 6bit padding (2 bytes)       = 2 bytes
    // 1 * 4bit for hat switch + 4bit padding (1 byte)          = 1 byte
    // 15 * 1bit for buttons + 1bit padding (2 bytes)           = 2 bytes
    // 1 * 1bit for record button + 7bit padding (1 byte)       = 1 byte

    // Additional input?
    // 1 * 1bit + 7bit padding (1 byte)                         = 1 byte

    // TODO: Split output size into seperate function
    // Output 
    // 1 * 4bit for DC Enable Actuators + 4bit padding (1 byte) = 1 byte
    // 4 * 8bit for Magnitude (4 bytes)                         = 4 bytes
    // 3 * 8bit for Duration, Start Delay, Loop Count (3 bytes) = 3 bytes

    return sizeof(XboxGamepadInputReportData);
}

size_t XboxGamepadDeviceConfiguration::makeDeviceReport(uint8_t* buffer, size_t bufferSize) {
    ESP_LOGE(LOG_TAG, "Before xbox descriptor alloc");
    
    if(sizeof(XboxOneS_1914_HIDDescriptor) < bufferSize){
        memcpy(buffer, XboxOneS_1914_HIDDescriptor, sizeof(XboxOneS_1914_HIDDescriptor));
    } else {
        return -1;
    }
    
    return sizeof(XboxOneS_1914_HIDDescriptor);
}

// XboxGamepadDevice methods
XboxGamepadDevice::XboxGamepadDevice(const XboxGamepadDeviceConfiguration& config) :
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

BLEHostConfiguration XboxGamepadDevice::getFakedHostConfiguration() {
    // Fake a xbox controller
    BLEHostConfiguration config;

    // Vendor: Microsoft
    config.setVid(0x1949); 
    
    // Product: Xbox One Wireless Controller - Model 1708 pre 2021 firmware
    // Specifically picked since it provides rumble support on linux kernels earlier than 6.5
    config.setPid(0x041A); 
    config.setGuidVersion(0x0101);

    // Serial: Probably don't need this
    config.setSerialNumber(XBOX_1914_SERIAL);

    return config;
}

void XboxGamepadDevice::init(NimBLEHIDDevice* hid) {
    /// Create input characteristic to send events to the computer
    auto input = hid->inputReport(XBOX_INPUT_REPORT_ID);
    _extra_input = hid->inputReport(XBOX_EXTRA_INPUT_REPORT_ID);

    // Create output characteristic to handle events coming from the computer
    auto output = hid->outputReport(XBOX_OUTPUT_REPORT_ID);
    _callbacks = new XboxGamepadCallbacks(this);
    output->setCallbacks(_callbacks);

    setCharacteristics(input, output);
}

BaseCompositeDeviceConfiguration* XboxGamepadDevice::getDeviceConfig() {
    // Return the device configuration
    return &_config;
}

void XboxGamepadDevice::resetInputs() {
    memset(&_inputReport, 0, sizeof(XboxGamepadInputReportData));
}

void XboxGamepadDevice::press(uint16_t button) {
    // Avoid double presses
    if (!isPressed(button))
    {
        _inputReport.buttons |= button;
        if (_config.getAutoReport())
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
        if (_config.getAutoReport())
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
        if (_config.getAutoReport())
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
        if (_config.getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::setLeftTrigger(uint16_t value) {
    value = constrain(value, XBOX_TRIGGER_MIN, XBOX_TRIGGER_MAX);

    if (_inputReport.brake != value) {
        _inputReport.brake = value;
        if (_config.getAutoReport()) {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::setRightTrigger(uint16_t value) {
    value = constrain(value, XBOX_TRIGGER_MIN, XBOX_TRIGGER_MAX);

    if (_inputReport.accelerator != value) {
        _inputReport.accelerator = value;
        if (_config.getAutoReport()) {
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
        if (_config.getAutoReport()) {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::pressDPadDirection(uint8_t direction) {
    // Avoid double presses
    if (!isDPadPressed(direction))
    {
        _inputReport.hat |= direction;
        if (_config.getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::releaseDPadDirection(uint8_t direction) {
    if (isDPadPressed(direction))
    {
        _inputReport.hat ^= direction;
        if (_config.getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

bool XboxGamepadDevice::isDPadPressed(uint8_t direction) {
    return (bool)(_inputReport.hat & direction);
}

void XboxGamepadDevice::pressSelect() {
    // Avoid double presses
    if (!(_inputReport.select & XBOX_BUTTON_SELECT))
    {
        _inputReport.select |= XBOX_BUTTON_SELECT;
        if (_config.getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void XboxGamepadDevice::releaseSelect() {
    if (_inputReport.select & XBOX_BUTTON_SELECT)
    {
        _inputReport.select ^= XBOX_BUTTON_SELECT;
        if (_config.getAutoReport())
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

    uint8_t packedData[_config.getDeviceReportSize()] = {
        _inputReport.x & 0xff, _inputReport.x >> 8,
        _inputReport.y & 0xff, _inputReport.y >> 8,
        _inputReport.z & 0xff, _inputReport.z >> 8,
        _inputReport.rz & 0xff, _inputReport.rz >> 8,
        _inputReport.brake & 0xff, _inputReport.brake >> 8,
        _inputReport.accelerator & 0xff, _inputReport.accelerator >> 8,
        _inputReport.hat,
        _inputReport.buttons & 0xff, _inputReport.buttons >> 8,
        _inputReport.select
    };

    input->setValue(packedData, sizeof(packedData));
    input->notify();
}
