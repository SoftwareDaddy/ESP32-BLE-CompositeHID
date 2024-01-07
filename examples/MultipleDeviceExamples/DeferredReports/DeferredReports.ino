// This example demonstrates how device reports can be deferred to be sent later 
// in order to avoid overloading the BLE connection.
// You can send queued reports manually by calling the sendDeferredReports() function,
// or send them regularly by using the setThreadedAutoSend(true) option

#include <BleCompositeHID.h>
#include <KeyboardDevice.h>
#include <MouseDevice.h>
#include <GamepadDevice.h>


GamepadDevice* gamepad = nullptr;
KeyboardDevice* keyboard = nullptr;
MouseDevice* mouse = nullptr;
BleCompositeHID compositeHID("CompositeHID Keyboard Mouse Gamepad", "Mystfit", 100);

int reportCount = 0;

void setup() {
    Serial.begin(115200);

    // To use deferred reports, we can either set the autoDefer flag to true in the configuration object,
    // or we can call each send[DEVICETYPE]Report() function with the defer parameter set to true.

    // Set up gamepad
    GamepadConfiguration gamepadConfig;
    gamepadConfig.setAutoDefer(true);
    gamepad = new GamepadDevice(gamepadConfig);
  
    // Set up keyboard
    KeyboardConfiguration keyboardConfig;
    keyboardConfig.setAutoDefer(true);
    keyboard = new KeyboardDevice(keyboardConfig);

    // Set up mouse
    MouseConfiguration mouseConfig;
    mouseConfig.setAutoDefer(true);
    mouse = new MouseDevice(mouseConfig);

     // Add all devices to the composite HID device to manage them
    compositeHID.addDevice(keyboard);
    compositeHID.addDevice(mouse);
    compositeHID.addDevice(gamepad);

    // We can also let our composite HID device send reports at a regular interval.
    // This works by creating a background task that will periodically send all queued reports.
    BLEHostConfiguration config;
    config.setDeferSendRate(240);
    config.setThreadedAutoSend(true);

    // Start the composite HID device to broadcast HID reports
    compositeHID.begin(config);

    delay(3000);
}

void loop() {
    if(compositeHID.isConnected())
    {        
        bool gamepadPressed = false;
        bool keyPressed = false;

        // Test mouse
        mouse->mouseMove(
            round(cos((float)millis() / 100.0f) * 10.0f), 
            round(sin((float)millis() / 100.0f) * 10.0f)
        );

        // Test keyboard
        if(reportCount % 1000 == 0){
            keyPressed = !keyPressed;
            if(keyPressed){
                keyboard->keyPress(KEY_A);
            } else {
                keyboard->keyRelease(KEY_A);
            }
        }

        // Test gamepad
        if(reportCount % 1000 == 0){
            gamepadPressed = !gamepadPressed;
            if(gamepadPressed){
                gamepad->press(BUTTON_1);
            } else {
                gamepad->release(BUTTON_1);
            }
        }

        // Since this example uses the threaded auto send option, you can uncomment this
        // line if you want to control when deferred reports are sent manually.
        
        //compositeHID.sendDeferredReports();
        reportCount++;
        delay(2);
    }
}