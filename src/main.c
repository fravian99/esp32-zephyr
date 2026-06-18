#include "internal_temp.h"
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static const struct device* const internal_temp_dev = DEVICE_DT_GET(DT_ALIAS(die_temp));
static const struct device* const temp_dev = DEVICE_DT_GET(DT_ALIAS(temp_sensor));

int main(void)
{

    double temp = 0.;
    if (!device_is_ready(internal_temp_dev)) {
        printk("Temperature sensor is not ready\n");
        return 1;
    }
    if (!device_is_ready(temp_dev)) {
        printk("Temperature sensor is not ready\n");
        return 1;
    }

    while (1) {
        if (get_internal_temp(internal_temp_dev, &temp)) {
            return 1;
        }
        if (get_temp(temp_dev, &temp)) {
            return 1;
        }
        k_msleep(500);
    }
    return 0;
}
