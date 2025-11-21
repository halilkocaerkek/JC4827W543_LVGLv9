/**
 * @file ui_temphumid.h
 * @brief Temperature and Humidity Display Screen UI Module
 *
 * This module displays real-time temperature and humidity readings from the DHT11 sensor.
 * Features include:
 * - Large temperature and humidity displays
 * - Min/Max value tracking
 * - Temperature unit toggle (°C/°F)
 * - Color-coded comfort indicators
 * - Last update timestamp
 * - Reset min/max button
 * - Back button to return to menu
 *
 * Usage:
 *   Call createTempHumidScreen() to build and display the screen
 *   Call updateTempHumidDisplay() periodically to refresh values
 */

#ifndef UI_TEMPHUMID_H
#define UI_TEMPHUMID_H

#include <lvgl.h>
#include "config.h"
#include "sensor_dht11.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

lv_obj_t *temphumid_screen = NULL;
lv_obj_t *temp_label = NULL;
lv_obj_t *temp_value_label = NULL;
lv_obj_t *humid_label = NULL;
lv_obj_t *humid_value_label = NULL;
lv_obj_t *temp_unit_btn = NULL;
lv_obj_t *minmax_label = NULL;
lv_obj_t *status_label = NULL;
lv_obj_t *timestamp_label = NULL;
lv_obj_t *temp_indicator = NULL;
lv_obj_t *humid_indicator = NULL;

TempUnit currentTempUnit = DEFAULT_TEMP_UNIT;
unsigned long lastDisplayUpdate = 0;

// External screen creation functions
extern void createMenuScreen();

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * @brief Get color based on temperature comfort zone
 * @param temp Temperature in Celsius
 * @return LVGL color for the temperature
 */
lv_color_t getTemperatureColor(float temp) {
  if (temp < TEMP_COLD_THRESHOLD) {
    return lv_color_hex(0x2196F3); // Blue - Cold
  } else if (temp < TEMP_COMFORT_MIN) {
    return lv_color_hex(0x03A9F4); // Light Blue - Cool
  } else if (temp <= TEMP_COMFORT_MAX) {
    return lv_color_hex(0x4CAF50); // Green - Comfortable
  } else if (temp <= TEMP_HOT_THRESHOLD) {
    return lv_color_hex(0xFF9800); // Orange - Warm
  } else {
    return lv_color_hex(0xF44336); // Red - Hot
  }
}

/**
 * @brief Get color based on humidity comfort zone
 * @param humid Humidity in percentage
 * @return LVGL color for the humidity
 */
lv_color_t getHumidityColor(float humid) {
  if (humid < HUMIDITY_LOW_THRESHOLD) {
    return lv_color_hex(0xFFEB3B); // Yellow - Dry
  } else if (humid < HUMIDITY_COMFORT_MIN) {
    return lv_color_hex(0xFFC107); // Amber - Low
  } else if (humid <= HUMIDITY_COMFORT_MAX) {
    return lv_color_hex(0x4CAF50); // Green - Comfortable
  } else if (humid <= HUMIDITY_HIGH_THRESHOLD) {
    return lv_color_hex(0x03A9F4); // Light Blue - High
  } else {
    return lv_color_hex(0x2196F3); // Blue - Very Humid
  }
}

/**
 * @brief Get comfort status text for temperature
 * @param temp Temperature in Celsius
 * @return Status string
 */
const char* getTemperatureStatus(float temp) {
  if (temp < TEMP_COLD_THRESHOLD) return "Cold";
  if (temp < TEMP_COMFORT_MIN) return "Cool";
  if (temp <= TEMP_COMFORT_MAX) return "Comfortable";
  if (temp <= TEMP_HOT_THRESHOLD) return "Warm";
  return "Hot";
}

/**
 * @brief Get comfort status text for humidity
 * @param humid Humidity in percentage
 * @return Status string
 */
const char* getHumidityStatus(float humid) {
  if (humid < HUMIDITY_LOW_THRESHOLD) return "Dry";
  if (humid < HUMIDITY_COMFORT_MIN) return "Low";
  if (humid <= HUMIDITY_COMFORT_MAX) return "Comfortable";
  if (humid <= HUMIDITY_HIGH_THRESHOLD) return "High";
  return "Very Humid";
}

// ============================================================================
// EVENT HANDLERS
// ============================================================================

// Forward declaration
void updateTempHumidDisplay();

/**
 * @brief Event handler for back button
 */
static void temphumid_back_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    #if DEBUG_NAVIGATION
      DEBUG_PRINTLN("[TEMP/HUMID] Navigating back to menu");
    #endif
    createMenuScreen();
  }
}

