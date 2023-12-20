#include <BleConnectionStatus.h>

#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

int ledPin = 5; // LED connected to digital pin 13

XboxGamepadDevice *gamepad;
BleCompositeHID* compositeHID;

void OnVibrateEvent(XboxGamepadOutputReportData data)
{
    Serial.println("Vibrate event DC enable: " + String(data.dcEnableActuators) + " Magnitude: " + String(data.magnitude) + " Duration: " + String(data.duration) + " Start delay: " + String(data.startDelay) + " Loop count: " + String(data.loopCount));
}

void setup()
{
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT); // sets the digital pin as output

    // Set up gamepad
    XboxGamepadDeviceConfiguration gamepadConfig(0x01);
    gamepad = new XboxGamepadDevice(gamepadConfig);
    
    FunctionSlot<XboxGamepadOutputReportData> ptrSlot(OnVibrateEvent);
    gamepad->onVibrate.attach(ptrSlot);

    // Set up composite HID device
    compositeHID = new BleCompositeHID("Xbox Wireless Controller", "Microsoft", 100);
    
    // Add both devices to the composite HID device to manage them
    compositeHID->addDevice(gamepad);

    // Fake a xbox controller
    BLEHostConfiguration config;
    config.setVid(0x045E); // Vendor: Microsoft
    
    // Product: Xbox One Wireless Controller - Model 1708 pre 2021 firmware
    // Specifically picked since it provides rumble support on linux kernels earlier than 6.5
    config.setPid(0x02fd); 
    config.setSerialNumber("3033363030343037323136373239");

    // Start the composite HID device to broadcast HID reports
    Serial.println("Starting composite HID device...");
    compositeHID->begin(config);
    Serial.println("Composite HID device started");
}

void loop()
{
    // Test gamepad
    digitalWrite(ledPin, LOW);

    delay(1000);

    digitalWrite(ledPin, HIGH);

    delay(1000);
}