/*
 * Copyright (c) 2026 Rohit
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_LED_XLED_H_
#define ZEPHYR_DRIVERS_LED_XLED_H_

#include <zephyr/device.h>
#include <zephyr/toolchain.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XLED_RGB_OFF      0x00
#define XLED_RGB_RED      0x04
#define XLED_RGB_GREEN    0x02
#define XLED_RGB_BLUE     0x01
#define XLED_RGB_YELLOW   (XLED_RGB_RED | XLED_RGB_GREEN)
#define XLED_RGB_CYAN     (XLED_RGB_GREEN | XLED_RGB_BLUE)
#define XLED_RGB_MAGENTA  (XLED_RGB_RED | XLED_RGB_BLUE)
#define XLED_RGB_WHITE    (XLED_RGB_RED | XLED_RGB_GREEN | XLED_RGB_BLUE)

#define XLED_DUTY_MAX 10000

struct xled_api {
	int (*set)(const struct device *dev, uint8_t color, uint16_t duty, uint32_t blink_ms);
	int (*set_brightness)(const struct device *dev, uint16_t duty);
	int (*on)(const struct device *dev);
	int (*off)(const struct device *dev);
};

static inline int xled_set(const struct device *dev, uint8_t color, uint16_t duty,
			   uint32_t blink_ms)
{
	const struct xled_api *api = (const struct xled_api *)dev->api;

	return api->set(dev, color, duty, blink_ms);
}

static inline int xled_set_brightness(const struct device *dev, uint16_t duty)
{
	const struct xled_api *api = (const struct xled_api *)dev->api;

	return api->set_brightness(dev, duty);
}

static inline int xled_on(const struct device *dev)
{
	const struct xled_api *api = (const struct xled_api *)dev->api;

	return api->on(dev);
}

static inline int xled_off(const struct device *dev)
{
	const struct xled_api *api = (const struct xled_api *)dev->api;

	return api->off(dev);
}

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_DRIVERS_LED_XLED_H_ */
