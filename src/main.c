#include "internal_temp.h"
#include "rgb_led.h"
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

static const struct device* const internal_temp_dev = DEVICE_DT_GET(DT_ALIAS(die_temp));
static const struct device* const temp_dev = DEVICE_DT_GET(DT_ALIAS(temp_sensor));
static const struct device* const rgb_led_dev = DEVICE_DT_GET(DT_ALIAS(rgb_led));

static colors_t get_color_by_temp(double temp);

int main(void)
{

    double temp = 0., hum = 0.;
    if (!device_is_ready(internal_temp_dev)) {
        printk("Temperature sensor is not ready\n");
        return 1;
    }
    if (!device_is_ready(temp_dev)) {
        printk("Temperature sensor is not ready\n");
        return 1;
    }
    if (!device_is_ready(rgb_led_dev)) {
        printk("RGB Led is not ready\n");
        return 1;
    }

    while (1) {
        if (get_internal_temp(internal_temp_dev, &temp, true)) {
            return 1;
        }
        if (get_temp(temp_dev, &temp, true)) {
            return 1;
        }
        if (rgb_led_turn_on(rgb_led_dev, get_color_by_temp(temp))) {
            return 1;
        }
        if (get_hum(temp_dev, &hum, false)) {
            return 1;
        }
        k_msleep(500);
    }
    return 0;
}


static colors_t get_color_by_temp(double temp) {
    if (temp <= CONFIG_LOW_TEMP) {
        return BLUE;
    }
    if (temp >= CONFIG_HIGH_TEMP) {
        return RED;
    }
    return GREEN;
}