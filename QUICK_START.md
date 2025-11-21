# Quick Start Guide - Temperature & Humidity Tracker

## 🚀 Get Running in 5 Minutes

### Step 1: Install DHT Libraries (30 seconds)

Open Arduino IDE Library Manager or use CLI:

```bash
arduino-cli lib install "DHT sensor library"
arduino-cli lib install "Adafruit Unified Sensor"
```

### Step 2: Wire DHT11 Sensor (2 minutes)

Connect 3 wires from DHT11 to your board:

```
DHT11 Pin    →    JC4827W543 Board (Extended IO)
─────────────────────────────────────────────────
VCC (red)    →    3.3V
DATA (yellow)→    GPIO 1
GND (black)  →    GND
```

**Extended IO connectors are at the bottom edge of the board.**

### Step 3: Upload Code (1 minute)

**Arduino IDE:**
1. Open `JC4827W543_LVGLv9.ino`
2. Select Board: "ESP32S3 Dev Module"
3. Select your COM port
4. Click Upload ⬆️

**Arduino CLI:**
```bash
arduino-cli compile --fqbn esp32:esp32:esp32s3 JC4827W543_LVGLv9.ino
arduino-cli upload -p COM3 --fqbn esp32:esp32:esp32s3 JC4827W543_LVGLv9.ino
```

### Step 4: Test It! (1 minute)

Open Serial Monitor at **115200 baud**.

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

**On the display:**
- Menu with 3 buttons appears
- Touch "Temp & Humidity" to see sensor readings
- Touch "UI Test" to test LVGL widgets
- Touch "Settings" to see placeholder screen

### Step 5: Enjoy! 🎉

Your temperature and humidity tracker is now running!

## Quick Troubleshooting

### "DHT11 initialization failed"
- Check wiring - especially GPIO 1
- Make sure DHT11 has power (3.3V)
- Reseat all connections

### Display is blank
- Check serial monitor for errors
- Verify all libraries are installed
- Try power cycling the board

### No touch response
- Touch controller auto-calibrates on boot
- Try restarting the board

## What's Next?

- **Customize:** Edit [src/config.h](src/config.h) to change colors, thresholds, etc.
- **Learn:** Read [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) to understand the code
- **Extend:** Add new features following the modular pattern
- **Full Docs:** See [README.md](README.md) for complete documentation

## Key Features Available Now

✅ Real-time temperature and humidity monitoring
✅ °C/°F temperature unit toggle
✅ Min/Max value tracking with reset
✅ Color-coded comfort zone indicators
✅ Touch-based menu navigation
✅ UI testing screen
✅ Serial debug output

## Configuration Quick Reference

Edit [src/config.h](src/config.h):

```cpp
// Change DHT11 pin
#define DHT11_PIN 1

// Change update interval (milliseconds)
#define SENSOR_READ_INTERVAL 2000

// Change comfort thresholds (Celsius)
#define TEMP_COMFORT_MIN 20.0
#define TEMP_COMFORT_MAX 26.0
```

## File Structure Quick Reference

```
JC4827W543_LVGLv9.ino          ← Main sketch (upload this)
src/config.h                    ← Configuration
src/sensor_dht11.h             ← Sensor handling
src/ui_menu.h                   ← Main menu
src/ui_temphumid.h             ← Temp/Humidity screen
src/ui_test.h                   ← UI test screen
src/ui_settings.h               ← Settings screen
```

## Support

- **Wiring Help:** [docs/BREADBOARD_WIRING.md](docs/BREADBOARD_WIRING.md)
- **Full Guide:** [README.md](README.md)
- **Code Details:** [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)

---

**That's it! You're ready to go!** 🚀
