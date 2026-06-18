
#include <zephyr/kernel.h>

int get_internal_temp(const struct device* internal_temp_dev, double* temp_pt);
int get_temp(const struct device* temp_dev, double* temp_pt);
