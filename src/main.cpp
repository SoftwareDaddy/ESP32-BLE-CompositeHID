#include "BleConnectionStatus.h"
#include "BleCompositeHID.h"
#include "XboxGamepadDevice.h"
#include <esp_log.h>
#include <math.h>
#include <string>
#include <chrono>

static const char* LOG_TAG = "main";

int ledPin = 5; // LED connected to digital pin 13

XboxGamepadDevice *gamepad;
BleCompositeHID compositeHID("CompositeHID XInput Controller", "Mystfit", 100);

extern "C" {void app_main(void);}

void OnVibrateEvent(XboxGamepadOutputReportData data)
{
    if(data.weakMotorMagnitude > 0 || data.strongMotorMagnitude > 0){
        // digitalWrite(ledPin, LOW);
        ESP_LOGI(LOG_TAG, "OnVibrateEvent - low");
    } else {
        // digitalWrite(ledPin, HIGH);
        ESP_LOGI(LOG_TAG, "OnVibrateEvent - high");
    }
    ESP_LOGI(LOG_TAG, "Vibration event. Weak motor: %d Strong motor: %d",data.weakMotorMagnitude, data.strongMotorMagnitude);
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
        //XBOX_BUTTON_HOME,   // Uncomment this to test the hom/guide button. Steam will flip out and enter big picture mode when running this sketch though so be warned!
        XBOX_BUTTON_LS, 
        XBOX_BUTTON_RS
    };
    for (uint16_t button : buttons)
    {
        ESP_LOGI(LOG_TAG, "Pressing button %d", button);
        gamepad->press(button);
        gamepad->sendGamepadReport();
        vTaskDelay(500);
        gamepad->resetInputs();
        //gamepad->release(button);
        gamepad->sendGamepadReport();
        vTaskDelay(100);
    }

    // The share button is a seperate call since it doesn't live in the same 
    // bitflag as the rest of the buttons
    gamepad->pressShare();
    gamepad->sendGamepadReport();
    vTaskDelay(500);
    gamepad->releaseShare();
    gamepad->sendGamepadReport();
    vTaskDelay(100);
}

void testPads(){
    XboxDpadFlags directions[] = {
        XboxDpadFlags::NORTH,
        XboxDpadFlags((uint8_t)XboxDpadFlags::NORTH | (uint8_t)XboxDpadFlags::EAST),
        XboxDpadFlags::EAST,
        XboxDpadFlags((uint8_t)XboxDpadFlags::EAST | (uint8_t)XboxDpadFlags::SOUTH),
        XboxDpadFlags::SOUTH,
        XboxDpadFlags((uint8_t)XboxDpadFlags::SOUTH | (uint8_t)XboxDpadFlags::WEST),
        XboxDpadFlags::WEST,
        XboxDpadFlags((uint8_t)XboxDpadFlags::WEST | (uint8_t)XboxDpadFlags::NORTH)
    };

    for (XboxDpadFlags direction : directions)
    {
        ESP_LOGI(LOG_TAG, "Pressing DPad: %d", direction);
        gamepad->pressDPadDirectionFlag(direction);
        gamepad->sendGamepadReport();
        vTaskDelay(500);
        gamepad->releaseDPad();
        gamepad->sendGamepadReport();
        vTaskDelay(100);
    }
}

void testTriggers(){
    for(int16_t val = XBOX_TRIGGER_MIN; val <= XBOX_TRIGGER_MAX; val++){
        if(val % 8 == 0)
        {
            ESP_LOGI(LOG_TAG, "Setting trigger value to %d", val);
        }
        gamepad->setLeftTrigger(val);
        gamepad->setRightTrigger(val);
        gamepad->sendGamepadReport();
        vTaskDelay(10);
    }
}

void testThumbsticks(){
    auto now = std::chrono::system_clock::now();
    auto startMillisecond = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    auto currentMillisecond = startMillisecond;
    int reportCount = 0;
    while(currentMillisecond - startMillisecond < 8000){
        reportCount++;
        int16_t x = cos((float)currentMillisecond / 1000.0f) * XBOX_STICK_MAX;
        int16_t y = sin((float)currentMillisecond / 1000.0f) * XBOX_STICK_MAX;

        gamepad->setLeftThumb(x, y);
        gamepad->setRightThumb(x, y);
        gamepad->sendGamepadReport();
        
        if(reportCount % 8 == 0)
            ESP_LOGI(LOG_TAG, "Setting left thumb to %d, %d", x, y);
            
        vTaskDelay(10);
        currentMillisecond = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
}

void app_main(void) {
    XboxOneSControllerDeviceConfiguration* config = new XboxOneSControllerDeviceConfiguration();
    BLEHostConfiguration hostConfig = config->getIdealHostConfiguration();
    ESP_LOGI(LOG_TAG, "Using VID source: %d", hostConfig.getVidSource());
    ESP_LOGI(LOG_TAG, "Using VID: %d", hostConfig.getVid());
    ESP_LOGI(LOG_TAG, "Using PID: %d", hostConfig.getPid());
    ESP_LOGI(LOG_TAG, "Using GUID version: %d", hostConfig.getGuidVersion());
    ESP_LOGI(LOG_TAG, "Using serial number: %s", hostConfig.getSerialNumber());

    // Set up gamepad
    gamepad = new XboxGamepadDevice(config);
    
    // Set up vibration event handler
    FunctionSlot<XboxGamepadOutputReportData> vibrationSlot(OnVibrateEvent);
    gamepad->onVibrate.attach(vibrationSlot);

    // Add all child devices to the top-level composite HID device to manage them
    compositeHID.addDevice(gamepad);

    // Start the composite HID device to broadcast HID reports
    ESP_LOGI(LOG_TAG, "Starting composite HID device...");
    compositeHID.begin(hostConfig);

    while(1){
        if(compositeHID.isConnected()){
            testButtons();
            // testPads();
            // testTriggers();
            // testThumbsticks();
        }
        vTaskDelay(200);
    }
}