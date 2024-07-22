#ifndef ESP32_BLE_CONNECTION_STATUS_H
#define ESP32_BLE_CONNECTION_STATUS_H
#include "sdkconfig.h"
#if defined(CONFIG_BT_ENABLED)

#include "nimconfig.h"
#if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)

#include <NimBLEServer.h>
#include "NimBLECharacteristic.h"

class BleConnectionStatus : public NimBLEServerCallbacks
{
public:
    BleConnectionStatus(void);
    bool connected = false;
    void onConnect(NimBLEServer *pServer, ble_gap_conn_desc* desc);
    void onDisconnect(NimBLEServer *pServer);

    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, std::string& name) override;
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
    void onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo) override;
    uint32_t onPassKeyDisplay() override;
    void onConfirmPIN(const NimBLEConnInfo& connInfo, uint32_t pin) override;
    void onAuthenticationComplete(const NimBLEConnInfo& connInfo) override;
    void onAuthenticationComplete(const NimBLEConnInfo& connInfo, const std::string& name) override;
    void onIdentity(const NimBLEConnInfo& connInfo) override;
};

#endif // CONFIG_BT_NIMBLE_ROLE_PERIPHERAL
#endif // CONFIG_BT_ENABLED
#endif // ESP32_BLE_CONNECTION_STATUS_H
