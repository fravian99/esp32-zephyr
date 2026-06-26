#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

static int get_data_sensor(const struct device * sensor_dev, enum sensor_channel chan, double * data_pt, bool with_fetch) {
    int rc;
    struct sensor_value val;

    if (with_fetch) {
        rc = sensor_sample_fetch(sensor_dev);
        if (rc) {
            printk("Failed to fetch sample (%d)\n", rc);
            return 1;
        }
    }

    rc = sensor_channel_get(sensor_dev, chan, &val);
    if (rc) {
        printk("Failed to get data (%d)\n", rc);
        return 1;
    }

    *data_pt = sensor_value_to_double(&val);
    return 0;
} 

int get_internal_temp(const struct device* internal_temp_dev, double* temp_pt, bool with_fetch)
{
    int err = get_data_sensor(internal_temp_dev, SENSOR_CHAN_DIE_TEMP, temp_pt, with_fetch);
    if (err) {
        return 1;
    }
    return 0;
}

int get_temp(const struct device* temp_dev, double* temp_pt, bool with_fetch)
{
    int err = get_data_sensor(temp_dev, SENSOR_CHAN_AMBIENT_TEMP, temp_pt, with_fetch);
    if (err) {
        return 1;
    }
    return 0;
}

int get_hum(const struct device* hum_dev, double* hum_pt, bool with_fetch)
{
    int err = get_data_sensor(hum_dev, SENSOR_CHAN_HUMIDITY, hum_pt, with_fetch);
    if (err) {
        return 1;
    }
    return 0;
}