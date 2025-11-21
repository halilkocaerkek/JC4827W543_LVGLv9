# Temperature & Humidity Tracker - JC4827W543 LVGLv9

A modular temperature and humidity monitoring application for the JC4827W543 ESP32-S3 development board with LVGL v9 GUI framework.

<a href="https://www.buymeacoffee.com/thelastoutpostworkshop" target="_blank">
<img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee">
</a>

## Features

### 📱 Menu-Driven Interface
- Clean, touch-based navigation system
- 3 main screens with intuitive controls
- Smooth screen transitions

### 🌡️ Temperature & Humidity Monitoring
- Real-time DHT11 sensor readings
- Temperature display with °C/°F toggle
- Humidity percentage display
- Color-coded comfort zone indicators
- Min/Max value tracking
- Automatic sensor error detection
- Update interval: 2 seconds

### 🎨 UI Test Screen
- Interactive button with click counter
- Arc widget with percentage display
- Touch responsiveness testing
- Serial output for debugging

### ⚙️ Settings (Placeholder)
- Reserved for future configuration options
- Planned features:
  - Temperature unit preference
  - Update interval configuration
  - WiFi settings
  - Data logging options

## Hardware Requirements

### Main Components
- **JC4827W543 Development Board**
  - ESP32-S3-WROOM-1 module
  - 4.3" 480x272 TFT display
  - GT911 capacitive touch controller
  - Up to 10 available GPIO pins

- **DHT11 Temperature and Humidity Sensor**
  - Operating Range: 0-50°C, 20-90% RH
  - Accuracy: ±2°C, ±5% RH
  - Connected to GPIO 1

### Wiring
See [docs/BREADBOARD_WIRING.md](docs/BREADBOARD_WIRING.md) for detailed wiring instructions.

**Quick Connection:**
```
DHT11 Module    →    JC4827W543 Board
VCC             →    3.3V (Extended IO)
DATA            →    GPIO 1 (Extended IO)
GND             →    GND (Extended IO)
```

## Software Requirements

