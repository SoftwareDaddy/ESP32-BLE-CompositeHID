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
    compositeHID = new BleCompositeHID("CompositeHID Xbox Controller", "Mystfit", 100);

    // Add all child devices to the top-level composite HID device to manage them
    compositeHID->addDevice(gamepad);

    // The composite HID device pretends to be a valid Xbox controller via vendor and product IDs (VID/PID).
    // Platforms like windows/linux need this in order to pick an XInput driver over the generic BLE GATT HID driver. 
    BLEHostConfiguration config = gamepad->getDeviceConfig()->getIdealHostConfiguration();
    
    Serial.println("Setting VID source to " + String(config.getVidSource(), HEX));
    Serial.println("Setting VID to " + String(config.getVid(), HEX));
    Serial.println("Setting PID to " + String(config.getPid(), HEX));
    Serial.println("Setting GUID version to " + String(config.getGuidVersion(), HEX));
    Serial.println("Setting serial number to " + String(config.getSerialNumber()));

    // Start the composite HID device to broadcast HID reports
    Serial.println("Starting composite HID device...");
    compositeHID->begin(config);
}

void loop()
{
    testButtons();
    testPads();
    testTriggers();
    testThumbsticks();
}

void testButtons(){
    // Test each button
    uint16_t buttons[] = {
        XBOX_BUTTON_A, 
        XBOX_BUTTON_B, 
        XBOX_BUTTON_X, 
        XBOX_BUTTON_Y, 
        XBOX_BUTTON_LB, 
        XBOX_BUTTON_RB, 
        XBOX_BUTTON_START,
        XBOX_BUTTON_SELECT,
        XBOX_BUTTON_HOME,
        XBOX_BUTTON_LS, 
        XBOX_BUTTON_RS
    };
    for (uint16_t button : buttons)
    {
        Serial.println("Pressing button " + String(button));
        gamepad->press(button);
        gamepad->sendGamepadReport();
        delay(500);
        gamepad->release(button);
        gamepad->sendGamepadReport();
        delay(100);
    }

    // The share button is a seperate call since it doesn't live in the same 
    // bitflag as the rest of the buttons
    gamepad->pressShare();
    gamepad->sendGamepadReport();
    delay(500);
    gamepad->releaseShare();
    gamepad->sendGamepadReport();
    delay(100);
}

void testPads(){
    uint16_t directions[] = {
        XBOX_BUTTON_DPAD_NORTH,
        XBOX_BUTTON_DPAD_NORTHEAST,
        XBOX_BUTTON_DPAD_EAST,
        XBOX_BUTTON_DPAD_SOUTHEAST,
        XBOX_BUTTON_DPAD_SOUTH,
        XBOX_BUTTON_DPAD_SOUTHWEST,
        XBOX_BUTTON_DPAD_WEST,
        XBOX_BUTTON_DPAD_NORTHWEST
    };

    for (uint16_t direction : directions)
    {
        Serial.println("Pressing DPad: " + String(direction));
        gamepad->pressDPadDirection(direction);
        gamepad->sendGamepadReport();
        delay(500);
        gamepad->releaseDPadDirection(direction);
        gamepad->sendGamepadReport();
        delay(100);
    }
}

void testTriggers(){
    for(int16_t val = XBOX_TRIGGER_MIN; val <= XBOX_TRIGGER_MAX; val++){
        if(val % 8 == 0)
            Serial.println("Setting trigger value to " + String(val));
        gamepad->setLeftTrigger(val);
        gamepad->setRightTrigger(val);
        gamepad->sendGamepadReport();
        delay(10);
    }
}

void testThumbsticks(){
    int startTime = millis();
    int reportCount = 0;
    while(millis() - startTime < 8000){
        reportCount++;
        int16_t x = cos((float)millis() / 1000.0f) * XBOX_STICK_MAX;
        int16_t y = sin((float)millis() / 1000.0f) * XBOX_STICK_MAX;

        gamepad->setLeftThumb(x, y);
        gamepad->setRightThumb(x, y);
        gamepad->sendGamepadReport();
        
        if(reportCount % 8 == 0)
            Serial.println("Setting left thumb to " + String(x) + ", " + String(y));
            
        delay(10);
    }
}