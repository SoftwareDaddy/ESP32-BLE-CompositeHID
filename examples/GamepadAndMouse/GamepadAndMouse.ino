#include <BleConnectionStatus.h>

#include <BleCompositeHID.h>

#include <GamepadDevice.h>
#include <GamepadConfiguration.h>


int ledPin = 5;              // LED connected to digital pin 13

GamepadDevice* gamepad;
BleCompositeHID compositeHID;

void setup() {
  pinMode(ledPin, OUTPUT);    // sets the digital pin as output
  
  GamepadConfiguration gamepadConfig;
  gamepadConfig.setButtonCount(8);
  gamepadConfig.setHatSwitchCount(0);
  gamepad = new GamepadDevice(gamepadConfig);

  compositeHID.addDevice(gamepad);
  compositeHID.begin();
}

void loop() {
  digitalWrite(ledPin, HIGH); // sets the LED on
  delay(1000);                // waits for a second
  digitalWrite(ledPin, LOW);  // sets the LED off
  delay(1000);                // waits for a second
}