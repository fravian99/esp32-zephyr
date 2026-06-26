#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/services/ans.h>
#include <zephyr/bluetooth/uuid.h>

int bte_enable(void);
void start_adv(void);


void connected(struct bt_conn* conn, uint8_t err);
void disconnected(struct bt_conn* conn, uint8_t reason);

extern double prepare_ble_temp(void);
extern double prepare_ble_hum(void);