/**
 * @file config.h
 * @brief Configuration file for Temperature and Humidity Tracker
 *
 * This file contains all pin definitions, constants, and configuration
 * parameters for the JC4827W543 temperature and humidity tracker project.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// HARDWARE PIN DEFINITIONS
// ============================================================================

// DHT11 Sensor Pin
#define DHT11_PIN 1              // GPIO 1 - Extended IO connector
#define DHT11_TYPE DHT11         // Sensor type

// Touch Controller Pins (already defined by PINS_JC4827W543.h)
// TOUCH_SDA 8
// TOUCH_SCL 4
// TOUCH_INT 3
// TOUCH_RST 38

// Display Pins (handled by gfx library)
// GFX_BL is defined in PINS_JC4827W543.h

// ============================================================================
// SENSOR CONFIGURATION
// ============================================================================

#define SENSOR_READ_INTERVAL 2000   // DHT11 read interval in milliseconds (2 seconds)
#define SENSOR_RETRY_ATTEMPTS 3     // Number of retry attempts for failed reads
#define SENSOR_MOVING_AVG_SAMPLES 5 // Number of samples for moving average filter

// Temperature thresholds for comfort zones (Celsius)
#define TEMP_COLD_THRESHOLD 18.0    // Below this is cold (blue)
#define TEMP_COMFORT_MIN 20.0       // Comfort zone minimum
#define TEMP_COMFORT_MAX 26.0       // Comfort zone maximum
#define TEMP_HOT_THRESHOLD 28.0     // Above this is hot (red)

// Humidity thresholds
#define HUMIDITY_LOW_THRESHOLD 30.0  // Below this is dry (yellow)
#define HUMIDITY_COMFORT_MIN 40.0    // Comfort zone minimum
#define HUMIDITY_COMFORT_MAX 60.0    // Comfort zone maximum
#define HUMIDITY_HIGH_THRESHOLD 70.0 // Above this is humid (blue)

// ============================================================================
// DISPLAY CONFIGURATION
// ============================================================================

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

// LVGL Colors (Brightened for better visibility)
#define COLOR_PRIMARY lv_color_hex(0x2196F3)    // Blue
#define COLOR_SECONDARY lv_color_hex(0x4CAF50)  // Green
#define COLOR_ACCENT lv_color_hex(0xFF9800)     // Orange
#define COLOR_DANGER lv_color_hex(0xF44336)     // Red
#define COLOR_BACKGROUND lv_color_hex(0x000000) // Black (was too dark gray)
#define COLOR_TEXT lv_color_hex(0xFFFFFF)       // White
#define COLOR_TEXT_SECONDARY lv_color_hex(0xE0E0E0) // Brighter light gray

// Menu button dimensions
#define MENU_BUTTON_WIDTH 140
#define MENU_BUTTON_HEIGHT 180
#define MENU_BUTTON_SPACING 20

// Navigation button dimensions
#define NAV_BUTTON_WIDTH 100
#define NAV_BUTTON_HEIGHT 40

// ============================================================================
// APPLICATION CONFIGURATION
// ============================================================================

// Temperature units
enum TempUnit {
  TEMP_CELSIUS = 0,
  TEMP_FAHRENHEIT = 1
};

// Screen identifiers
enum ScreenID {
  SCREEN_MENU = 0,
  SCREEN_TEST = 1,
  SCREEN_TEMP_HUMID = 2,
  SCREEN_SETTINGS = 3
};

// Default settings
#define DEFAULT_TEMP_UNIT TEMP_CELSIUS
#define SERIAL_BAUD_RATE 115200

// ============================================================================
// DEBUG CONFIGURATION
// ============================================================================

#define DEBUG_SERIAL true          // Enable serial debug output
#define DEBUG_SENSOR_VALUES true   // Print sensor readings to serial
#define DEBUG_NAVIGATION true      // Print screen navigation events
#define DEBUG_MEMORY false         // Print memory usage statistics

// Helper macro for debug printing
#if DEBUG_SERIAL
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(x, y) Serial.printf(x, y)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(x, y)
#endif

// ============================================================================
// MEMORY CONFIGURATION
// ============================================================================

// LVGL buffer size (defined in main sketch)
// Using screenWidth * 40 for partial rendering

#endif // CONFIG_H
