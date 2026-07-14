
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>

#include "ble_services.h"

static int stored_value_temp, stored_value_hum;
static bool indicate_temp_enabled, notify_temp_enabled, notify_hum_enabled;

static struct bt_gatt_indicate_params ind_params;

extern double prepare_ble_temp(void);
extern double prepare_ble_hum(void);

static int get_temp_text(char * buffer, double temp) 
{
	stored_value_temp = (int) temp;
	sprintf(buffer, "%dC\n", stored_value_temp);
	return strlen(buffer);
}

static int get_hum_text(char * buffer, double hum) 
{
	stored_value_hum = (int) hum;
	sprintf(buffer, "%d%%\n", stored_value_hum);
	return strlen(buffer);
}

static ssize_t read_cb_temp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			   void *buf, uint16_t len, uint16_t offset)
{
    char value[100];
	int size;

	size = get_temp_text(value, prepare_ble_temp());
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, size);
}

static ssize_t read_cb_hum(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			   void *buf, uint16_t len, uint16_t offset)
{
    char value[100];
	int size;

    size = get_hum_text(value, prepare_ble_hum());
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, size);
}

static void temp_update_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	indicate_temp_enabled = (value == BT_GATT_CCC_INDICATE);
}

static void temp_update_indicate_cb(struct bt_conn *conn, struct bt_gatt_indicate_params *params, uint8_t error) {
	printk("Indication %s\n", error != 0U ? "fail" : "successs");
}

static void temp_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	notify_temp_enabled = (value == BT_GATT_CCC_NOTIFY);
}

static void hum_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	notify_hum_enabled = (value == BT_GATT_CCC_NOTIFY);
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
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       SAMPLE_BT_PERM_READ,
			       read_cb_temp, NULL, NULL),
	BT_GATT_CCC(temp_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_TEMP_UPDATE_CHAR,
			       BT_GATT_CHRC_INDICATE,
			       SAMPLE_BT_PERM_READ,
			       NULL, NULL, NULL),
	BT_GATT_CCC(temp_update_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_CHARACTERISTIC(BT_UUID_HUM_CHAR,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       SAMPLE_BT_PERM_READ,
			       read_cb_hum, NULL, NULL),
	BT_GATT_CCC(hum_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

#define BT_GATT_SERVICE_FIND_BY_UUID(service, value) bt_gatt_find_by_uuid(service.attrs, service.attr_count, value)  

static struct bt_gatt_attr * temp_attr;
static struct bt_gatt_attr * temp_update_attr;
static struct bt_gatt_attr * hum_attr;

void ble_gatt_init(void) 
{
	temp_attr = BT_GATT_SERVICE_FIND_BY_UUID(primary_service, BT_UUID_TEMP_CHAR);
	temp_update_attr = BT_GATT_SERVICE_FIND_BY_UUID(primary_service, BT_UUID_TEMP_UPDATE_CHAR);
	hum_attr = BT_GATT_SERVICE_FIND_BY_UUID(primary_service, BT_UUID_HUM_CHAR);

	ind_params.attr = temp_update_attr;
	ind_params.func = temp_update_indicate_cb;
	ind_params.destroy = NULL;
}

int send_temp_state_indicate(char data) 
{
	if (!indicate_temp_enabled) {
		return -EACCES;
	}
	
	ind_params.data = &data;
	ind_params.len = sizeof(data);
	return bt_gatt_indicate(NULL, &ind_params);
}

int send_temp_notify(double temp) 
{
 	char value[100];
	int size;
    
	if (!notify_temp_enabled) {
		return -EACCES;
	}

	size = get_temp_text(value, temp);
	return bt_gatt_notify(NULL, temp_attr, value, size);
}


int send_hum_notify(double hum) 
{
 	char value[100];
	int size;

	if (!notify_hum_enabled) {
		return -EACCES;
	}
    
	size = get_hum_text(value, hum);
	return bt_gatt_notify(NULL, hum_attr, value, size);
}
