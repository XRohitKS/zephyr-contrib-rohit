/*
 * Copyright (c) 2026 Rohit Kumar Sharma
 * SPDX-License-Identifier: Apache-2.0
 *
 * Author: Rohit Kumar Sharma
 *
 * xled.h - Minimal Extended LED library for CH32V003
 */

#ifndef XLED_H
#define XLED_H

#include <stdint.h>
#include <stdbool.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

/* --- 3-bit color bitmask constants --- */
#define XLED_OFF     0  /* 000 */
#define XLED_BLUE    1  /* 001 */
#define XLED_GREEN   2  /* 010 */
#define XLED_CYAN    3  /* 011 */
#define XLED_RED     4  /* 100 */
#define XLED_MAGENTA 5  /* 101 */
#define XLED_YELLOW  6  /* 110 */
#define XLED_WHITE   7  /* 111 */

/* Maximum duty value (represents 100.00%) */
#define XLED_DUTY_MAX 10000U

/* --- Pin mode --- */
typedef enum {
    XLED_PIN_NONE = 0, /* Pin not connected / not used. All ops are skipped. */
    XLED_PIN_GPIO = 1, /* GPIO on/off only. duty is ignored.                  */
    XLED_PIN_PWM  = 2, /* Hardware PWM. duty sets brightness 0-10000.         */
} xled_pin_mode_t;

/*
 * Per-pin configuration.
 */
struct xled_pin {
    uint8_t mode; /* xled_pin_mode_t */

    union {
        struct {
            const struct device *port;
            gpio_pin_t           pin;
        } gpio;
        struct {
            const struct device *dev;
            uint32_t             channel;
            uint32_t             period;
        } pwm;
    } hw;
};

/* --- XLED Handle --- */
struct xled {
    /* Pin Hardware Config */
    struct xled_pin red;
    struct xled_pin green;
    struct xled_pin blue;

    /* Background State Tracking */
    uint8_t  active_color;      /* Target color when ON */
    uint16_t active_duty;       /* Target duty when ON  */
    uint32_t period_ms;         /* Blink period (0=off) */
    bool     phase;             /* Toggle state (ON/OFF) */
    struct k_timer timer;       /* Background kernel timer */
};

/*
 * xled_init() - Startup configuration.
 * Must be called before any other functions.
 * Returns 0 on success.
 */
int xled_init(struct xled *led);

/*
 * xled_set() - Primary control function.
 * 
 * @color    : XLED_OFF, XLED_RED, etc.
 * @duty     : 0 - 10000 (100.00%)
 * @blink_ms : 0 = Solid color.
 *             >0 = Blink at this frequency (e.g. 500 = toggle every 500ms).
 *
 * This function returns immediately. Blinking happens in the background.
 */
int xled_set(struct xled *led, uint8_t color, uint16_t duty, uint32_t blink_ms);

#endif /* XLED_H */
