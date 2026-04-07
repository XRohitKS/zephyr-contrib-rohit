#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/xled.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	const struct device *led1 = DEVICE_DT_GET(DT_NODELABEL(led_xled_red_only));
	const struct device *led2 = DEVICE_DT_GET(DT_NODELABEL(led_xled_rg_only));

	printk("xled dual less pin sample\n");

	if (!device_is_ready(led1) || !device_is_ready(led2)) {
		printk("xled less-pin devices not ready\n");
		return 0;
	}

	while (1) {
		/* LED1 has only red, LED2 has red+green */
		xled_set(led1, XLED_RGB_RED, XLED_DUTY_MAX, 0);
		xled_set(led2, XLED_RGB_YELLOW, XLED_DUTY_MAX, 0);
		k_msleep(1200);

		/* Blink the single-pin LED while LED2 shows green only */
		xled_set(led1, XLED_RGB_RED, 3000, 250);
		xled_set(led2, XLED_RGB_GREEN, XLED_DUTY_MAX, 0);
		k_msleep(2000);

		/* Turn off LED1 and leave only LED2 red active */
		xled_off(led1);
		xled_set(led2, XLED_RGB_RED, XLED_DUTY_MAX, 0);
		k_msleep(1200);

		/* Everything off */
		xled_off(led2);
		k_msleep(1200);
	}
}
/*
 * Copyright (c) 2026 Rohit
 *
 * SPDX-License-Identifier: Apache-2.0
 */
