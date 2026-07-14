
#include <zephyr/bluetooth/uuid.h>

#define BT_UUID_SERVICE_VAL \
	BT_UUID_128_ENCODE(0x10001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

#define BT_UUID_TEMP_CHAR_VAL \
	BT_UUID_128_ENCODE(0x10001524, 0x1212, 0xefde, 0x1523, 0x785feabcd123)
	
#define BT_UUID_TEMP_UPDATE_CHAR_VAL \
	BT_UUID_128_ENCODE(0x10001524, 0x1213, 0xefde, 0x1523, 0x785feabcd123)

#define BT_UUID_HUM_CHAR_VAL \
  BT_UUID_128_ENCODE(0x10001525, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

  
#define BT_UUID_SERVICE BT_UUID_DECLARE_128(BT_UUID_SERVICE_VAL)
#define BT_UUID_TEMP_CHAR BT_UUID_DECLARE_128(BT_UUID_TEMP_CHAR_VAL)
#define BT_UUID_TEMP_UPDATE_CHAR BT_UUID_DECLARE_128(BT_UUID_TEMP_UPDATE_CHAR_VAL)
#define BT_UUID_HUM_CHAR BT_UUID_DECLARE_128(BT_UUID_HUM_CHAR_VAL)

extern double prepare_ble_temp(void);
extern double prepare_ble_hum(void);

void ble_gatt_init(void);

int send_temp_state_indicate(char temp_state);
int send_temp_notify(double temp);
int send_hum_notify(double hum);