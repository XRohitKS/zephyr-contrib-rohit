/*
 * Copyright (c) 2026 Rohit Kumar Sharma
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Rohit Kumar Sharma
 *
 * main.c - XLED Background Blink Demo
 * 
 * Demonstrates independent, non-blocking blinking using k_timer.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "xled.h"

/* --- Configured PC5=Red, PC6=Green, PC7=Blue --- */
static struct xled power_led = {
    .red = {
        .mode        = XLED_PIN_PWM,
        .pwm_dev     = DEVICE_DT_GET(DT_NODELABEL(pwm2)), /* TIM2 CH0 → PC5 */
        .pwm_channel = 0,
        .pwm_period  = 48000,
    },
    .green = {
        .mode        = XLED_PIN_PWM,
        .pwm_dev     = DEVICE_DT_GET(DT_NODELABEL(pwm1)), /* TIM1 CH0 → PC6 */
        .pwm_channel = 0,
        .pwm_period  = 48000,
    },
    .blue = {
        .mode        = XLED_PIN_PWM,
        .pwm_dev     = DEVICE_DT_GET(DT_NODELABEL(pwm1)), /* TIM1 CH1 → PC7 */
        .pwm_channel = 1,
        .pwm_period  = 48000,
    },
};

int main(void)
{
    int err = xled_init(&power_led);

    if (err) {
        printk("XLED init failed: %d\n", err);
        return 0;
    }

    printk("XLED Background Blink Demo\n");

    while (1) {
        /* 1. SOLID RED for 2 seconds */
        printk("Solid Red...\n");
        xled_set(&power_led, XLED_RED, 1000, 0);
        k_sleep(K_SECONDS(2));

        /* 2. BLINK CYAN (Blue+Green) for 5 seconds */
        /* Blinks every 250ms (very fast) */
        printk("Blinking Cyan (Fast)...\n");
        xled_set(&power_led, XLED_CYAN, 500, 250);
        k_sleep(K_SECONDS(5));

        /* 3. BLINK MAGENTA (Red+Blue) slowly for 5 seconds */
        /* Blinks every 1000ms */
        printk("Blinking Magenta (Slow)...\n");
        xled_set(&power_led, XLED_MAGENTA, 250, 1000);
        k_sleep(K_SECONDS(5));

        /* 4. TURN OFF for 1 second */
        printk("Off...\n\n");
        xled_set(&power_led, XLED_OFF, 0, 0);
        k_sleep(K_SECONDS(1));
        // k_msleep(1000); // k_msleep took more rom ( 4 Bytes more only every k_msleep call) as compared to k_sleep
    }

    return 0;
}
