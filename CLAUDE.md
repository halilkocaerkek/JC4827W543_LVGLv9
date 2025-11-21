# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an Arduino sketch for the JC4827W543 development board (ESP32-S3 based) that demonstrates LVGL v9 GUI framework integration with a capacitive touch display. The project serves as a starting template for developing touchscreen GUI applications on this specific hardware.

**Tutorial**: https://youtu.be/mnOzfRFQJIM

## Hardware Platform

- **Board**: JC4827W543 (ESP32-S3 based)
- **Display**: 480x272 resolution with RGB565 color format
- **Touch Controller**: GT911 capacitive touch (I2C: SDA=8, SCL=4, INT=3, RST=38)
- **Arduino Board Selection**: "ESP32S3 Dev Module" from esp32 Arduino Core by Espressif

## Required Dependencies

Install these libraries via Arduino Library Manager:

1. **lvgl** (last tested: v9.2.2) - Must configure `lv_conf.h` per https://docs.lvgl.io/master/get-started/platforms/arduino.html
2. **GFX Library for Arduino** (last tested: v1.5.6)
3. **Dev Device Pins** (last tested: v0.0.2) - Provides `PINS_JC4827W543.h`
4. **TAMC_GT911** (last tested: v1.0.2) - Touch controller driver
5. **esp32 Arduino Core by Espressif** (last tested: v3.2.0)

## Building and Uploading

This project uses the Arduino IDE or Arduino CLI workflow:

```bash
# Compile (Arduino CLI)
arduino-cli compile --fqbn esp32:esp32:esp32s3 JC4827W543_LVGLv9.ino

# Upload (Arduino CLI)
arduino-cli upload -p <PORT> --fqbn esp32:esp32:esp32s3 JC4827W543_LVGLv9.ino

# Monitor serial output
arduino-cli monitor -p <PORT> -c baudrate=115200
```

Or use Arduino IDE: File → Open → Select `.ino` file → Verify/Upload buttons

## Architecture

### LVGL Integration Pattern

The codebase follows the standard LVGL integration pattern for Arduino:

1. **Display Driver** (`my_disp_flush`): LVGL rendering callback that transfers pixel buffers to the hardware display via `gfx->draw16bitRGBBitmap()`
2. **Input Driver** (`my_touchpad_read`): LVGL touchpad callback that polls the GT911 touch controller and updates touch state
3. **Tick Source** (`millis_cb`): Provides LVGL with time elapsed for animations and timers
4. **Memory Management**: Uses `heap_caps_malloc()` for display buffer allocation with fallback strategy (MALLOC_CAP_INTERNAL first, then generic MALLOC_CAP_8BIT)

### Main Loop Architecture

The `loop()` function must continuously call `lv_task_handler()` to process LVGL events, rendering, and animations. The 5ms delay balances responsiveness with CPU usage.

### Widget Creation

All UI widgets are created in `setup()` after LVGL initialization. Event callbacks (e.g., `btn_event_cb`, `value_changed_event_cb`) handle user interactions.

## Key Technical Details

- **Display Buffer Size**: `screenWidth * 40` pixels (partial rendering mode) to balance memory usage and performance
- **Touch Rotation**: Set to `ROTATION_INVERTED` - adjust based on physical display orientation
- **Backlight Control**: GPIO pin `GFX_BL` controls display backlight (HIGH = on)
- **Color Format**: RGB565 (16-bit color depth)

## Modifying the Code

When adding new UI features:

1. Create widgets in `setup()` after the display/touch initialization
2. Register event callbacks for interactive widgets
3. Use `lv_screen_active()` as the parent for top-level widgets
4. Ensure `lv_task_handler()` continues to be called in `loop()`

When adjusting hardware configuration:

- Touch pin definitions are in lines 11-16
- Display resolution constants are defined by the GT911 initialization (lines 15-16)
- The `gfx` global object is provided by `PINS_JC4827W543.h`
