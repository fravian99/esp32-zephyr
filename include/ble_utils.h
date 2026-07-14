#include <zephyr/bluetooth/conn.h>

int bte_enable(void);
void start_adv(void);

void connected(struct bt_conn* conn, uint8_t err);
void disconnected(struct bt_conn* conn, uint8_t reason);
