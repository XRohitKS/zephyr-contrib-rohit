/*
 * Copyright (c) 2026 Rohit Kumar Sharma
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Rohit Kumar Sharma
 *
 * xled.c - Extended LED library implementation
 *
 * Background blinking logic:
 *  - Uses k_timer for independent background toggling.
 *  - Toggling happens in ISR context to be non-blocking.
 *  - Duty is uint16_t (10000 = 100.00%).
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

#include "xled.h"

/* --- Internal: apply color bit and duty to a single pin --- */
static inline int pin_apply(const struct xled_pin *pin, uint8_t color_bit, uint16_t duty)
{
	if (pin->mode == XLED_PIN_NONE) {
		return 0;
	}

	if (pin->mode == XLED_PIN_PWM) {
		uint32_t pulse = 0U;
		if (color_bit && duty > 0U) {
			pulse = (pin->hw.pwm.period * (uint32_t)duty) / XLED_DUTY_MAX;
		}
		return pwm_set_cycles(pin->hw.pwm.dev, pin->hw.pwm.channel, pin->hw.pwm.period,
				      pulse, 0);
	}

	/* XLED_PIN_GPIO */
	return gpio_pin_set(pin->hw.gpio.port, pin->hw.gpio.pin, (color_bit && duty > 0U) ? 1 : 0);
}

/* --- Internal: apply the active color/duty state to all pins --- */
static void apply_state(const struct xled *led, uint16_t duty)
{
	/* Bit 2 = RED, Bit 1 = GREEN, Bit 0 = BLUE */
	pin_apply(&led->red, (led->active_color >> 2) & 1U, duty);
	pin_apply(&led->green, (led->active_color >> 1) & 1U, duty);
	pin_apply(&led->blue, led->active_color & 1U, duty);
}

/* --- Background Toggle ISR Callback --- */
static void toggle_callback(struct k_timer *timer_id)
{
	/* k_timer_get_id returns the pointer to our struct xled if we set it up right */
	struct xled *led = CONTAINER_OF(timer_id, struct xled, timer);

	led->phase = !led->phase;
	apply_state(led, led->phase ? led->active_duty : 0U);
}

/* --- Internal: configure a single pin --- */
static int pin_init_dev(const struct xled_pin *pin)
{
	if (pin->mode == XLED_PIN_NONE) {
		return 0;
	}

	if (pin->mode == XLED_PIN_PWM) {
		if (!device_is_ready(pin->hw.pwm.dev)) {
			return -ENODEV;
		}
		return pwm_set_cycles(pin->hw.pwm.dev, pin->hw.pwm.channel, pin->hw.pwm.period, 0U,
				      0);
	}

	if (!device_is_ready(pin->hw.gpio.port)) {
		return -ENODEV;
	}
	return gpio_pin_configure(pin->hw.gpio.port, pin->hw.gpio.pin, GPIO_OUTPUT_INACTIVE);
}

/* --- Public API --- */

int xled_init(struct xled *led)
{
	int err;

	if (led == NULL) {
		return -EINVAL;
	}

	/* Initialize background timer */
	k_timer_init(&led->timer, toggle_callback, NULL);

	/* Initialize Hardware */
	err = pin_init_dev(&led->red);
	if (err) {
		return err;
	}

	err = pin_init_dev(&led->green);
	if (err) {
		return err;
	}

	err = pin_init_dev(&led->blue);
	if (err) {
		return err;
	}

	return 0;
}

int xled_set(struct xled *led, uint8_t color, uint16_t duty, uint32_t blink_ms)
{
	if (led == NULL || duty > XLED_DUTY_MAX) {
		return -EINVAL;
	}

	/* Update internal state */
	led->active_color = color;
	led->active_duty = duty;
	led->period_ms = blink_ms;
	led->phase = true; /* Start in the ON state */

	if (blink_ms == 0) {
		/* Stop existing background timer and apply immediately */
		k_timer_stop(&led->timer);
		apply_state(led, duty);
	} else {
		/* Apply initial state then start background timer */
		apply_state(led, duty);
		k_timer_start(&led->timer, K_MSEC((int32_t)blink_ms), K_MSEC((int32_t)blink_ms));
	}

	return 0;
}
