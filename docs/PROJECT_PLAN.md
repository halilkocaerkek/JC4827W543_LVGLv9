# Temperature and Humidity Tracker - Project Plan

## Project Overview
Develop a temperature and humidity tracking application for the JC4827W543 ESP32-S3 development board with a menu-driven interface using LVGL v9.

## Hardware Requirements

### Main Components
- **JC4827W543 Development Board** (ESP32-S3 based)
  - 4.3" 480x272 TFT Display with capacitive touch
  - ESP32-S3-WROOM-1 module
  - Up to 10 available GPIO pins via Extended IO connectors

- **DHT11 Temperature and Humidity Sensor**
  - Operating Range: 0-50°C, 20-90% RH
  - Accuracy: ±2°C, ±5% RH
  - 3-pin connection: VCC, DATA, GND

### Pin Assignment
Based on the board specification, Extended IO ports are available at the bottom of the board.

**Selected Pin for DHT11 Data: GPIO 1** (Extended IO)
- Avoids conflict with:
  - Display pins
  - Touch controller (GPIO 3, 4, 8, 38)
  - USB/Serial pins
  - SD card pins

## Software Architecture

### Modular Design
The project will use a modular architecture with separate header files:

```
JC4827W543_LVGLv9/
├── JC4827W543_LVGLv9.ino          # Main Arduino sketch
├── src/
│   ├── config.h                    # Pin definitions and constants
│   ├── sensor_dht11.h             # DHT11 sensor module
│   ├── ui_menu.h                   # Menu screen UI
│   ├── ui_test.h                   # Test screen UI (current demo)
│   ├── ui_temphumid.h             # Temperature/Humidity display UI
│   └── ui_settings.h               # Settings screen UI
├── docs/
│   ├── PROJECT_PLAN.md             # This file
│   ├── BREADBOARD_WIRING.md        # Hardware setup guide
│   └── ARCHITECTURE.md             # Code architecture details
└── README.md                       # Updated project README
```

### Menu Navigation Design
**Menu Type:** Main menu screen with 3 touch buttons

**Menu Items:**
1. **UI Test** - Original demo with button and arc widget
2. **Temperature & Humidity** - Live sensor readings
3. **Settings** - Placeholder for future features

**Navigation Pattern:**
- Main menu appears on startup
- Each menu item is a full-screen button with icon and label
- Selecting an item loads that screen
- Each screen has a "Back" button to return to menu

## Feature Specifications

### Menu Item 1: UI Test Screen
- **Purpose:** Test LVGL widgets and touch functionality
- **Components:**
  - Button with click counter
  - Arc widget with percentage display
  - Back button to return to menu
- **Serial Output:** Button clicks and arc value changes

### Menu Item 2: Temperature & Humidity Display
- **Purpose:** Real-time environmental monitoring
- **Display Elements:**
  - Large temperature reading with °C/°F toggle
  - Large humidity reading with % symbol
  - Last update timestamp
  - Visual indicators (color-coded for comfort zones)
  - Min/Max values since startup
- **Update Interval:** 2 seconds (DHT11 recommended interval)
- **Features:**
  - Automatic sensor error detection
  - Temperature unit toggle button (°C/°F)
  - Reset min/max button
  - Back button to menu
- **Serial Output:** All sensor readings with timestamps

### Menu Item 3: Settings Screen
- **Purpose:** Placeholder for future configuration
- **Current State:** Empty screen with title and back button
- **Future Features:**
  - Temperature unit preference
  - Update interval configuration
  - WiFi settings
  - Data logging options

## Dependencies

### Arduino Libraries Required
1. **lvgl** (v9.2.2) - Already installed
2. **GFX Library for Arduino** (v1.5.6) - Already installed
3. **Dev Device Pins** (v0.0.2) - Already installed
4. **TAMC_GT911** (v1.0.2) - Already installed
5. **DHT sensor library** by Adafruit (v1.4.4+) - **NEW**
6. **Adafruit Unified Sensor** (v1.1.14+) - **NEW** (dependency for DHT)

