#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include <NimBLEHIDDevice.h>
#include <HIDTypes.h>
#include <HIDKeyboardTypes.h>
#include <driver/adc.h>
#include "sdkconfig.h"

#include "BleConnectionStatus.h"
#include "BleCompositeHID.h"

#if defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define LOG_TAG "BLECompositeHID"
#else
#include "esp_log.h"
static const char *LOG_TAG = "BLECompositeHID";
#endif

#define SERVICE_UUID_DEVICE_INFORMATION        "180A"      // Service - Device information

#define CHARACTERISTIC_UUID_MODEL_NUMBER       "2A24"      // Characteristic - Model Number String - 0x2A24
#define CHARACTERISTIC_UUID_SOFTWARE_REVISION  "2A28"      // Characteristic - Software Revision String - 0x2A28
#define CHARACTERISTIC_UUID_SERIAL_NUMBER      "2A25"      // Characteristic - Serial Number String - 0x2A25
#define CHARACTERISTIC_UUID_FIRMWARE_REVISION  "2A26"      // Characteristic - Firmware Revision String - 0x2A26
#define CHARACTERISTIC_UUID_HARDWARE_REVISION  "2A27"      // Characteristic - Hardware Revision String - 0x2A27


// uint8_t tempHidReportDescriptor[150];
// int hidReportDescriptorSize = 0;

uint16_t vid;
uint16_t pid;
uint16_t guidVersion;
uint16_t axesMin;
uint16_t axesMax;
uint16_t simulationMin;
uint16_t simulationMax;
std::string modelNumber;
std::string softwareRevision;
std::string serialNumber;
std::string firmwareRevision;
std::string hardwareRevision;

BleCompositeHID::BleCompositeHID(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel) : hid(nullptr)
{
    this->deviceName = deviceName;
    this->deviceManufacturer = deviceManufacturer;
    this->batteryLevel = batteryLevel;
    this->connectionStatus = new BleConnectionStatus();
}

void BleCompositeHID::begin(BleCompositeHIDConfiguration *config)
{
    configuration = *config; // we make a copy, so the user can't change actual values midway through operation, without calling the begin function again

    modelNumber = configuration.getModelNumber();
    softwareRevision = configuration.getSoftwareRevision();
    serialNumber = configuration.getSerialNumber();
    firmwareRevision = configuration.getFirmwareRevision();
    hardwareRevision = configuration.getHardwareRevision();

	vid = configuration.getVid();
	pid = configuration.getPid();
	guidVersion = configuration.getGuidVersion();

	uint8_t high = highByte(vid);
	uint8_t low = lowByte(vid);

	vid = low << 8 | high;

	high = highByte(pid);
	low = lowByte(pid);

	pid = low << 8 | high;
	
	high = highByte(guidVersion);
	low = lowByte(guidVersion);
	guidVersion = low << 8 | high;

    xTaskCreate(this->taskServer, "server", 20000, (void *)this, 5, NULL);
}

void BleCompositeHID::end(void)
{
}

bool BleCompositeHID::isConnected(void)
{
    return this->connectionStatus->connected;
}

void BleCompositeHID::setBatteryLevel(uint8_t level)
{
    this->batteryLevel = level;
    if (hid != 0)
    {
        this->hid->setBatteryLevel(this->batteryLevel);

        if (this->isConnected())
        {
            this->hid->batteryLevel()->notify();
        }
		
        if (configuration.getAutoReport())
        {
            sendGamepadReport();
        }
    }
}

