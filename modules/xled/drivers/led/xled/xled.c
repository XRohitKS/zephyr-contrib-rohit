#define DT_DRV_COMPAT zephyrcontrib_xled

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

#include <zephyr/drivers/xled.h>

struct xled_data {
	struct k_timer timer;
	const struct device *dev;
	uint8_t active_color;
	uint16_t active_duty;
	uint32_t active_period_ms;
	bool phase;
};

struct xled_config {
	void (*apply)(uint8_t color, uint16_t duty);
};

static void toggle_callback(struct k_timer *timer_id)
{
	struct xled_data *data = CONTAINER_OF(timer_id, struct xled_data, timer);
	const struct xled_config *cfg = data->dev->config;

	data->phase = !data->phase;
	cfg->apply(data->active_color, data->phase ? data->active_duty : 0U);
}

static int xled_api_set(const struct device *dev, uint8_t color, uint16_t duty, uint32_t blink_ms)
{
	struct xled_data *data = (struct xled_data *)dev->data;
	const struct xled_config *cfg = dev->config;

	if (duty > XLED_DUTY_MAX) {
		return -EINVAL;
	}

	data->active_color = color;
	data->active_duty = duty;
	data->active_period_ms = blink_ms;
	data->phase = true;

	if (blink_ms == 0U || duty == 0U) {
		k_timer_stop(&data->timer);
		cfg->apply(color, duty);
	} else {
		cfg->apply(color, duty);
		k_timer_start(&data->timer, K_MSEC((int32_t)blink_ms), K_MSEC((int32_t)blink_ms));
	}

	return 0;
}

static int xled_api_set_brightness(const struct device *dev, uint16_t duty)
{
	struct xled_data *data = (struct xled_data *)dev->data;
	uint8_t color = data->active_color ? data->active_color : XLED_RGB_WHITE;

	return xled_api_set(dev, color, duty, data->active_period_ms);
}

static int xled_api_on(const struct device *dev)
{
	struct xled_data *data = (struct xled_data *)dev->data;
	uint16_t duty = data->active_duty ? data->active_duty : XLED_DUTY_MAX;

	return xled_api_set_brightness(dev, duty);
}

static int xled_api_off(const struct device *dev)
{
	struct xled_data *data = (struct xled_data *)dev->data;
	const struct xled_config *cfg = dev->config;

	k_timer_stop(&data->timer);
	cfg->apply(data->active_color, 0U);

	return 0;
}

static const struct xled_api xled_api_funcs = {
	.set = xled_api_set,
	.set_brightness = xled_api_set_brightness,
	.on = xled_api_on,
	.off = xled_api_off
};

#define XLED_PIN_HAS_GPIO(inst, idx) DT_INST_NODE_HAS_PROP(inst, pin##idx##_gpios)
#define XLED_PIN_HAS_PWM(inst, idx) DT_INST_NODE_HAS_PROP(inst, pin##idx##_pwms)

