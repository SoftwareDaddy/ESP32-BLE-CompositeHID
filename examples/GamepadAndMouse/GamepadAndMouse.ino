#include <BleConnectionStatus.h>

#include <BleCompositeHID.h>

#include <GamepadDevice.h>
#include <MouseDevice.h>

#include <GamepadConfiguration.h>
#include <MouseConfiguration.h>

int ledPin = 5;              // LED connected to digital pin 13

GamepadDevice* gamepad;
MouseDevice* mouse;
BleCompositeHID compositeHID;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);    // sets the digital pin as output
  
  // Set up gamepad
  GamepadConfiguration gamepadConfig;
  gamepadConfig.setButtonCount(8);
  gamepadConfig.setHatSwitchCount(0);
  gamepad = new GamepadDevice(gamepadConfig);
  Serial.println("Created gamepad device");

  // Set up mouse
  MouseConfiguration mouseConfig;
  mouse = new MouseDevice(mouseConfig);
  Serial.println("Created mouse device");

  // Add both devices to the composite HID device to manage them
  compositeHID.addDevice(gamepad);
  compositeHID.addDevice(mouse);

  // Start the composite HID device to broadcast HID reports
  Serial.println("Starting composite HID device...");
  compositeHID.begin();
  Serial.println("Composite HID device started");
}

void loop() {
  // Test gamepad
  digitalWrite(ledPin, LOW);
  
  gamepad->press(BUTTON_1);
  mouse->mousePress(MOUSE_LOGICAL_RIGHT_BUTTON);
  delay(1000);               
  
  gamepad->release(BUTTON_1);
  mouse->mouseRelease(MOUSE_LOGICAL_RIGHT_BUTTON);
  digitalWrite(ledPin, HIGH);

  delay(1000);
}