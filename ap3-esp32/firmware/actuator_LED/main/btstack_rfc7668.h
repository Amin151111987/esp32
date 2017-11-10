
// spp_and_le_counter.h generated from ../../example/spp_and_le_counter.gatt for BTstack

// binary representation
// attribute size in bytes (16), flags(16), handle (16), uuid (16/128), value(...)

#include <stdint.h>
#include <freertos/event_groups.h>
#include <bluetooth.h>

EventGroupHandle_t lowpan_ble_flags;

#define LOWPAN_BLE_FLAG_CONNECTED (1<<0)

#define LOWPAN_BLE_TAG "BTSTACK-RFC7668"

#define LOWPAN_BLE_DEBUG_L2CAP_CB 0

/** 
 * Get the current remote BT address
 */
bd_addr_t* getBTRemoteAddress(void);

/** 
 * Get the current host BT address
 */
bd_addr_t * getBTLocalAddress(void);

/**
 * Get the current local host address string (IPv6)
 * */
char* getLocalIPv6Address(void);

/**
 * Get the current remote (BT host) address string (IPv6)
 * ONLY VALID AFTER CONNECTED!
 * */
char* getRemoteIPv6Address(void);