### Arduino IDE Setup
1. **Board:** ESP32S3 Dev Module
2. **esp32 Arduino Core:** v3.2.0 by Espressif
3. **LVGL Configuration:** Configure `lv_conf.h` per [LVGL docs](https://docs.lvgl.io/master/get-started/platforms/arduino.html)

### Required Libraries
Install via Arduino Library Manager or Arduino CLI:

```bash
# Core libraries (already installed if you ran the original demo)
arduino-cli lib install "lvgl@9.2.2"
arduino-cli lib install "GFX Library for Arduino@1.5.6"
arduino-cli lib install "Dev Device Pins@0.0.2"
arduino-cli lib install "TAMC_GT911@1.0.2"

# New libraries for DHT11 sensor
arduino-cli lib install "DHT sensor library"
arduino-cli lib install "Adafruit Unified Sensor"
```

## Quick Start

### 1. Install Dependencies
Make sure all required libraries are installed (see above).

### 2. Wire the DHT11 Sensor
Follow the wiring diagram in [docs/BREADBOARD_WIRING.md](docs/BREADBOARD_WIRING.md).

### 3. Upload the Sketch
```bash
# Using Arduino CLI
arduino-cli compile --fqbn esp32:esp32:esp32s3 JC4827W543_LVGLv9.ino
arduino-cli upload -p <PORT> --fqbn esp32:esp32:esp32s3 JC4827W543_LVGLv9.ino

# Using Arduino IDE
# Open JC4827W543_LVGLv9.ino → Select board → Upload
```

### 4. Monitor Serial Output
```bash
arduino-cli monitor -p <PORT> -c baudrate=115200
```

You should see:
```
Temperature & Humidity Tracker
Board: JC4827W543 (ESP32-S3)
LVGL v9.2.2
Display initialized successfully
Touch controller initialized
DHT11 initialized successfully
Setup complete! System ready.
```

### 5. Navigate the Interface
- Main menu appears on startup
- Touch a button to navigate to that screen
- Use "Back" button to return to menu
- On Temp/Humidity screen:
  - Touch "°C" to toggle temperature unit
  - Touch "Reset" to reset min/max values

## Project Structure

```
JC4827W543_LVGLv9/
├── JC4827W543_LVGLv9.ino       # Main Arduino sketch
├── src/                         # Source modules
│   ├── config.h                 # Configuration & constants
│   ├── sensor_dht11.h          # DHT11 sensor module
│   ├── ui_menu.h                # Main menu screen
│   ├── ui_test.h                # UI test screen
│   ├── ui_temphumid.h          # Temperature/Humidity screen
│   └── ui_settings.h            # Settings screen
├── docs/                        # Documentation
│   ├── PROJECT_PLAN.md          # Project specifications
│   ├── BREADBOARD_WIRING.md     # Hardware setup guide
│   ├── ARCHITECTURE.md          # Code architecture details
│   └── JC4827W543/             # Factory documentation
├── README.md                    # This file
└── CLAUDE.md                    # AI assistant guidance
```

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed code architecture documentation.

## Configuration

Edit [src/config.h](src/config.h) to customize:

### Sensor Settings
```cpp
#define DHT11_PIN 1                    // GPIO pin for DHT11
#define SENSOR_READ_INTERVAL 2000      // Read interval (ms)
#define SENSOR_MOVING_AVG_SAMPLES 5    // Smoothing samples
```

### Comfort Thresholds
```cpp
#define TEMP_COMFORT_MIN 20.0          // Comfort zone min (°C)
#define TEMP_COMFORT_MAX 26.0          // Comfort zone max (°C)
#define HUMIDITY_COMFORT_MIN 40.0      // Comfort zone min (%)
#define HUMIDITY_COMFORT_MAX 60.0      // Comfort zone max (%)
```

### Debug Options
```cpp
#define DEBUG_SERIAL true              // Enable serial debug
#define DEBUG_SENSOR_VALUES true       // Print sensor readings
#define DEBUG_NAVIGATION true          // Print navigation events
```

### Display Colors
```cpp
#define COLOR_PRIMARY lv_color_hex(0x2196F3)    // Blue
#define COLOR_SECONDARY lv_color_hex(0x4CAF50)  // Green
#define COLOR_ACCENT lv_color_hex(0xFF9800)     // Orange
```

## Modular Architecture

This project uses a clean, modular architecture for easy maintenance and extension:

- **Separation of Concerns:** Each module handles a specific responsibility
- **Well-Documented:** Comprehensive inline documentation and comments
- **Extensible:** Easy to add new screens, sensors, or features
- **Maintainable:** Consistent coding style and clear structure

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for implementation details.

## Serial Debug Output

### Sensor Readings (DEBUG_SENSOR_VALUES enabled)
```
[DHT11] T: 23.5°C, H: 45.2%, Min T: 20.1°C, Max T: 25.3°C
```

### Navigation Events (DEBUG_NAVIGATION enabled)
```
[MENU] Navigating to Temperature/Humidity screen
[TEMP/HUMID] Creating temperature/humidity screen
[TEMP/HUMID] Temperature unit changed to: Fahrenheit
```

## Troubleshooting

### No Sensor Readings
- **Check wiring:** Verify DHT11 is connected to GPIO 1
- **Check power:** Ensure DHT11 VCC is connected to 3.3V
- **Check ground:** Verify GND connection
- **Check resistor:** If using bare DHT11 sensor, add 10kΩ pull-up resistor

### "Sensor Error" Message
- **Loose connection:** Reseat all jumper wires
- **Wrong pin:** Verify GPIO 1 is used (not conflicting with display/touch)
- **Sensor failure:** Test DHT11 with simple example sketch

### Display Not Working
- **Library issue:** Verify all libraries are installed correctly
- **LVGL config:** Ensure `lv_conf.h` is configured properly
- **Serial output:** Check serial monitor for error messages

### Touch Not Responding
- **Touch controller:** GT911 may need reinitialization
- **Rotation:** Try changing `ROTATION_INVERTED` setting
- **Calibration:** Touch controller auto-calibrates on boot

### Compile Errors
- **Missing libraries:** Install all required libraries
- **Wrong board:** Select "ESP32S3 Dev Module" in Arduino IDE
- **esp32 core version:** Use v3.2.0 or compatible version

## Future Enhancements

### Planned Features (Phase 2)
- WiFi connectivity for remote monitoring
- Web server interface
- Data logging to SD card
- EEPROM settings persistence
- Alarm thresholds with notifications

### Possible Additions (Phase 3)
- Multiple sensor support (BME280, SHT30, etc.)
- Historical graphs and charts
- MQTT integration for IoT platforms
- Over-the-air (OTA) updates
- Battery level monitoring
- LCD sleep mode for power saving

## Documentation

- **[Project Plan](docs/PROJECT_PLAN.md)** - Project specifications and timeline
- **[Wiring Guide](docs/BREADBOARD_WIRING.md)** - Hardware setup instructions
- **[Architecture](docs/ARCHITECTURE.md)** - Code structure and design
- **[CLAUDE.md](CLAUDE.md)** - AI assistant guidance for development

## Youtube Tutorial (Original LVGL Demo)
[<img src="https://github.com/thelastoutpostworkshop/images/blob/main/lvgl_v9.png" width="500">](https://youtu.be/mnOzfRFQJIM)

## Contributing

This is a modular, well-documented codebase designed for easy extension. To add features:

1. Review [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for code structure
2. Create new modules in `src/` directory
3. Follow existing naming conventions and documentation style
4. Test thoroughly before committing

## License

This project builds upon open-source libraries and the original LVGL demo. Check individual library licenses for details.

## Credits

- **LVGL** - Graphics library (https://lvgl.io/)
- **Arduino GFX Library** - Display driver
- **Adafruit DHT Library** - Sensor driver
- **JC4827W543 Board** - GUITION/Shenzhen Jingcai
- **Original Tutorial** - https://youtu.be/mnOzfRFQJIM

## Support

For issues, questions, or feature requests:
- Check [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for technical details
- Review [docs/BREADBOARD_WIRING.md](docs/BREADBOARD_WIRING.md) for wiring help
- Open an issue on GitHub with serial output and description

---

**Built with ❤️ for the ESP32-S3 and LVGL community**