/**
 * @brief Event handler for temperature unit toggle button
 */
static void temp_unit_toggle_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    // Toggle temperature unit
    currentTempUnit = (currentTempUnit == TEMP_CELSIUS) ? TEMP_FAHRENHEIT : TEMP_CELSIUS;

    // Update button label
    lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    lv_label_set_text(label, (currentTempUnit == TEMP_CELSIUS) ? "°C" : "°F");

    DEBUG_PRINT("[TEMP/HUMID] Temperature unit changed to: ");
    DEBUG_PRINTLN((currentTempUnit == TEMP_CELSIUS) ? "Celsius" : "Fahrenheit");

    // Force immediate display update
    updateTempHumidDisplay();
  }
}

/**
 * @brief Event handler for reset min/max button
 */
static void reset_minmax_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    resetMinMax();
    updateTempHumidDisplay();
    DEBUG_PRINTLN("[TEMP/HUMID] Min/Max values reset");
  }
}

// ============================================================================
// DISPLAY UPDATE
// ============================================================================

/**
 * @brief Update the temperature and humidity display with current sensor values
 * Call this function periodically to refresh the display
 */
void updateTempHumidDisplay() {
  if (temphumid_screen == NULL) return;

  float temp = getCurrentTemperature();
  float humid = getCurrentHumidity();

  // Convert temperature to selected unit
  float displayTemp = getTemperature(currentTempUnit);
  const char* unitSymbol = (currentTempUnit == TEMP_CELSIUS) ? "°C" : "°F";

  // Update temperature value and color
  if (temp_value_label != NULL) {
    lv_label_set_text_fmt(temp_value_label, "%.1f%s", displayTemp, unitSymbol);
    lv_color_t tempColor = getTemperatureColor(temp);
    lv_obj_set_style_text_color(temp_value_label, tempColor, 0);
  }

  // Update humidity value and color
  if (humid_value_label != NULL) {
    lv_label_set_text_fmt(humid_value_label, "%.1f%%", humid);
    lv_color_t humidColor = getHumidityColor(humid);
    lv_obj_set_style_text_color(humid_value_label, humidColor, 0);
  }

  // Update status indicators
  if (temp_indicator != NULL) {
    const char* tempStatus = getTemperatureStatus(temp);
    lv_obj_t *temp_status_label = lv_obj_get_child(temp_indicator, 0);
    lv_label_set_text(temp_status_label, tempStatus);
    lv_obj_set_style_bg_color(temp_indicator, getTemperatureColor(temp), 0);
  }

  if (humid_indicator != NULL) {
    const char* humidStatus = getHumidityStatus(humid);
    lv_obj_t *humid_status_label = lv_obj_get_child(humid_indicator, 0);
    lv_label_set_text(humid_status_label, humidStatus);
    lv_obj_set_style_bg_color(humid_indicator, getHumidityColor(humid), 0);
  }

  // Update min/max display
  if (minmax_label != NULL) {
    float minT = getMinTemperature();
    float maxT = getMaxTemperature();
    float minH = getMinHumidity();
    float maxH = getMaxHumidity();

    // Convert min/max temps to selected unit
    if (currentTempUnit == TEMP_FAHRENHEIT) {
      minT = celsiusToFahrenheit(minT);
      maxT = celsiusToFahrenheit(maxT);
    }

    lv_label_set_text_fmt(minmax_label,
                         "Min: %.1f%s / %.1f%%  |  Max: %.1f%s / %.1f%%",
                         minT, unitSymbol, minH,
                         maxT, unitSymbol, maxH);
  }

  // Update sensor status
  if (status_label != NULL) {
    const char* status = getDHT11Status();
    lv_label_set_text(status_label, status);

    // Color based on status
    if (isDHT11Ready()) {
      lv_obj_set_style_text_color(status_label, COLOR_SECONDARY, 0);
    } else {
      lv_obj_set_style_text_color(status_label, COLOR_DANGER, 0);
    }
  }

  // Update timestamp
  if (timestamp_label != NULL) {
    unsigned long seconds = millis() / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    seconds = seconds % 60;
    minutes = minutes % 60;

    lv_label_set_text_fmt(timestamp_label, "Uptime: %02lu:%02lu:%02lu", hours, minutes, seconds);
  }

  lastDisplayUpdate = millis();
}

// ============================================================================
// SCREEN CREATION
// ============================================================================

/**
 * @brief Create and display the temperature/humidity screen
 */
