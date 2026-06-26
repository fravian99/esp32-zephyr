#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/services/ans.h>
#include <zephyr/bluetooth/uuid.h>

#if defined(CONFIG_REBOOT)
#include <zephyr/sys/reboot.h>
#endif

#include <string.h>

#define ADV_PARAM BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_IDENTITY, \
    BT_GAP_ADV_SLOW_INT_MIN,                                  \
    BT_GAP_ADV_SLOW_INT_MAX, NULL)

#define BT_UUID_SERVICE_VAL \
	BT_UUID_128_ENCODE(0x10001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

#define BT_UUID_TEMP_CHAR_VAL \
	BT_UUID_128_ENCODE(0x10001524, 0x1212, 0xefde, 0x1523, 0x785feabcd123)
	

#define BT_UUID_HUM_CHAR_VAL \
  BT_UUID_128_ENCODE(0x10001525, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

  
#define BT_UUID_SERVICE BT_UUID_DECLARE_128(BT_UUID_SERVICE_VAL)
#define BT_UUID_TEMP_CHAR BT_UUID_DECLARE_128(BT_UUID_TEMP_CHAR_VAL)
#define BT_UUID_HUM_CHAR BT_UUID_DECLARE_128(BT_UUID_HUM_CHAR_VAL)

static int stored_value_temp, stored_value_hum;

extern double prepare_ble_temp(void);
extern double prepare_ble_hum(void);

static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static ssize_t read_cb_temp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			   void *buf, uint16_t len, uint16_t offset)
{
    char value[100];
    stored_value_temp = (int)prepare_ble_temp();

	sprintf(value, "%dC\n", stored_value_temp);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 strlen(value));
}

static ssize_t read_cb_hum(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			   void *buf, uint16_t len, uint16_t offset)
{
    char value[100];
    stored_value_hum = (int)prepare_ble_hum();

	sprintf(value, "%d%%\n", stored_value_hum);
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(stored_value_hum));
}


#if CONFIG_SAMPLE_BT_USE_AUTHENTICATION && defined(CONFIG_BT_SMP)
/* Require encryption using authenticated link-key. */
#define SAMPLE_BT_PERM_READ BT_GATT_PERM_READ_AUTHEN
#define SAMPLE_BT_PERM_WRITE BT_GATT_PERM_WRITE_AUTHEN
#elif defined(CONFIG_BT_SMP)
/* Require encryption. */
#define SAMPLE_BT_PERM_READ BT_GATT_PERM_READ_ENCRYPT
#define SAMPLE_BT_PERM_WRITE BT_GATT_PERM_WRITE_ENCRYPT
#else 
#define SAMPLE_BT_PERM_READ BT_GATT_PERM_READ
#define SAMPLE_BT_PERM_WRITE BT_GATT_PERM_WRITE
#endif

BT_GATT_SERVICE_DEFINE(primary_service,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_SERVICE),
	BT_GATT_CHARACTERISTIC(BT_UUID_TEMP_CHAR,
			       BT_GATT_CHRC_READ,
			       SAMPLE_BT_PERM_READ,
			       read_cb_temp, NULL, NULL),
	BT_GATT_CHARACTERISTIC(BT_UUID_HUM_CHAR,
			       BT_GATT_CHRC_READ,
			       SAMPLE_BT_PERM_READ,
			       read_cb_hum, NULL, NULL),
);

static struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	/*BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		      BT_UUID_16_ENCODE(BT_UUID_HRS_VAL),
		      BT_UUID_16_ENCODE(BT_UUID_BAS_VAL),
		      BT_UUID_16_ENCODE(BT_UUID_CTS_VAL)),*/
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1), // Device Name
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SERVICE_VAL),
};


void start_adv(void)
{
    int err;

    err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err != 0) {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }

    printk("Advertising successfully started\n");
}

void connected(struct bt_conn* conn, uint8_t err)
{
    if (err) {
        printk("Connection failed, err 0x%02x %s\n", err, bt_hci_err_to_str(err));
    } else {
        printk("Connected\n");
    }
}

void disconnected(struct bt_conn* conn, uint8_t reason)
{
    printk("Disconnected, reason 0x%02x %s\n", reason, bt_hci_err_to_str(reason));
}

static void recycled(void)
{
	start_adv();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
    .recycled = recycled 
};

#if defined(CONFIG_BT_SMP)
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	printk("Passkey for %s: %06u\n", addr, passkey);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
	printk("Pairing cancelled: %s\n", addr);
}


static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
    .cancel = auth_cancel,
};

void pairing_complete(struct bt_conn *conn, bool bonded)
{
#if defined(CONFIG_REBOOT)
	printk("Pairing completed. Rebooting in 5 seconds...\n");
	k_sleep(K_SECONDS(5));
	sys_reboot(SYS_REBOOT_WARM);
#else
    printk("Pairing completed.\n");
#endif
}

static struct bt_conn_auth_info_cb bt_conn_auth_info = {
	.pairing_complete = pairing_complete
};
#endif



int bte_enable(void)
{
   
    int err = bt_enable(NULL);
    if (err != 0) {
        printk("Enabling failed (err %d)\n", err);
        return err;
    }
#if defined(CONFIG_BT_SMP)
    bt_conn_auth_cb_register(&auth_cb_display);
    bt_conn_auth_info_cb_register(&bt_conn_auth_info);
#endif
    return 0;
}

