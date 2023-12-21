#include <BleConnectionStatus.h>

#include <BleCompositeHID.h>
#include <XboxGamepadDevice.h>

int ledPin = 5; // LED connected to digital pin 13

XboxGamepadDevice *gamepad;
BleCompositeHID* compositeHID;

void OnVibrateEvent(XboxGamepadOutputReportData data)
{
    if(data.weakMotorMagnitude > 0 || data.strongMotorMagnitude > 0){
        digitalWrite(ledPin, LOW);
    } else {
        digitalWrite(ledPin, HIGH);
    }
    Serial.println("Vibration event. Weak motor: " + String(data.weakMotorMagnitude) + " Strong motor: " + String(data.strongMotorMagnitude));
}

void setup()
{
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT); // sets the digital pin as output

    // Set up gamepad
    gamepad = new XboxGamepadDevice();
    
    // Set up vibration event handler
    FunctionSlot<XboxGamepadOutputReportData> vibrationSlot(OnVibrateEvent);
    gamepad->onVibrate.attach(vibrationSlot);

    // Set up composite HID device to hold our controller device
    compositeHID = new BleCompositeHID("XInput Gamepad + Mouse", "EspressIF", 100);

    // Add all child devices to the top-level composite HID device to manage them
    compositeHID->addDevice(gamepad);

    // The composite HID device pretends to be a valid Xbox controller via vendor and product IDs (VID/PID).
    // Platforms like windows/linux need this in order to pick an XInput driver,
    // rather than using the generic BLE GATT HID driver. 
    BLEHostConfiguration config = XboxGamepadDevice::getFakedHostConfiguration();
    
    // Start the composite HID device to broadcast HID reports
    Serial.println("Starting composite HID device...");
    compositeHID->begin(config);
}

void loop()
{
    // Test each button
    uint16_t buttons[] = {
        XBOX_BUTTON_A, 
        XBOX_BUTTON_B, 
        XBOX_BUTTON_X, 
        XBOX_BUTTON_Y, 
        XBOX_BUTTON_LB, 
        XBOX_BUTTON_RB, 
        XBOX_BUTTON_START,
        XBOX_BUTTON_HOME,
        XBOX_BUTTON_LS, 
        XBOX_BUTTON_RS
    };
    for (uint16_t button : buttons)
    {
        Serial.println("Pressing button " + String(button));
        gamepad->press(button);
        gamepad->sendGamepadReport();
        delay(1000);
        gamepad->release(button);
        gamepad->sendGamepadReport();
        delay(500);
    }

    // The select button is a seperate call since it doesn't live in the same 
    // bitflag as the rest of the buttons
    gamepad->pressSelect();
    gamepad->sendGamepadReport();
    delay(1000);
    gamepad->releaseSelect();
    gamepad->sendGamepadReport();
    delay(500);
}