#define XLED_PIN_ASSERT(inst, idx)                                                              \
	BUILD_ASSERT(!(XLED_PIN_HAS_GPIO(inst, idx) && XLED_PIN_HAS_PWM(inst, idx)),           \
		     "xled channel " #idx " cannot define both GPIO and PWM backends")

#define XLED_PWM_VALUE(inst, idx, color, duty)                                                  \
	(((color >> (3 - idx)) & 1U) ?                                                            \
		(DT_INST_PHA_BY_IDX(inst, pin##idx##_pwms, 0, period) * duty / XLED_DUTY_MAX) : \
		0U)

#define DEFINE_APPLY_PIN(inst, idx)                                                             \
	static int xled_apply_pin_##inst##_##idx(uint8_t color, uint16_t duty)               \
	{                                                                                     \
		COND_CODE_1(XLED_PIN_HAS_PWM(inst, idx),                                     \
			    (return pwm_set_cycles(DEVICE_DT_GET(                           \
					     DT_INST_PHANDLE_BY_IDX(inst, pin##idx##_pwms, 0)), \
				     DT_INST_PHA_BY_IDX(inst, pin##idx##_pwms, 0, channel),  \
				     DT_INST_PHA_BY_IDX(inst, pin##idx##_pwms, 0, period),   \
				     XLED_PWM_VALUE(inst, idx, color, duty),                 \
				     DT_INST_PHA_BY_IDX(inst, pin##idx##_pwms, 0, flags));), \
			    (COND_CODE_1(XLED_PIN_HAS_GPIO(inst, idx),                    \
					 ({                                              \
						 const struct gpio_dt_spec s =        \
							 GPIO_DT_SPEC_INST_GET(inst,     \
										 pin##idx##_gpios); \
						 return gpio_pin_set_dt(              \
							 &s,                              \
							 ((((color >> (3 - idx)) & 1U) && \
							   duty > 0U) ? 1 : 0));         \
					 }),                                             \
					 (return 0;))))                               \
	}

#define DEFINE_INIT_PIN(inst, idx)                                                              \
	static int xled_init_pin_##inst##_##idx(void)                                          \
	{                                                                                      \
		COND_CODE_1(XLED_PIN_HAS_PWM(inst, idx),                                      \
			    ({                                                              \
				    const struct device *d = DEVICE_DT_GET(                 \
					    DT_INST_PHANDLE_BY_IDX(inst, pin##idx##_pwms, 0)); \
				    if (!device_is_ready(d))                               \
					    return -ENODEV;                                \
				    return pwm_set_cycles(                                \
					    d, DT_INST_PHA_BY_IDX(inst, pin##idx##_pwms, 0,  \
								 channel),                 \
					    DT_INST_PHA_BY_IDX(inst, pin##idx##_pwms, 0,      \
								 period), 0U,              \
					    DT_INST_PHA_BY_IDX(inst, pin##idx##_pwms, 0,      \
								 flags));                  \
			    }),                                                             \
			    (COND_CODE_1(XLED_PIN_HAS_GPIO(inst, idx),                     \
					 ({                                                     \
						 const struct gpio_dt_spec s =             \
							 GPIO_DT_SPEC_INST_GET(inst,          \
										 pin##idx##_gpios);    \
						 if (!gpio_is_ready_dt(&s))               \
							 return -ENODEV;                      \
						 return gpio_pin_configure_dt(            \
							 &s, GPIO_OUTPUT_INACTIVE);          \
					 }),                                                    \
					 (return 0;))))                                \
	}

#define XLED_INST_DEFINE(inst)                                                                  \
	XLED_PIN_ASSERT(inst, 1);                                                               \
	XLED_PIN_ASSERT(inst, 2);                                                               \
	XLED_PIN_ASSERT(inst, 3);                                                               \
	DEFINE_APPLY_PIN(inst, 1);                                                              \
	DEFINE_APPLY_PIN(inst, 2);                                                              \
	DEFINE_APPLY_PIN(inst, 3);                                                              \
	DEFINE_INIT_PIN(inst, 1);                                                               \
	DEFINE_INIT_PIN(inst, 2);                                                               \
	DEFINE_INIT_PIN(inst, 3);                                                               \
	static void xled_apply_##inst(uint8_t color, uint16_t duty)                            \
	{                                                                                      \
		(void)xled_apply_pin_##inst##_##1(color, duty);                               \
		(void)xled_apply_pin_##inst##_##2(color, duty);                               \
		(void)xled_apply_pin_##inst##_##3(color, duty);                               \
	}                                                                                      \
	static int xled_init_##inst(const struct device *dev)                                   \
	{                                                                                      \
		struct xled_data *data = (struct xled_data *)dev->data;                        \
		int ret;                                                                       \
		data->dev = dev;                                                               \
		ret = xled_init_pin_##inst##_##1();                                            \
		if (ret < 0)                                                                   \
			return ret;                                                            \
		ret = xled_init_pin_##inst##_##2();                                            \
		if (ret < 0)                                                                   \
			return ret;                                                            \
		ret = xled_init_pin_##inst##_##3();                                            \
		if (ret < 0)                                                                   \
			return ret;                                                            \
		k_timer_init(&data->timer, toggle_callback, NULL);                             \
		return 0;                                                                      \
	}                                                                                      \
	static struct xled_data xled_data_##inst;                                              \
	static const struct xled_config xled_config_##inst = {.apply = xled_apply_##inst};     \
	DEVICE_DT_INST_DEFINE(inst, xled_init_##inst, NULL, &xled_data_##inst,                 \
			      &xled_config_##inst, POST_KERNEL, CONFIG_XLED_INIT_PRIORITY, \
			      &xled_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(XLED_INST_DEFINE)
/*
 * Copyright (c) 2026 Rohit
 *
 * SPDX-License-Identifier: Apache-2.0
 */