### Installation Commands
```bash
arduino-cli lib install "DHT sensor library"
arduino-cli lib install "Adafruit Unified Sensor"
```

## Implementation Phases

### Phase 1: Project Structure Setup
- [x] Create directory structure
- [ ] Create all header files with templates
- [ ] Update main .ino file with includes
- [ ] Create configuration file with pin definitions

### Phase 2: DHT11 Sensor Integration
- [ ] Wire DHT11 to GPIO 1
- [ ] Create sensor module with read functions
- [ ] Implement error handling
- [ ] Test sensor readings via serial monitor
- [ ] Add moving average filter for stability

### Phase 3: Menu System Implementation
- [ ] Design menu screen layout
- [ ] Create 3 menu buttons with icons
- [ ] Implement screen switching logic
- [ ] Add navigation state management
- [ ] Test touch navigation

### Phase 4: Screen Implementations
- [ ] Migrate existing demo to UI Test screen
- [ ] Create Temperature/Humidity display screen
- [ ] Create Settings placeholder screen
- [ ] Add back buttons to all screens
- [ ] Test screen transitions

### Phase 5: Data Display Enhancement
- [ ] Implement min/max tracking
- [ ] Add temperature unit conversion
- [ ] Create visual comfort indicators
- [ ] Add timestamp display
- [ ] Implement data logging to serial

### Phase 6: Polish and Testing
- [ ] Add error handling throughout
- [ ] Optimize memory usage
- [ ] Test all navigation paths
- [ ] Verify sensor accuracy
- [ ] Create user documentation

## Code Style Guidelines

### Documentation Standards
- All functions must have comment headers explaining purpose, parameters, and return values
- Complex algorithms require inline comments
- Each module header includes usage instructions
- Constants and pin definitions must be clearly labeled

### Naming Conventions
- **Functions:** camelCase (e.g., `readTemperature()`)
- **Variables:** camelCase (e.g., `currentTemp`)
- **Constants:** UPPER_SNAKE_CASE (e.g., `DHT11_PIN`)
- **LVGL Objects:** prefix with screen name (e.g., `menu_btn1`, `temp_label`)

### Memory Management
- Use static allocation where possible
- Clean up LVGL objects when switching screens
- Monitor heap usage during development
- Use PSRAM for large buffers if needed

## Testing Strategy

### Unit Testing
- Test DHT11 sensor readings independently
- Verify each screen renders correctly
- Test menu navigation in all directions
- Verify serial output formatting

### Integration Testing
- Test complete user workflows
- Verify memory stability over time
- Test error recovery (sensor disconnect)
- Validate touch responsiveness

### Edge Cases
- DHT11 read failures
- Extreme temperature values
- Rapid screen switching
- Long-term operation (memory leaks)

## Performance Targets
- Screen transition: < 100ms
- Sensor read: < 250ms
- UI refresh rate: Maintain LVGL 60fps target
- Memory usage: < 70% of available RAM

## Future Enhancements (Post-MVP)
- WiFi connectivity for remote monitoring
- Data logging to SD card
- Historical graphs and charts
- Multiple sensor support
- Alarm thresholds with notifications
- Web interface for remote access
- MQTT integration for IoT platforms

## Timeline Estimate
This is a modular implementation - work proceeds feature by feature:
1. Structure and sensor: Foundation work
2. Menu system: Core navigation
3. Screens: Individual features
4. Polish: Refinement and testing

## Success Criteria
- [x] All 3 menu items accessible via touch
- [ ] DHT11 sensor provides accurate readings
- [ ] Temperature/humidity display updates every 2 seconds
- [ ] No memory leaks after 1 hour of operation
- [ ] All navigation paths work correctly
- [ ] Code is well-documented and maintainable
- [ ] Serial output provides debugging information

## Version Control
- Commit after each major feature completion
- Tag releases with semantic versioning
- Maintain CHANGELOG.md for user-facing changes
