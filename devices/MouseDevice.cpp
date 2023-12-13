#include "MouseDevice.h"

MouseDevice::MouseDevice(MouseConfiguration* config, NimBLECharacteristic* inputCharacteristic = nullptr, NimBLECharacteristic* outputCharacteristic = nullptr):
    config(*config), // Copy config to avoid modification
    _input(inputCharacteristic),
    _output(outputCharacteristic),
    _mouseButtons(),
    _mouseX(0),
    _mouseY(0),
    _mouseWheel(0),
    _mouseHWheel(0)
{
    this->resetButtons();
}

void MouseDevice::resetButtons()
{
    memset(&_mouseButtons, 0, sizeof(_mouseButtons));
}

// Mouse
void MouseDevice::mouseClick(uint8_t button)
{
    // No-op?
}

void MouseDevice::mousePress(uint8_t button)
{
    uint8_t index = (button - 1) / 8;
    uint8_t bit = (button - 1) % 8;
    uint8_t bitmask = (1 << bit);

    uint8_t result = _mouseButtons[index] | bitmask;

    if (result != _mouseButtons[index])
    {
        _mouseButtons[index] = result;
    }

    if (_config.getAutoReport())
    {
        sendMouseReport();
    }
}

void MouseDevice::mouseRelease(uint8_t button)
{
    uint8_t index = (button - 1) / 8;
    uint8_t bit = (button - 1) % 8;
    uint8_t bitmask = (1 << bit);

    uint64_t result = _mouseButtons[index] & ~bitmask;

    if (result != _mouseButtons[index])
    {
        _mouseButtons[index] = result;
    }

    if (_config.getAutoReport())
    {
        sendMouseReport();
    }
}

void MouseDevice::mouseMove(signed char x, signed char y, signed char scrollX, signed char scrollY)
{
    if (x == -127)
    {
        x = -126;
    }
    if (y == -127)
    {
        y = -126;
    }
    if (scrollX == -127)
    {
        scrollX = -126;
    }
    if (scrollY == -127)
    {
        scrollY = -126;
    }

    _mouseX = x;
    _mouseY = y;
    _mouseWheel = scrollY;
    _mouseHWheel = scrollX;

    if (_config.getAutoReport())
    {
        sendMouseReport();
    }
}

void MouseDevice::sendMouseReport()
{
    if (!_input || !this->isConnected())
        return;
    
    uint8_t mouse_report[_config.getDeviceReportSize()];
    uint8_t currentReportIndex = 0;

    memset(&mouse_report, 0, sizeof(mouse_report));
    memcpy(&mouse_report, &_mouseButtons, sizeof(_mouseButtons));
    currentReportIndex += numOfMouseButtonBytes;

    // TODO: Make dynamic based on axis counts
    if (configuration.getMouseAxisCount() > 0)
    {
        mouse_report[currentReportIndex++] = _mouseX;
        mouse_report[currentReportIndex++] = _mouseY;
        mouse_report[currentReportIndex++] = _mouseWheel;
        mouse_report[currentReportIndex++] = _mouseHWheel;
    }

    this->_input->setValue(mouse_report, sizeof(mouse_report));
    this->_input->notify();
}