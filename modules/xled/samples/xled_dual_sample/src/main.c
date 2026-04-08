/*
 * Copyright (c) 2026 Rohit
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/xled.h>
#include <zephyr/kernel.h>

int main(void)
{
	const struct device *led1 = DEVICE_DT_GET(DT_NODELABEL(led_xled_1));
	const struct device *led2 = DEVICE_DT_GET(DT_NODELABEL(led_xled_2));

	if (!device_is_ready(led1) || !device_is_ready(led2)) {
		return 0;
	}

	while (1) {
		xled_set(led1, XLED_RGB_RED, 1000, 0);
		xled_set(led2, XLED_RGB_GREEN, 1000, 0);
		k_msleep(1000);

		xled_set(led1, XLED_RGB_WHITE, 1000, 250);
		xled_set(led2, XLED_RGB_BLUE, 1000, 0);
		k_msleep(2000);
	}
}
