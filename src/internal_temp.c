#include "internal_temp.h"
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

int get_internal_temp(const struct device* internal_temp_dev, double* temp_pt)
{
    int rc;
    struct sensor_value val;

    rc = sensor_sample_fetch(internal_temp_dev);
    if (rc) {
        printk("Failed to fetch sample (%d)\n", rc);
        return 1;
    }

    rc = sensor_channel_get(internal_temp_dev, SENSOR_CHAN_DIE_TEMP, &val);
    if (rc) {
        printk("Failed to get data (%d)\n", rc);
        return 1;
    }

    *temp_pt = sensor_value_to_double(&val);
    printk("Temperatura interna: %.1f ºC\n", *temp_pt);
    return 0;
}

int get_temp(const struct device* temp_dev, double* temp_pt)
{
    int rc;
    struct sensor_value val;

    rc = sensor_sample_fetch(temp_dev);
    if (rc) {
        printk("Failed to fetch sample (%d)\n", rc);
        return 1;
    }

    rc = sensor_channel_get(temp_dev, SENSOR_CHAN_AMBIENT_TEMP, &val);
    if (rc) {
        printk("Failed to get data (%d)\n", rc);
        return 1;
    }

    *temp_pt = sensor_value_to_double(&val);
    printk("Temperatura: %.1f ºC\n", *temp_pt);
    return 0;
}
