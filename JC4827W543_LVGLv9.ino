/**
 * @file JC4827W543_LVGLv9.ino
 * @brief Temperature and Humidity Tracker for JC4827W543 Development Board
 *
 * This application provides a menu-driven interface for monitoring temperature
 * and humidity using a DHT11 sensor, along with UI testing capabilities.
 *
 * Hardware:
 * - JC4827W543 ESP32-S3 board with 4.3" TFT display (480x272)
 * - GT911 capacitive touch controller
 * - DHT11 temperature/humidity sensor on GPIO 1
 *
 * Features:
 * - Menu system with 3 screens
 * - Real-time temperature and humidity monitoring
 * - Min/Max value tracking
 * - Temperature unit conversion (°C/°F)
 * - UI test screen with LVGL widgets
 * - Settings placeholder for future features
 *
 * Tutorial: https://youtu.be/mnOzfRFQJIM
 * Board: ESP32S3 Dev Module (esp32 Arduino Core by Espressif v3.2.0)
 *
 * Required Libraries:
 * - lvgl (v9.2.2)
 * - GFX Library for Arduino (v1.5.6)
 * - Dev Device Pins (v0.0.2)
 * - TAMC_GT911 (v1.0.2)
 * - DHT sensor library by Adafruit (v1.4.4+)
 * - Adafruit Unified Sensor (v1.1.14+)
 */

// ============================================================================
// LIBRARY INCLUDES
// ============================================================================

#include <lvgl.h>            // LVGL graphics library
#include <PINS_JC4827W543.h> // Board pin definitions
#include "TAMC_GT911.h"      // Touch controller library
#include <DHT.h>             // DHT sensor library
#include <Preferences.h>     // ESP32 NVS for settings storage

// ============================================================================
// MODULE INCLUDES (Order matters - config first, then dependencies)
// ============================================================================

#include "src/config.h"          // Configuration and constants
#include "src/sensor_dht11.h"    // DHT11 sensor module
#include "src/ui_menu.h"         // Main menu screen
#include "src/ui_test.h"         // UI test screen
#include "src/ui_temphumid.h"    // Temperature/Humidity screen
#include "src/ui_settings.h"     // Settings screen

// ============================================================================
// TOUCH CONTROLLER CONFIGURATION
// ============================================================================

#define TOUCH_SDA 8
#define TOUCH_SCL 4
#define TOUCH_INT 3
#define TOUCH_RST 38
#define TOUCH_WIDTH 480
#define TOUCH_HEIGHT 272

TAMC_GT911 touchController = TAMC_GT911(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, TOUCH_WIDTH, TOUCH_HEIGHT);

// ============================================================================
// BRIGHTNESS CONTROL CONFIGURATION
// ============================================================================

// PWM configuration for backlight control
#define BACKLIGHT_PWM_CHANNEL 0
#define BACKLIGHT_PWM_FREQ 5000
#define BACKLIGHT_PWM_RESOLUTION 8  // 8-bit resolution (0-255)

// Preferences storage
Preferences preferences;

// Current brightness value (0-100%)
int currentBrightness = 80;  // Default 80%

// ============================================================================
// DISPLAY GLOBAL VARIABLES
// ============================================================================

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_display_t *disp;
lv_color_t *disp_draw_buf;

// ============================================================================
// LVGL CALLBACK FUNCTIONS
// ============================================================================

/**
 * @brief LVGL log callback function
 * @param level Log level
 * @param buf Log message buffer
 */
void my_print(lv_log_level_t level, const char *buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

/**
 * @brief LVGL tick callback function
 * @return Current milliseconds for LVGL timing
 */
uint32_t millis_cb(void) {
  return millis();
}

/**
 * @brief LVGL display flush callback
 * Transfers rendered image to display hardware
 * @param disp Display object
 * @param area Area to update
 * @param px_map Pixel map data
 */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);

  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)px_map, w, h);

  lv_disp_flush_ready(disp);
}