void BleCompositeHID::taskServer(void *pvParameter)
{
    BleCompositeHID *BleCompositeHIDInstance = (BleCompositeHID *)pvParameter; // static_cast<BleCompositeHID *>(pvParameter);

    // Use the procedure below to set a custom Bluetooth MAC address
    // Compiler adds 0x02 to the last value of board's base MAC address to get the BT MAC address, so take 0x02 away from the value you actually want when setting
    //uint8_t newMACAddress[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF - 0x02};
    //esp_base_mac_addr_set(&newMACAddress[0]); // Set new MAC address 
    
    NimBLEDevice::init(BleCompositeHIDInstance->deviceName);
    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(BleCompositeHIDInstance->connectionStatus);

    BleCompositeHIDInstance->hid = new NimBLEHIDDevice(pServer);
    
    // Set up gamepad HID device
    BleCompositeHIDInstance->inputGamepad = BleCompositeHIDInstance->hid->inputReport(BleCompositeHIDInstance->configuration.getGamepadHidReportId()); // <-- input REPORTID from report map
    BleCompositeHIDInstance->connectionStatus->inputGamepad = BleCompositeHIDInstance->inputGamepad;

    // Set up mouse HID device
    BleCompositeHIDInstance->inputMouse = BleCompositeHIDInstance->hid->inputReport(BleCompositeHIDInstance->configuration.getMouseHidReportId()); // <-- input REPORTID from report map
    BleCompositeHIDInstance->connectionStatus->inputMouse = BleCompositeHIDInstance->inputMouse;

    BleCompositeHIDInstance->hid->manufacturer()->setValue(BleCompositeHIDInstance->deviceManufacturer);

    NimBLEService *pService = pServer->getServiceByUUID(SERVICE_UUID_DEVICE_INFORMATION);
	
	BLECharacteristic* pCharacteristic_Model_Number = pService->createCharacteristic(
      CHARACTERISTIC_UUID_MODEL_NUMBER,
      NIMBLE_PROPERTY::READ
    );
    pCharacteristic_Model_Number->setValue(modelNumber);
	
	BLECharacteristic* pCharacteristic_Software_Revision = pService->createCharacteristic(
      CHARACTERISTIC_UUID_SOFTWARE_REVISION,
      NIMBLE_PROPERTY::READ
    );
    pCharacteristic_Software_Revision->setValue(softwareRevision);
	
	BLECharacteristic* pCharacteristic_Serial_Number = pService->createCharacteristic(
      CHARACTERISTIC_UUID_SERIAL_NUMBER,
      NIMBLE_PROPERTY::READ
    );
    pCharacteristic_Serial_Number->setValue(serialNumber);
	
	BLECharacteristic* pCharacteristic_Firmware_Revision = pService->createCharacteristic(
      CHARACTERISTIC_UUID_FIRMWARE_REVISION,
      NIMBLE_PROPERTY::READ
    );
    pCharacteristic_Firmware_Revision->setValue(firmwareRevision);
	
	BLECharacteristic* pCharacteristic_Hardware_Revision = pService->createCharacteristic(
      CHARACTERISTIC_UUID_HARDWARE_REVISION,
      NIMBLE_PROPERTY::READ
    );
    pCharacteristic_Hardware_Revision->setValue(hardwareRevision);

    BleCompositeHIDInstance->hid->pnp(0x01, vid, pid, guidVersion);
    BleCompositeHIDInstance->hid->hidInfo(0x00, 0x01);

    NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND);

    uint8_t *customHidReportDescriptor = new uint8_t[hidReportDescriptorSize];
    memcpy(customHidReportDescriptor, tempHidReportDescriptor, hidReportDescriptorSize);

    // Testing
    for (int i = 0; i < hidReportDescriptorSize; i++)
       Serial.printf("%02x", customHidReportDescriptor[i]);

    BleCompositeHIDInstance->hid->reportMap((uint8_t *)customHidReportDescriptor, hidReportDescriptorSize);
    BleCompositeHIDInstance->hid->startServices();

    BleCompositeHIDInstance->onStarted(pServer);

    NimBLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(GENERIC_HID);
    pAdvertising->addServiceUUID(BleCompositeHIDInstance->hid->hidService()->getUUID());
    pAdvertising->start();
    BleCompositeHIDInstance->hid->setBatteryLevel(BleCompositeHIDInstance->batteryLevel);

    ESP_LOGD(LOG_TAG, "Advertising started!");
    vTaskDelay(portMAX_DELAY); // delay(portMAX_DELAY);
}
