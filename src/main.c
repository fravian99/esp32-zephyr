

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "internal_temp.h"
#include "rgb_led.h"

#if defined(CONFIG_BT_PERIPHERAL)
#include "ble_utils.h"
#include "ble_services.h"
#endif

static const struct device* const internal_temp_dev = DEVICE_DT_GET(DT_ALIAS(die_temp));
static const struct device* const temp_dev = DEVICE_DT_GET(DT_ALIAS(temp_sensor));
static const struct device* const rgb_led_dev = DEVICE_DT_GET(DT_ALIAS(rgb_led));

static double internal_temp = 0., temp = 0., old_temp = 0., hum = 0.;

static colors_t get_color_by_temp(double temp);
static void temp_changed(double actual_temp, double old_temp);

int main(void)
{
    printk("Hello\n");
    
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

    if (rgb_led_turn_on(rgb_led_dev, NONE)) {
        return 1;
    }

#if defined(CONFIG_BT_PERIPHERAL)
    bte_enable();
    start_adv();
#endif
    while (1) {
        if (get_internal_temp(internal_temp_dev, &internal_temp, true)) {
            return 1;
        }
        if (get_temp(temp_dev, &temp, true)) {
            return 1;
        }
        if (get_hum(temp_dev, &hum, false)) {
            return 1;
        }

        if (rgb_led_turn_on(rgb_led_dev, get_color_by_temp(temp))) {
            return 1;
        }
#if !defined(CONFIG_BT_PERIPHERAL)
        printk("Temperatura interna: %.1f ºC\n", internal_temp);
        printk("Temperatura: %.1f ºC\n", temp);
        printk("Humedad: %.1f \n", hum);
#else
        temp_changed(temp, old_temp);
        send_temp_notify(temp);
        send_hum_notify(hum);
#endif
        old_temp = temp;
        k_msleep(500);
    }
    return 0;
}

static void temp_changed(double actual_temp, double old_temp) 
{
    bool has_increased, has_decreased;
    has_increased = actual_temp > old_temp;
    has_decreased = actual_temp < old_temp;
    if (has_increased) {
        send_temp_state_indicate('+');
    } else if (has_decreased) {
        send_temp_state_indicate('-');
    } 
}

double prepare_ble_temp(void) {
    return temp;
}

double prepare_ble_hum(void) {
    return hum;
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