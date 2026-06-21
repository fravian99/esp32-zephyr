
#include <zephyr/kernel.h>

int get_internal_temp(const struct device* internal_temp_dev, double* temp_pt, bool with_fetch);
int get_temp(const struct device* temp_dev, double* temp_pt, bool with_fetch);
int get_hum(const struct device* hum_dev, double* hum_pt, bool with_fetch);