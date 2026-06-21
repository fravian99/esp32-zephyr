#include <zephyr/sys/printk.h>
#include <zephyr/drivers/led_strip.h>

#include <zephyr/sys/util.h>
#include "rgb_led.h"

#define STRIP_NUM_PIXELS 1
#define RGB_SET(rgb_color, _r, _g, _b) rgb_color.r = (_r); rgb_color.g = (_g); rgb_color.b = (_b);

static struct led_rgb pixels[STRIP_NUM_PIXELS];

static struct led_rgb get_color(colors_t color) {
    struct led_rgb rgb_color; 
    switch (color)
    {
    case RED:
        RGB_SET(rgb_color, CONFIG_LED_BRIGHTNESS, 0x00, 0x00);
        break;
    case GREEN:
        RGB_SET(rgb_color, 0x00, CONFIG_LED_BRIGHTNESS, 0x00);
        break;
    case BLUE:
        RGB_SET(rgb_color, 0x00, 0x00, CONFIG_LED_BRIGHTNESS);
        break;
    default:
        RGB_SET(rgb_color, 0x00, 0x00, 0x00);
        break;
    }
    return rgb_color;
}

int rgb_led_turn_on(const struct device* rgb_led_dev, colors_t color) {
    int rc;

    struct led_rgb rgb_color = get_color(color);
    memset(&pixels, 0x00, sizeof(pixels));
	memcpy(&pixels[0], &rgb_color, sizeof(struct led_rgb));
    rc = led_strip_update_rgb(rgb_led_dev, pixels, STRIP_NUM_PIXELS);
    if (rc) {
        printk("couldn't update strip: %d\n", rc);
        return rc;
    }
    return 0;
}