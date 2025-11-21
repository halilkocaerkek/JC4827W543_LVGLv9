# Code Architecture Documentation

## Overview
This document describes the modular architecture of the Temperature and Humidity Tracker application for the JC4827W543 ESP32-S3 development board.

## Design Philosophy

### Modularity
The codebase is organized into separate, focused modules that handle specific responsibilities:
- **Configuration** - Centralized constants and settings
- **Sensor Management** - DHT11 sensor interfacing and data processing
- **UI Modules** - Individual screen implementations with encapsulated logic

### Maintainability
- Clear separation of concerns
- Comprehensive inline documentation
- Consistent naming conventions
- Minimal coupling between modules

### Scalability
The modular design allows easy addition of:
- New sensors
- Additional screens
- Enhanced features
- Configuration options

## Directory Structure

```
JC4827W543_LVGLv9/
├── JC4827W543_LVGLv9.ino       # Main Arduino sketch (entry point)
├── src/                         # Source modules directory
│   ├── config.h                 # Configuration and constants
│   ├── sensor_dht11.h          # DHT11 sensor module
│   ├── ui_menu.h                # Main menu screen
│   ├── ui_test.h                # UI test screen (LVGL demo)
│   ├── ui_temphumid.h          # Temperature/Humidity display
│   └── ui_settings.h            # Settings screen (placeholder)
├── docs/                        # Documentation
│   ├── PROJECT_PLAN.md          # Project plan and specifications
│   ├── BREADBOARD_WIRING.md     # Hardware wiring guide
│   ├── ARCHITECTURE.md          # This file
│   └── JC4827W543/             # Factory documentation
├── README.md                    # Project overview
└── CLAUDE.md                    # AI assistant guidance

```

## Module Descriptions

### 1. Main Sketch (JC4827W543_LVGLv9.ino)

**Purpose:** Application entry point and hardware initialization

**Responsibilities:**
- Initialize serial communication
- Configure display hardware (GFX library)
- Initialize touch controller (GT911)
- Set up LVGL framework
- Initialize DHT11 sensor
- Create initial menu screen
- Main event loop coordination

**Key Functions:**
- `setup()` - One-time hardware and software initialization
- `loop()` - Main event loop (LVGL task handler, sensor updates, display refresh)
- `my_disp_flush()` - LVGL callback for display rendering
- `my_touchpad_read()` - LVGL callback for touch input
- `millis_cb()` - LVGL callback for timing

**Dependencies:**
- All module headers (config, sensor, UI modules)
- LVGL library
- GFX Library for Arduino
- TAMC_GT911 touch library
- DHT sensor library

### 2. Configuration Module (src/config.h)

**Purpose:** Centralized configuration and constants

**Contents:**
- **Pin Definitions:** GPIO assignments for DHT11 sensor
- **Sensor Configuration:** Read intervals, thresholds, averaging parameters
- **Display Configuration:** Screen dimensions, colors, layout constants
- **Application Configuration:** Enumerations for units and screens
- **Debug Configuration:** Debug flags and macros

**Key Constants:**
```cpp
DHT11_PIN                  // GPIO 1
SENSOR_READ_INTERVAL       // 2000ms
TEMP_COMFORT_MIN/MAX       // 20-26°C
HUMIDITY_COMFORT_MIN/MAX   // 40-60%
COLOR_PRIMARY              // Blue (#2196F3)
COLOR_SECONDARY            // Green (#4CAF50)
```

**Usage Pattern:**
All modules include `config.h` to access shared constants and configuration.

### 3. Sensor Module (src/sensor_dht11.h)

**Purpose:** DHT11 sensor management and data processing

**Features:**
- Sensor initialization and health checking
- Periodic reading with error handling
- Moving average filter (5 samples) for stability
- Min/Max value tracking
- Temperature unit conversion (°C/°F)
- Serial debugging output

**Key Functions:**
```cpp
bool initDHT11()                    // Initialize sensor
void updateDHT11()                  // Read sensor (call in loop)
float getCurrentTemperature()       // Get current temp (°C)
float getCurrentHumidity()          // Get current humidity (%)
float getTemperature(TempUnit)      // Get temp in specified unit
bool isDHT11Ready()                 // Check sensor status
void resetMinMax()                  // Reset min/max tracking
```

**Data Flow:**
1. `updateDHT11()` called in main loop
2. Checks if read interval elapsed
3. Reads DHT11 sensor via Adafruit library
4. Applies moving average filter
5. Updates min/max values
6. Logs to serial if debug enabled

