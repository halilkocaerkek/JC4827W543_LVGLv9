# LVGL Benchmark for ESP32-S3

This is an LVGL (Light and Versatile Graphics Library) benchmark demo for ESP32-S3 with a 480x272 display using the NV3041A driver and GT911 touch controller.

## Hardware Requirements

- ESP32-S3 development board
- 480x272 TFT LCD with NV3041A controller (QSPI interface)
- GT911 capacitive touch panel
- USB cable for programming

## Software Requirements

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode)
- Windows 10/11 or WSL Ubuntu

## Getting Started

### 1. Install VSCode and PlatformIO

1. Download and install [Visual Studio Code](https://code.visualstudio.com/)
2. Open VSCode
3. Go to Extensions (Ctrl+Shift+X)
4. Search for "PlatformIO IDE"
5. Click Install
6. Restart VSCode after installation

### 2. Open Project

1. Open VSCode
2. Click on PlatformIO icon in the sidebar (alien head icon)
3. Click "Open Project"
4. Navigate to this folder and open it

OR

1. File → Open Folder
2. Select this `LvglBenchmark` folder

### 3. Convert Arduino Project to PlatformIO

Since this is an Arduino .ino project, you need to create a `platformio.ini` configuration file:

**Create `platformio.ini` in this folder with the following content:**

```ini
[env:esp32-s3-devkitc-1]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

; Serial Monitor settings
monitor_speed = 115200
monitor_filters = esp32_exception_decoder

; Upload settings
upload_speed = 921600

; Build settings
build_flags =
    -DBOARD_HAS_PSRAM
    -DARDUINO_USB_CDC_ON_BOOT=1
    -DLV_CONF_INCLUDE_SIMPLE
    -DLV_CONF_PATH="${PROJECT_DIR}/lv_conf.h"

; Library dependencies
lib_deps =
    lvgl/lvgl@^8.3.11
    moononournation/GFX Library for Arduino@^1.4.7
    tamctec/TAMC_GT911@^1.0.2
```

### 4. Project Structure

After conversion, your project should look like:

```
LvglBenchmark/
├── README.md                                    (this file)
├── platformio.ini                               (PlatformIO config)
├── LvglBenchmark.ino                           (main sketch)
├── lv_conf.h                                   (LVGL configuration)
├── touch.h                                     (touch driver config)
├── lv_demo_benchmark.c                         (benchmark demo)
├── lv_demo_benchmark.h
├── img_benchmark_cogwheel_*.c                  (image resources)
├── lv_font_bechmark_montserrat_*.c            (font resources)
└── (other supporting files)
```

### 5. Build the Project

**Option A: Using VSCode GUI**
1. Click the PlatformIO icon in the sidebar
2. Under "PROJECT TASKS", expand your environment
3. Click "Build"

**Option B: Using Terminal**
```bash
# In VSCode terminal (Ctrl+`)
pio run
```

### 6. Upload to ESP32-S3

1. Connect your ESP32-S3 to your computer via USB
2. **Important for ESP32-S3:** Press and hold the BOOT button, then press RESET, then release BOOT to enter download mode (if auto-reset doesn't work)

**Option A: Using VSCode GUI**
1. Click PlatformIO icon
2. Under "PROJECT TASKS" → "General"
3. Click "Upload"

**Option B: Using Terminal**
```bash
pio run --target upload
```

### 7. Monitor Serial Output

**Option A: Using VSCode GUI**
1. Click PlatformIO icon
2. Under "PROJECT TASKS" → "General"
3. Click "Monitor"

**Option B: Using Terminal**
```bash
pio device monitor
```

**Option C: Upload and Monitor (Combined)**
```bash
pio run --target upload && pio device monitor
```

## Troubleshooting

### Cannot Find USB Port

**On Windows:**
- Check Device Manager for the COM port
- Install [CP210x drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) if needed
- Install [CH340 drivers](http://www.wch-ic.com/downloads/CH341SER_EXE.html) if using CH340 USB chip

**On WSL:**
- Use `usbipd-win` to attach USB device to WSL
- See [WSL USB documentation](https://learn.microsoft.com/windows/wsl/connect-usb)

### Build Errors

**"lv_conf.h not found"**
- Make sure `lv_conf.h` exists in the project folder
- Check that `build_flags` in `platformio.ini` includes the LV_CONF_PATH

**Library version conflicts**
- Try updating libraries: `pio lib update`
- Or specify exact versions in `platformio.ini`

### Upload Fails

1. **Press BOOT button** during upload (for ESP32-S3)
2. Try lower upload speed in `platformio.ini`:
   ```ini
   upload_speed = 115200
   ```
3. Check correct USB cable (must support data, not just charging)
4. Try different USB port

### Display Not Working

1. Check hardware connections (QSPI pins)
2. Verify backlight pin (GPIO 1) is connected
3. Check power supply is adequate (ESP32-S3 + display can draw significant current)

### Touch Not Responding

1. Verify GT911 I2C connections (SDA/SCL pins)
2. Check `touch.h` configuration matches your hardware
3. Ensure touch panel is properly calibrated

## Hardware Pin Configuration

Based on the code, this project uses:

**Display (QSPI Interface):**
- CS: GPIO 45
- SCK: GPIO 47
- D0: GPIO 21
- D1: GPIO 48
- D2: GPIO 40
- D3: GPIO 39
- Backlight: GPIO 1

**Touch Panel:**
- See `touch.h` for specific I2C pin configuration

## LVGL Configuration

The project uses a custom `lv_conf.h` with these key settings:
- `LV_COLOR_DEPTH 16` - 16-bit color (RGB565)
- `LV_USE_DEMO_BENCHMARK 1` - Enable benchmark demo
- `LV_USE_FONT_COMPRESSED 1` - Compressed fonts support
- `LV_COLOR_16_SWAP` - Color byte swapping (check your display)

## Performance Tips

1. **Enable PSRAM** - Already configured in `platformio.ini`
2. **Optimize build** - Use `-O2` or `-O3` optimization flags
3. **Direct mode** - Uncomment `#define DIRECT_MODE` in .ino file for full framebuffer (requires more RAM)
4. **Canvas mode** - Default mode uses less RAM but may be slower

## Useful PlatformIO Commands

```bash
# Clean build files
pio run --target clean

# Update libraries
pio lib update

# List installed libraries
pio lib list

# Show device info
pio device list

# Erase flash
pio run --target erase

# Build in verbose mode
pio run -v

# Upload with custom port
pio run --target upload --upload-port COM3
```

## Additional Resources

- [LVGL Documentation](https://docs.lvgl.io/)
- [Arduino_GFX Library](https://github.com/moononournation/Arduino_GFX)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)

## License

This demo is based on LVGL demos. Please check individual library licenses:
- LVGL: MIT License
- Arduino_GFX: BSD License

## Support

For issues specific to this project, check:
1. Hardware connections match pin configuration
2. Libraries are properly installed
3. `lv_conf.h` is correctly configured
4. Serial monitor output for error messages

## Next Steps

After successful upload:
- The benchmark will run automatically
- Check serial monitor (115200 baud) for output
- Benchmark results will be displayed on screen
- Touch screen should be responsive
