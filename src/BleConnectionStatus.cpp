#include "BleConnectionStatus.h"
#include "esp_log.h"
#include <NimBLEDevice.h>
static const char *LOG_TAG = "BleConnectionStatus";

BleConnectionStatus::BleConnectionStatus(void)
{
}

void BleConnectionStatus::onConnect(NimBLEServer *pServer, ble_gap_conn_desc* desc)
{
    ESP_LOGI(LOG_TAG, "***BleConnectionStatus::onConnect\n");
    pServer->updateConnParams(desc->conn_handle, 6, 7, 0, 600);
    this->connected = true;
}

void BleConnectionStatus::onDisconnect(NimBLEServer *pServer)
{
    ESP_LOGI(LOG_TAG, "***BleConnectionStatus::onDisconnect\n");
    this->connected = false;
}

void BleConnectionStatus::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
    ESP_LOGI(LOG_TAG, "onConnect()");
    ESP_LOGI(LOG_TAG, "***BleConnectionStatus::onConnect\n");
    pServer->updateConnParams(connInfo.getConnHandle(), 6, 7, 0, 600);
    this->connected = true;
} // onConnect

void BleConnectionStatus::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, std::string& name) {
    ESP_LOGI(LOG_TAG, "onConnect()");
} // onConnect

void BleConnectionStatus::onDisconnect(NimBLEServer* pServer,
                                         NimBLEConnInfo& connInfo, int reason) {
    ESP_LOGI(LOG_TAG, "onDisconnect()");
    this->connected = false;
} // onDisconnect

void BleConnectionStatus::onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo) {
    ESP_LOGI(LOG_TAG, "onMTUChange()");
} // onMTUChange

uint32_t BleConnectionStatus::onPassKeyDisplay(){
    ESP_LOGI(LOG_TAG, "onPassKeyDisplay");
    return 123456;
} //onPassKeyDisplay

void BleConnectionStatus::onConfirmPIN(const NimBLEConnInfo& connInfo, uint32_t pin){
    NimBLEDevice::injectConfirmPIN(connInfo, true);
} // onConfirmPIN

void BleConnectionStatus::onIdentity(const NimBLEConnInfo& connInfo){
    ESP_LOGI(LOG_TAG, "onIdentity");
} // onIdentity

void BleConnectionStatus::onAuthenticationComplete(const NimBLEConnInfo& connInfo){
    ESP_LOGI(LOG_TAG, "onAuthenticationComplete");
} // onAuthenticationComplete

void BleConnectionStatus::onAuthenticationComplete(const NimBLEConnInfo& connInfo, const std::string& name){
    NIMBLE_LOGD(LOG_TAG, "onAuthenticationComplete");
} // onAuthenticationComplete