**Error Handling:**
- NaN detection from sensor
- Consecutive error counting
- Sensor ready/not ready flag
- Graceful degradation (continues operation)

### 4. Menu Screen Module (src/ui_menu.h)

**Purpose:** Main menu interface with 3 navigation buttons

**Layout:**
```
┌────────────────────────────────────┐
│   Temperature & Humidity Tracker   │
│                                    │
│  ┌──────┐  ┌──────┐  ┌──────┐    │
│  │ UI   │  │ Temp │  │Settings│   │
│  │ Test │  │  &   │  │        │   │
│  │      │  │Humid │  │        │   │
│  └──────┘  └──────┘  └──────┘    │
│                                    │
│        ESP32-S3 | LVGL v9.2       │
└────────────────────────────────────┘
```

**Key Functions:**
```cpp
void createMenuScreen()                      // Build and display menu
void destroyMenuScreen()                     // Clean up resources
lv_obj_t* createMenuButton(...)             // Helper for button creation
```

**Event Handlers:**
- `menu_btn1_event_cb()` - Navigate to UI Test
- `menu_btn2_event_cb()` - Navigate to Temp/Humid
- `menu_btn3_event_cb()` - Navigate to Settings

**Navigation Pattern:**
Menu → Screen → Back → Menu (circular navigation)

### 5. UI Test Screen Module (src/ui_test.h)

**Purpose:** LVGL widget testing (original demo functionality)

**Features:**
- Button with click counter
- Arc widget with percentage display
- Serial output for events
- Back button to menu

**Key Functions:**
```cpp
void createTestScreen()       // Build and display test screen
void destroyTestScreen()      // Clean up resources
```

**Event Handlers:**
- `btn_event_cb()` - Increment counter on button click
- `value_changed_event_cb()` - Update arc percentage label
- `back_button_event_cb()` - Return to menu

**Purpose:** Provides a sandbox for testing LVGL responsiveness and touch functionality.

### 6. Temperature/Humidity Screen Module (src/ui_temphumid.h)

**Purpose:** Real-time sensor data visualization

**Layout:**
```
┌───────────────────────────────────────┐
│    Temperature & Humidity             │
│  ┌──────────┐      ┌──────────┐      │
│  │Temperature│      │ Humidity │      │
│  │  23.5°C  │      │  45.2%   │      │
│  │Comfortable│      │Comfortable│     │
│  └──────────┘      └──────────┘      │
│  Min: 20.1°C/40% | Max: 25.3°C/48%  │
│  Status: OK        Uptime: 00:15:23  │
│  [Back] [°C] [Reset]                 │
└───────────────────────────────────────┘
```

**Features:**
- Large temperature and humidity displays
- Color-coded comfort indicators
- Min/Max value tracking
- Temperature unit toggle (°C/°F)
- Reset min/max button
- Real-time uptime display
- Sensor status indicator

**Key Functions:**
```cpp
void createTempHumidScreen()           // Build and display screen
void updateTempHumidDisplay()          // Refresh values (500ms interval)
void destroyTempHumidScreen()          // Clean up resources
lv_color_t getTemperatureColor(temp)   // Get comfort zone color
const char* getTemperatureStatus(temp) // Get status text
```

**Color Coding:**
- **Temperature:**
  - Blue (< 18°C): Cold
  - Light Blue (18-20°C): Cool
  - Green (20-26°C): Comfortable
  - Orange (26-28°C): Warm
  - Red (> 28°C): Hot

- **Humidity:**
  - Yellow (< 30%): Dry
  - Amber (30-40%): Low
  - Green (40-60%): Comfortable
  - Light Blue (60-70%): High
  - Blue (> 70%): Very Humid

**Update Mechanism:**
- Main loop checks if temphumid_screen is active
- Calls `updateTempHumidDisplay()` every 500ms
- Function reads current sensor values
- Applies color coding and formatting
- Updates all labels and indicators

### 7. Settings Screen Module (src/ui_settings.h)

**Purpose:** Placeholder for future configuration options

**Current State:**
- Title and "coming soon" message
- List of planned features
- Back button to menu

**Planned Features:**
- Temperature unit preference (persist to EEPROM)
- Sensor update interval configuration
- WiFi settings
- Data logging options
- Display brightness control
- Alarm thresholds

**Key Functions:**
```cpp
void createSettingsScreen()    // Build and display settings
void destroySettingsScreen()   // Clean up resources
```

**Extensibility:**
This module is designed as a template for adding configuration UI in future development phases.

## Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                     Main Loop (loop())                       │
│  ┌────────────┐  ┌──────────┐  ┌───────────────────┐      │
│  │lv_task_    │  │updateDHT11│  │updateTempHumid    │      │
│  │handler()   │  │()         │  │Display() (if active)│     │
│  └────────────┘  └──────────┘  └───────────────────┘      │
└────────┬───────────────┬────────────────┬──────────────────┘
         │               │                │
         ▼               ▼                ▼
    ┌────────┐    ┌──────────┐    ┌──────────────┐
    │ LVGL   │    │ DHT11    │    │ UI Update    │
    │ Events │    │ Read     │    │ (500ms)      │
    └────┬───┘    └────┬─────┘    └──────┬───────┘
         │             │                   │
         ▼             ▼                   ▼
    ┌────────┐    ┌──────────┐    ┌──────────────┐
    │Touch   │    │Moving    │    │Color Code    │
    │Handler │    │Average   │    │Status Text   │
    └────┬───┘    │Filter    │    └──────┬───────┘
         │        └────┬─────┘            │
         │             │                   │
         ▼             ▼                   ▼
    ┌────────────────────────────────────────┐
    │       Screen Event Handlers             │
    │  (Button clicks, navigation, etc.)      │
    └────────────────────────────────────────┘
```

## Screen Navigation State Machine

```
         ┌──────────┐
         │   MENU   │ ◄──────────────────────┐
         └─────┬────┘                        │
           ┌───┴───┬─────────┐               │
           │       │         │               │
           ▼       ▼         ▼               │
      ┌──────┐ ┌─────┐ ┌──────────┐         │
      │ TEST │ │TEMP │ │ SETTINGS │         │
      │      │ │HUMID│ │          │         │
      └───┬──┘ └──┬──┘ └────┬─────┘         │
          │       │         │                │
          └───────┴─────────┴────────────────┘
                 (Back buttons)