void createTempHumidScreen() {
  #if DEBUG_NAVIGATION
    DEBUG_PRINTLN("[TEMP/HUMID] Creating temperature/humidity screen");
  #endif

  // Clean up previous screen
  lv_obj_clean(lv_screen_active());

  // Create screen container
  temphumid_screen = lv_obj_create(lv_screen_active());
  lv_obj_set_size(temphumid_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_color(temphumid_screen, COLOR_BACKGROUND, 0);
  lv_obj_set_style_border_width(temphumid_screen, 0, 0);
  lv_obj_set_style_pad_all(temphumid_screen, 0, 0);
  lv_obj_align(temphumid_screen, LV_ALIGN_CENTER, 0, 0);

  // Create title
  lv_obj_t *title = lv_label_create(temphumid_screen);
  lv_label_set_text(title, "Temperature & Humidity");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(title, COLOR_TEXT, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

  // Temperature section (left side)
  lv_obj_t *temp_container = lv_obj_create(temphumid_screen);
  lv_obj_set_size(temp_container, 220, 140);
  lv_obj_set_pos(temp_container, 10, 35);
  lv_obj_set_style_bg_color(temp_container, lv_color_hex(0x303030), 0);
  lv_obj_set_style_border_color(temp_container, COLOR_PRIMARY, 0);
  lv_obj_set_style_border_width(temp_container, 2, 0);
  lv_obj_set_style_radius(temp_container, 10, 0);

  temp_label = lv_label_create(temp_container);
  lv_label_set_text(temp_label, LV_SYMBOL_POWER " Temperature");
  lv_obj_set_style_text_color(temp_label, COLOR_TEXT_SECONDARY, 0);
  lv_obj_align(temp_label, LV_ALIGN_TOP_MID, 0, 5);

  temp_value_label = lv_label_create(temp_container);
  lv_label_set_text(temp_value_label, "--.-°C");
  lv_obj_set_style_text_font(temp_value_label, &lv_font_montserrat_32, 0);
  lv_obj_set_style_text_color(temp_value_label, COLOR_PRIMARY, 0);
  lv_obj_align(temp_value_label, LV_ALIGN_CENTER, 0, 10);

  temp_indicator = lv_obj_create(temp_container);
  lv_obj_set_size(temp_indicator, 180, 25);
  lv_obj_set_style_radius(temp_indicator, 12, 0);
  lv_obj_set_style_bg_color(temp_indicator, COLOR_PRIMARY, 0);
  lv_obj_set_style_border_width(temp_indicator, 0, 0);
  lv_obj_align(temp_indicator, LV_ALIGN_BOTTOM_MID, 0, -5);

  lv_obj_t *temp_status = lv_label_create(temp_indicator);
  lv_label_set_text(temp_status, "Reading...");
  lv_obj_set_style_text_color(temp_status, COLOR_TEXT, 0);
  lv_obj_center(temp_status);

  // Humidity section (right side)
  lv_obj_t *humid_container = lv_obj_create(temphumid_screen);
  lv_obj_set_size(humid_container, 220, 140);
  lv_obj_set_pos(humid_container, 250, 35);
  lv_obj_set_style_bg_color(humid_container, lv_color_hex(0x303030), 0);
  lv_obj_set_style_border_color(humid_container, COLOR_SECONDARY, 0);
  lv_obj_set_style_border_width(humid_container, 2, 0);
  lv_obj_set_style_radius(humid_container, 10, 0);

  humid_label = lv_label_create(humid_container);
  lv_label_set_text(humid_label, LV_SYMBOL_REFRESH " Humidity");
  lv_obj_set_style_text_color(humid_label, COLOR_TEXT_SECONDARY, 0);
  lv_obj_align(humid_label, LV_ALIGN_TOP_MID, 0, 5);

  humid_value_label = lv_label_create(humid_container);
  lv_label_set_text(humid_value_label, "--.-%%");
  lv_obj_set_style_text_font(humid_value_label, &lv_font_montserrat_32, 0);
  lv_obj_set_style_text_color(humid_value_label, COLOR_SECONDARY, 0);
  lv_obj_align(humid_value_label, LV_ALIGN_CENTER, 0, 10);

  humid_indicator = lv_obj_create(humid_container);
  lv_obj_set_size(humid_indicator, 180, 25);
  lv_obj_set_style_radius(humid_indicator, 12, 0);
  lv_obj_set_style_bg_color(humid_indicator, COLOR_SECONDARY, 0);
  lv_obj_set_style_border_width(humid_indicator, 0, 0);
  lv_obj_align(humid_indicator, LV_ALIGN_BOTTOM_MID, 0, -5);

  lv_obj_t *humid_status = lv_label_create(humid_indicator);
  lv_label_set_text(humid_status, "Reading...");
  lv_obj_set_style_text_color(humid_status, COLOR_TEXT, 0);
  lv_obj_center(humid_status);

  // Min/Max display
  minmax_label = lv_label_create(temphumid_screen);
  lv_label_set_text(minmax_label, "Min: --°C / --%%  |  Max: --°C / --%%");
  lv_obj_set_style_text_color(minmax_label, COLOR_TEXT_SECONDARY, 0);
  lv_obj_set_style_text_font(minmax_label, &lv_font_montserrat_14, 0);
  lv_obj_align(minmax_label, LV_ALIGN_TOP_MID, 0, 185);

  // Status and timestamp
  status_label = lv_label_create(temphumid_screen);
  lv_label_set_text(status_label, "Initializing...");
  lv_obj_set_style_text_color(status_label, COLOR_TEXT_SECONDARY, 0);
  lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
  lv_obj_set_pos(status_label, 10, 205);

  timestamp_label = lv_label_create(temphumid_screen);
  lv_label_set_text(timestamp_label, "Uptime: 00:00:00");
  lv_obj_set_style_text_color(timestamp_label, COLOR_TEXT_SECONDARY, 0);
  lv_obj_set_style_text_font(timestamp_label, &lv_font_montserrat_14, 0);
  lv_obj_align(timestamp_label, LV_ALIGN_TOP_RIGHT, -10, 205);

  // Control buttons at bottom
  // Back button (left)
  lv_obj_t *back_btn = lv_button_create(temphumid_screen);
  lv_obj_set_size(back_btn, NAV_BUTTON_WIDTH, NAV_BUTTON_HEIGHT);
  lv_obj_set_pos(back_btn, 10, SCREEN_HEIGHT - NAV_BUTTON_HEIGHT - 10);
  lv_obj_set_style_bg_color(back_btn, COLOR_ACCENT, 0);
  lv_obj_add_event_cb(back_btn, temphumid_back_event_cb, LV_EVENT_ALL, NULL);

  lv_obj_t *back_label = lv_label_create(back_btn);
  lv_label_set_text(back_label, LV_SYMBOL_LEFT " Back");
  lv_obj_center(back_label);

  // Unit toggle button (center-left)
  temp_unit_btn = lv_button_create(temphumid_screen);
  lv_obj_set_size(temp_unit_btn, 60, NAV_BUTTON_HEIGHT);
  lv_obj_set_pos(temp_unit_btn, 120, SCREEN_HEIGHT - NAV_BUTTON_HEIGHT - 10);
  lv_obj_set_style_bg_color(temp_unit_btn, COLOR_PRIMARY, 0);
  lv_obj_add_event_cb(temp_unit_btn, temp_unit_toggle_cb, LV_EVENT_ALL, NULL);

  lv_obj_t *unit_label = lv_label_create(temp_unit_btn);
  lv_label_set_text(unit_label, (currentTempUnit == TEMP_CELSIUS) ? "°C" : "°F");
  lv_obj_center(unit_label);

  // Reset min/max button (center-right)
  lv_obj_t *reset_btn = lv_button_create(temphumid_screen);
  lv_obj_set_size(reset_btn, 120, NAV_BUTTON_HEIGHT);
  lv_obj_set_pos(reset_btn, 190, SCREEN_HEIGHT - NAV_BUTTON_HEIGHT - 10);
  lv_obj_set_style_bg_color(reset_btn, COLOR_SECONDARY, 0);
  lv_obj_add_event_cb(reset_btn, reset_minmax_cb, LV_EVENT_ALL, NULL);

  lv_obj_t *reset_label = lv_label_create(reset_btn);
  lv_label_set_text(reset_label, LV_SYMBOL_REFRESH " Reset");
  lv_obj_center(reset_label);

  // Initial display update
  updateTempHumidDisplay();

  #if DEBUG_NAVIGATION
    DEBUG_PRINTLN("[TEMP/HUMID] Screen created successfully");
  #endif
}

/**
 * @brief Clean up temperature/humidity screen resources
 */
void destroyTempHumidScreen() {
  if (temphumid_screen != NULL) {
    lv_obj_del(temphumid_screen);
    temphumid_screen = NULL;
    temp_label = NULL;
    temp_value_label = NULL;
    humid_label = NULL;
    humid_value_label = NULL;
    temp_unit_btn = NULL;
    minmax_label = NULL;
    status_label = NULL;
    timestamp_label = NULL;
    temp_indicator = NULL;
    humid_indicator = NULL;
  }
}

#endif // UI_TEMPHUMID_H
