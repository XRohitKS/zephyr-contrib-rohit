# xled Zephyr Module

`xled` is a lightweight RGB LED driver module for Zephyr.

Compatible:

`zephyrcontrib,xled`

Public API:

- `xled_set(dev, color, duty, blink_ms)`
- `xled_set_brightness(dev, duty)`
- `xled_on(dev)`
- `xled_off(dev)`

## Goals

- low flash and RAM usage
- simple Zephyr module packaging
- PWM or GPIO support per color channel
- easy reuse from other Zephyr applications

## Module layout

```text
modules/xled/
|- CMakeLists.txt
|- Kconfig
|- zephyr/module.yaml
|- dts/bindings/led/zephyrcontrib,xled.yaml
|- drivers/led/xled/
|- include/zephyr/drivers/xled.h
|- samples/
|  |- xled_sample/
|  |- xled_dual_sample/
|  |- xled_pwm_gpio_mix_sample/
|  \- xled_dual_less_pin_sample/
\- README.md
```

## Devicetree model

One XLED device can use up to 3 color channels:

- `pin1-*` for red
- `pin2-*` for green
- `pin3-*` for blue

Each channel may be:

- PWM
- GPIO
- omitted

Do not define both `*-pwms` and `*-gpios` for the same channel.

## Example node

```dts
xled0: xled_0 {
	compatible = "zephyrcontrib,xled";
	status = "okay";
	pin1-pwms = <&pwm2 3 PWM_MSEC(1) PWM_POLARITY_NORMAL>;
	pin2-gpios = <&gpioc 0 GPIO_ACTIVE_HIGH>;
	pin3-gpios = <&gpioc 1 GPIO_ACTIVE_HIGH>;
};
```

## Build with this repo

Before building, make sure your Python virtual environment is active and your Zephyr toolchain paths are already set up.

```powershell
# Example
.venv\Scripts\Activate.ps1
```

Then build an app from this repo:

```powershell
west build -b ch32v003f4p6 .\modules\xled\samples\xled_sample
```

If you want to pass the module path explicitly:

```powershell
west build -b ch32v003f4p6 .\modules\xled\samples\xled_sample -- "-DZEPHYR_EXTRA_MODULES=$PWD\modules\xled"
```

## Notes

- this module is optimized for small MCUs and low flash usage
- the current structure is suitable for a contrib-style public repo
- future reusable modules should follow the same `modules/<name>/` pattern