```

## Memory Management

### LVGL Buffer Allocation
- **Size:** `screenWidth * 40 * 2` pixels
- **Strategy:** Partial rendering mode
- **Fallback:** Try MALLOC_CAP_INTERNAL first, then generic MALLOC_CAP_8BIT

### Screen Objects
- Each screen creates its widgets when displayed
- Previous screen is cleaned with `lv_obj_clean(lv_screen_active())`
- Only one screen active at a time (reduces memory footprint)

### Sensor Data
- Static global variables in sensor module
- Fixed-size moving average buffers (5 samples)
- Minimal heap allocation

## Debug and Logging

### Serial Output Categories

**Initialization:**
```
Temperature & Humidity Tracker
Board: JC4827W543 (ESP32-S3)
LVGL v9.2.2
Display initialized successfully
Touch controller initialized
DHT11 initialized successfully
```

**Navigation (if DEBUG_NAVIGATION enabled):**
```
[MENU] Navigating to Temperature/Humidity screen
[TEMP/HUMID] Creating temperature/humidity screen
[TEMP/HUMID] Navigating back to menu
```

**Sensor Readings (if DEBUG_SENSOR_VALUES enabled):**
```
[DHT11] T: 23.5°C, H: 45.2%, Min T: 20.1°C, Max T: 25.3°C
[TEMP/HUMID] Temperature unit changed to: Fahrenheit
```

### Debug Flags (config.h)
```cpp
#define DEBUG_SERIAL true           // Master debug flag
#define DEBUG_SENSOR_VALUES true    // Sensor readings
#define DEBUG_NAVIGATION true       // Screen transitions
#define DEBUG_MEMORY false          // Heap usage
```

## Extension Guidelines

### Adding a New Screen

1. **Create header file** in `src/` (e.g., `ui_newscreen.h`)
2. **Include** config.h and lvgl.h
3. **Declare** global screen objects
4. **Implement:**
   - `createNewScreen()` - Build UI
   - `destroyNewScreen()` - Cleanup
   - Event callbacks (back button, interactions)
   - `extern void createMenuScreen()` for navigation
5. **Add to menu** in `ui_menu.h`:
   - Create menu button
   - Add event callback
6. **Include** in main .ino file

### Adding a New Sensor

1. **Create header file** in `src/` (e.g., `sensor_newtype.h`)
2. **Implement:**
   - Initialization function
   - Update function (call in main loop)
   - Data accessor functions
   - Error handling
3. **Add pin definitions** to config.h
4. **Initialize** in `setup()`
5. **Update** in `loop()`
6. **Display** in relevant UI screen

### Configuration Changes

1. **Edit** `src/config.h`
2. **Update** constants/enums
3. **Recompile** entire project (header-only modules)

## Performance Considerations

### Target Metrics
- **Screen Transition:** < 100ms
- **Sensor Read:** < 250ms (DHT11 hardware limit)
- **UI Refresh Rate:** 60fps (LVGL target)
- **Memory Usage:** < 70% of available RAM

### Optimization Strategies
- Partial rendering (40-line buffer)
- Sensor update throttling (2-second interval)
- Display update throttling (500ms for temp/humid screen)
- Moving average filter reduces sensor noise
- Single active screen reduces memory

### Watchdog Prevention
- 5ms delay in main loop
- LVGL task handler runs each iteration
- No blocking operations in callbacks

## Testing Strategy

### Unit Testing (Manual)
- **Sensor Module:** Test with known temperature/humidity
- **Each Screen:** Verify rendering and touch response
- **Navigation:** Test all paths (menu ↔ screens)

### Integration Testing
- **Complete Workflow:** Menu → Each screen → Back
- **Sensor Error Recovery:** Disconnect DHT11, reconnect
- **Memory Stability:** Run for 1+ hours, monitor heap
- **Touch Responsiveness:** Rapid screen switching

### Edge Cases
- DHT11 read failures
- Extreme temperature values (0°C, 50°C)
- Rapid button presses
- Unit conversion accuracy

## Future Enhancements

### Immediate (Phase 2)
- WiFi connectivity
- Web server for remote monitoring
- Data logging to SD card
- EEPROM settings persistence

### Medium Term (Phase 3)
- Multiple sensor support
- Historical graphs
- Alarm thresholds with buzzer
- LCD sleep mode (power saving)

### Long Term (Phase 4)
- MQTT integration
- Cloud data sync
- Mobile app companion
- Over-the-air (OTA) updates

## Troubleshooting Guide

### Common Issues

| Symptom | Likely Cause | Solution |
|---------|-------------|----------|
| No sensor readings | DHT11 not connected | Check GPIO 1 wiring |
| "Sensor Error" | Loose connection | Reseat jumper wires |
| Random values | No pull-up resistor | Add 10kΩ resistor (bare sensor) |
| Display blank | LVGL init failed | Check serial output, verify libraries |
| Touch not working | GT911 init failed | Check touch controller wiring |
| Compile errors | Missing libraries | Install all required libraries |

### Debug Workflow
1. Open Serial Monitor (115200 baud)
2. Check initialization messages
3. Enable debug flags in config.h
4. Monitor sensor readings
5. Test navigation events
6. Check memory usage if crashes occur

## Code Style Guide

### Naming Conventions
- **Functions:** `camelCase` (e.g., `updateDHT11()`)
- **Variables:** `camelCase` (e.g., `currentTemp`)
- **Constants:** `UPPER_SNAKE_CASE` (e.g., `DHT11_PIN`)
- **LVGL Objects:** `screenname_objecttype` (e.g., `menu_btn1`)

### Documentation
- All functions have doc comments
- Complex logic has inline comments
- Module headers explain purpose and usage
- Pin assignments clearly labeled

### File Organization
- Header guards (#ifndef/#define/#endif)
- Includes at top
- Global variables section
- Helper functions
- Event handlers
- Main functions (create/destroy)

## Dependencies and Versions

### Required Arduino Libraries
```
lvgl@9.2.2
GFX Library for Arduino@1.5.6
Dev Device Pins@0.0.2
TAMC_GT911@1.0.2
DHT sensor library@1.4.4+
Adafruit Unified Sensor@1.1.14+
```

### Board Configuration
```
Board: ESP32S3 Dev Module
esp32 Arduino Core: v3.2.0 by Espressif
```

### Installation
```bash
arduino-cli lib install "lvgl"
arduino-cli lib install "GFX Library for Arduino"
arduino-cli lib install "Dev Device Pins"
arduino-cli lib install "TAMC_GT911"
arduino-cli lib install "DHT sensor library"
arduino-cli lib install "Adafruit Unified Sensor"
```

## License and Credits

This project builds upon:
- [LVGL](https://lvgl.io/) - Graphics library
- [Arduino GFX Library](https://github.com/moononournation/Arduino_GFX) - Display driver
- [Adafruit DHT Library](https://github.com/adafruit/DHT-sensor-library) - Sensor driver
- JC4827W543 board by GUITION/Shenzhen Jingcai

Original LVGL demo: https://youtu.be/mnOzfRFQJIM
