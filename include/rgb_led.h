
#include <zephyr/sys/util.h>

typedef enum {
    NONE,
    RED,
    GREEN,
    BLUE
} colors_t;

int rgb_led_turn_on(const struct device* rgb_led_dev, colors_t color);