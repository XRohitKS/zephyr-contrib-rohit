# zephyr-contrib-rohit

This repo is structured like a small Zephyr contrib workspace:

- reusable modules live in `modules/`
- public module samples live in each module's `samples/` folder
- root `apps/` is reserved for local experiments and is ignored by Git

This root structure keeps reusable module code separate from local experiments and scales well if you add more modules later.

## Layout

```text
zephyr-contrib-rohit/
|- modules/
|  \- xled/
|     \- samples/
|        |- xled_sample/
|        |- xled_dual_sample/
|        |- xled_pwm_gpio_mix_sample/
|        \- xled_dual_less_pin_sample/
\- README.md
```

## Current module

`modules/xled` is a lightweight Zephyr RGB LED module using the compatible:

`zephyrcontrib,xled`

It keeps the existing API:

- `xled_set(dev, color, duty, blink_ms)`
- `xled_set_brightness(dev, duty)`
- `xled_on(dev)`
- `xled_off(dev)`

## Build flow

Before building, make sure `west` works in your shell and your Zephyr toolchain paths are set up.

```powershell
# Example
west --version
```

Then build any sample from this repo root:

```powershell
west build -b ch32v003f4p6 .\modules\xled\samples\xled_sample
west build -b ch32v003f4p6 .\modules\xled\samples\xled_dual_sample
west build -b ch32v003f4p6 .\modules\xled\samples\xled_pwm_gpio_mix_sample
west build -b ch32v003f4p6 .\modules\xled\samples\xled_dual_less_pin_sample
```

If needed, you can also pass the module explicitly:

```powershell
west build -b ch32v003f4p6 .\modules\xled\samples\xled_sample -- "-DZEPHYR_EXTRA_MODULES=$PWD\modules\xled"
```

## Sample overview

- `xled_sample`: one RGB XLED instance
- `xled_dual_sample`: two full XLED instances
- `xled_pwm_gpio_mix_sample`: one instance mixing PWM and GPIO channels
- `xled_dual_less_pin_sample`: partial-channel setup with one red-only LED and one red+green LED

## Future growth

If you add more reusable drivers later, keep following the same pattern:

- `modules/<name>/` for the reusable Zephyr module
- `modules/<name>/samples/<sample-name>/` for public examples
- `apps/<sample-name>/` only for local experiments that should not be committed

That will keep this repo easy to maintain and easy to share.
