#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/xled.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
	const struct device *led = DEVICE_DT_GET(DT_NODELABEL(led_xled_mix));

	printk("xled pwm+gpio mix sample\n");

	if (!device_is_ready(led)) {
		printk("xled mix not ready\n");
		return 0;
	}

	while (1) {
		/* PWM red channel only */
		xled_set(led, XLED_RGB_RED, 1000, 0);
		k_msleep(1200);

		/* GPIO green channel only */
		xled_set(led, XLED_RGB_GREEN, XLED_DUTY_MAX, 0);
		k_msleep(1200);

		/* GPIO blue channel only */
		xled_set(led, XLED_RGB_BLUE, XLED_DUTY_MAX, 0);
		k_msleep(1200);

		/* Mixed color across PWM + GPIO channels */
		xled_set(led, XLED_RGB_YELLOW, 4000, 0);
		k_msleep(1200);

		/* Blink all available channels together */
		xled_set(led, XLED_RGB_WHITE, 4000, 250);
		k_msleep(2000);

		/* All outputs off */
		xled_off(led);
		k_msleep(1200);
	}
}
/*
 * Copyright (c) 2026 Rohit
 *
 * SPDX-License-Identifier: Apache-2.0
 */