/**
 * @brief LVGL touchpad read callback
 * Reads touch controller and updates LVGL touch data
 * @param indev Input device object
 * @param data Touch data structure to update
 */
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  // Update touch data from GT911 controller
  touchController.read();

  // If touch detected, update LVGL with first point's coordinates
  if (touchController.isTouched && touchController.touches > 0) {
    data->point.x = touchController.points[0].x;
    data->point.y = touchController.points[0].y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// ============================================================================
// BRIGHTNESS CONTROL FUNCTIONS
// ============================================================================

/**
 * @brief Set display brightness
 * @param brightness Brightness level (0-100%)
 */
void setBrightness(int brightness) {
  // Clamp brightness to valid range
  if (brightness < 0) brightness = 0;
  if (brightness > 100) brightness = 100;

  currentBrightness = brightness;

  // Convert 0-100% to 0-255 PWM value
  int pwmValue = map(brightness, 0, 100, 0, 255);

  // Apply PWM to backlight (ESP32 Arduino Core 3.x uses ledcWrite with pin directly)
  ledcWrite(GFX_BL, pwmValue);

  #if DEBUG_SERIAL
    DEBUG_PRINT("Brightness set to: ");
    DEBUG_PRINT(brightness);
    DEBUG_PRINT("% (PWM: ");
    DEBUG_PRINT(pwmValue);
    DEBUG_PRINTLN(")");
  #endif
}

/**
 * @brief Save brightness to preferences
 * @param brightness Brightness level to save (0-100%)
 */
void saveBrightness(int brightness) {
  preferences.begin("settings", false);
  preferences.putInt("brightness", brightness);
  preferences.end();

  #if DEBUG_SERIAL
    DEBUG_PRINT("Brightness saved: ");
    DEBUG_PRINT(brightness);
    DEBUG_PRINTLN("%");
  #endif
}

/**
 * @brief Load brightness from preferences
 * @return Saved brightness level (0-100%), or default if not found
 */
int loadBrightness() {
  preferences.begin("settings", true); // Read-only mode
  int brightness = preferences.getInt("brightness", 80); // Default 80%
  preferences.end();

  // Clamp to valid range
  if (brightness < 0) brightness = 0;
  if (brightness > 100) brightness = 100;

  #if DEBUG_SERIAL
    DEBUG_PRINT("Brightness loaded: ");
    DEBUG_PRINT(brightness);
    DEBUG_PRINTLN("%");
  #endif

  return brightness;
}

/**
 * @brief Get current brightness value
 * @return Current brightness level (0-100%)
 */
int getBrightness() {
  return currentBrightness;
}

// ============================================================================
// ARDUINO SETUP
// ============================================================================

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("\n\n================================================");
  Serial.println("Temperature & Humidity Tracker");
  Serial.println("================================================");
  Serial.println("Board: JC4827W543 (ESP32-S3)");

  String LVGL_Arduino = String("LVGL v") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println(LVGL_Arduino);
  Serial.println("================================================\n");

  // ========================================
  // Initialize Display
  // ========================================
  Serial.println("Initializing display...");
  if (!gfx->begin()) {
    Serial.println("ERROR: Display initialization failed!");
    while (true) {
      delay(1000);
    }
  }

  // Configure PWM for backlight control
  // ESP32 Arduino Core 3.x uses ledcAttach instead of ledcSetup/ledcAttachPin
  ledcAttach(GFX_BL, BACKLIGHT_PWM_FREQ, BACKLIGHT_PWM_RESOLUTION);

  // Load saved brightness and apply it
  currentBrightness = loadBrightness();
  setBrightness(currentBrightness);

  gfx->fillScreen(RGB565_BLACK);
  Serial.println("Display initialized successfully");

  // ========================================
  // Initialize Touch Controller
  // ========================================
  Serial.println("Initializing touch controller...");
  touchController.begin();
  touchController.setRotation(ROTATION_INVERTED);
  Serial.println("Touch controller initialized");

  // ========================================
  // Initialize LVGL
  // ========================================
  Serial.println("Initializing LVGL...");
  lv_init();

  // Set tick source for LVGL timing
  lv_tick_set_cb(millis_cb);

  // Register print function for debugging
  #if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print);
  #endif

  // Configure display buffer
  screenWidth = gfx->width();
  screenHeight = gfx->height();
  bufSize = screenWidth * 40;

  Serial.print("Screen resolution: ");
  Serial.print(screenWidth);
  Serial.print("x");
  Serial.println(screenHeight);
  Serial.print("Buffer size: ");
  Serial.print(bufSize);
  Serial.println(" pixels");

  // Allocate display buffer
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!disp_draw_buf) {
    // Retry without MALLOC_CAP_INTERNAL flag
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_8BIT);
  }

  if (!disp_draw_buf) {
    Serial.println("ERROR: LVGL display buffer allocation failed!");
    while (true) {
      delay(1000);
    }
  }

  // Create LVGL display
  disp = lv_display_create(screenWidth, screenHeight);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, disp_draw_buf, NULL, bufSize * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);

  // Create LVGL input device (touchpad)
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);

  Serial.println("LVGL initialized successfully");

  // ========================================
  // Initialize DHT11 Sensor
  // ========================================
  Serial.println("\nInitializing DHT11 sensor...");
  if (!initDHT11()) {
    Serial.println("WARNING: DHT11 initialization failed!");
    Serial.println("Check wiring: GPIO 1 for data pin");
    Serial.println("Continuing with sensor offline...");
  }

  // ========================================
  // Create Main Menu Screen
  // ========================================
  Serial.println("\nCreating main menu...");
  createMenuScreen();

  Serial.println("\n================================================");
  Serial.println("Setup complete! System ready.");
  Serial.println("================================================\n");

  #if DEBUG_MEMORY
    Serial.print("Free heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
  #endif
}

// ============================================================================
// ARDUINO MAIN LOOP
// ============================================================================

void loop() {
  // Let LVGL handle its tasks (rendering, animations, events)
  lv_task_handler();

  // Update DHT11 sensor readings
  updateDHT11();

  // Update temperature/humidity display if that screen is active
  if (temphumid_screen != NULL) {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate >= 500) {  // Update display every 500ms
      updateTempHumidDisplay();
      lastUpdate = millis();
    }
  }

  // Handle direct mode rendering if needed
  #ifdef DIRECT_MODE
    #if defined(CANVAS) || defined(RGB_PANEL) || defined(DSI_PANEL)
      gfx->flush();
    #else
      gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)disp_draw_buf, screenWidth, screenHeight);
    #endif
  #else
    #ifdef CANVAS
      gfx->flush();
    #endif
  #endif

  // Small delay to prevent watchdog issues
  delay(5);
}
