# 🌟 Zephyr Contrib: Rohit's Embedded Toolbox

A collection of highly optimized, professional-grade libraries and drivers for the **Zephyr RTOS**, with a focus on ultra-constrained hardware like the **CH32V003**.

## 📦 What's inside?

### 🔹 [xLED](./lib/xled)
The **xLED** library is a versatile "Extended" LED driver designed for flexibility and efficiency. It handles anything from a single status LED to full RGB arrays, supporting both **Hardware PWM** and **GPIO switching** in a mixed configuration.

**Key Features:**
- **Hybrid Mixed Configuration**: Mix and match Hardware PWM pins and GPIO pins seamlessly for a single RGB handle.
- **Versatile Modes**: 
  - **PWM Generation**: Use it to generate PWM signals for brightness control.
  - **Single LED**: Configure only one pin for a simple status indicator.
  - **Partial RGB**: Use any 2 LEDs (e.g., Red/Green only) by leaving unused pins as `XLED_PIN_NONE`.
- **Background Blinking**: Built-in non-blocking background toggling using Zephyr's `k_timer`.
- **Ultra-Lightweight**: Minimal RAM footprint, ideal for memory-constrained chips like the CH32V003.
- **Simple API**: Set color, duty cycle (0-10000), and blink period in one function call.

---

## 🚀 Repository Structure

```bash
.
├── lib/
│   └── xled/            # 📂 xLED Implementation (xled.c, xled.h)
├── samples/
│   └── ch32v003f4p6_xled/ # 🎯 Demo for CH32V003F4P6 chip
├── README.md
└── LICENSE
```

## 🛠️ Usage

To use **xLED** in your Zephyr project:
1. Copy `lib/xled/xled.h` and `lib/xled/xled.c` into your project.
2. In your `CMakeLists.txt`, add:
   ```cmake
   target_sources(app PRIVATE lib/xled/xled.c)
   zephyr_include_directories(lib/xled)
   ```
3. Initialize the configuration in your code:
   ```c
   #include "xled.h"
   
   static struct xled status_led = {
       .red = { .mode = XLED_PIN_PWM, .pwm_dev = DEVICE_DT_GET(PWM_NODE), ... },
       .green = { .mode = XLED_PIN_GPIO, .gpio_port = DEVICE_DT_GET(GPIO_NODE), ... },
       .blue = { .mode = XLED_PIN_NONE }, // Skip BLUE
   };
   
   xled_init(&status_led);
   xled_set(&status_led, XLED_RED, 5000, 250); // 50% Red, blinking every 250ms
   ```

---

## ⚡ Contributing

This is a personal collection. If you have an optimization suggestion or a bug fix, feel free to open a PR!

*Created by Rohit*
