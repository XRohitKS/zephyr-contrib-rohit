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

/* --- Demo Mode Selection (Choose ONE by uncommenting) --- */
#define DEMO_ALL_PWM      /* PC5, PC6, PC7 all using PWM (Full dimming) */
// #define DEMO_MIXED_MODES  /* PC5=PWM, PC6=GPIO, PC7=PWM (Mixed Hardware) */
// #define DEMO_ALL_GPIO     /* PC5, PC6, PC7 all using GPIO (Fast/No Math) */
// #define DEMO_DUAL_LED     /* Only Red/Green enabled (Blue is disabled) */
// #define DEMO_SINGLE_LED   /* Only Red enabled (Others are ignored) */

static struct xled power_led = {
#if defined(DEMO_ALL_PWM)
    .red   = { .mode = XLED_PIN_PWM, .hw.pwm = { .dev = DEVICE_DT_GET(DT_NODELABEL(pwm2)), .channel = 0, .period = 48000 } },
    .green = { .mode = XLED_PIN_PWM, .hw.pwm = { .dev = DEVICE_DT_GET(DT_NODELABEL(pwm1)), .channel = 0, .period = 48000 } },
    .blue  = { .mode = XLED_PIN_PWM, .hw.pwm = { .dev = DEVICE_DT_GET(DT_NODELABEL(pwm1)), .channel = 1, .period = 48000 } },

#elif defined(DEMO_MIXED_MODES)
    .red   = { .mode = XLED_PIN_PWM,  .hw.pwm  = { .dev = DEVICE_DT_GET(DT_NODELABEL(pwm2)), .channel = 0, .period = 48000 } },
    .green = { .mode = XLED_PIN_GPIO, .hw.gpio = { .port = DEVICE_DT_GET(DT_NODELABEL(gpioc)), .pin = 6 } },
    .blue  = { .mode = XLED_PIN_PWM,  .hw.pwm  = { .dev = DEVICE_DT_GET(DT_NODELABEL(pwm1)), .channel = 1, .period = 48000 } },

#elif defined(DEMO_ALL_GPIO)
    .red   = { .mode = XLED_PIN_GPIO, .hw.gpio = { .port = DEVICE_DT_GET(DT_NODELABEL(gpioc)), .pin = 5 } },
    .green = { .mode = XLED_PIN_GPIO, .hw.gpio = { .port = DEVICE_DT_GET(DT_NODELABEL(gpioc)), .pin = 6 } },
    .blue  = { .mode = XLED_PIN_GPIO, .hw.gpio = { .port = DEVICE_DT_GET(DT_NODELABEL(gpioc)), .pin = 7 } },

#elif defined(DEMO_DUAL_LED)
    .red   = { .mode = XLED_PIN_PWM,  .hw.pwm  = { .dev = DEVICE_DT_GET(DT_NODELABEL(pwm2)), .channel = 0, .period = 48000 } },
    .green = { .mode = XLED_PIN_PWM,  .hw.pwm  = { .dev = DEVICE_DT_GET(DT_NODELABEL(pwm1)), .channel = 0, .period = 48000 } },
    .blue  = { .mode = XLED_PIN_NONE },

#elif defined(DEMO_SINGLE_LED)
    .red   = { .mode = XLED_PIN_PWM, .hw.pwm = { .dev = DEVICE_DT_GET(DT_NODELABEL(pwm2)), .channel = 0, .period = 48000 } },
    .green = { .mode = XLED_PIN_NONE },
    .blue  = { .mode = XLED_PIN_NONE },
#endif
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
        xled_set_brightness(&power_led, 500);
        k_sleep(K_SECONDS(2));
        xled_set_brightness(&power_led, 4000);
        k_sleep(K_SECONDS(2));
        xled_set_brightness(&power_led, 8000);
        k_sleep(K_SECONDS(2));

        /* 4. TURN OFF for 1 second */
        printk("Off...\n\n");
        xled_set(&power_led, XLED_OFF, 0, 0);
        k_sleep(K_SECONDS(1));
        // k_msleep(1000); // k_msleep took more rom ( 4 Bytes more only every k_msleep call) as compared to k_sleep
    }

    return 0;
}
