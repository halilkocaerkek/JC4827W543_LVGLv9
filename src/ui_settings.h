/**
 * @file ui_settings.h
 * @brief Settings Screen UI Module
 *
 * This module creates and manages the settings screen. Currently displays
 * placeholder text with planned features. Future versions will include:
 * - Temperature unit selection (Celsius/Fahrenheit)
 * - Update interval configuration
 * - WiFi network configuration
 * - Data logging options
 *
 * Usage:
 *   Call createSettingsScreen() to display the settings UI
 *   Back button returns to the main menu
 */

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <lvgl.h>
#include "config.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

lv_obj_t *settings_screen = NULL;
lv_obj_t *settings_back_btn = NULL;

// External screen creation functions (defined in other modules)
extern void createMenuScreen();

// ============================================================================
// EVENT HANDLERS
// ============================================================================

/**
 * @brief Event handler for back button - returns to menu
 */
static void settings_back_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    #if DEBUG_NAVIGATION
      DEBUG_PRINTLN("[SETTINGS] Back button clicked, returning to menu");
    #endif
    createMenuScreen();
  }
}

// ============================================================================
// SCREEN CREATION
// ============================================================================

/**
 * @brief Create and display the settings screen
 */
void createSettingsScreen() {
  #if DEBUG_NAVIGATION
    DEBUG_PRINTLN("[SETTINGS] Creating settings screen");
  #endif

  // Clean up previous screen if it exists
  if (settings_screen != NULL) {
    lv_obj_clean(lv_screen_active());
  }

  // Create new screen
  settings_screen = lv_obj_create(lv_screen_active());
  lv_obj_set_size(settings_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_color(settings_screen, COLOR_BACKGROUND, 0);
  lv_obj_set_style_border_width(settings_screen, 0, 0);
  lv_obj_set_style_pad_all(settings_screen, 0, 0);
  lv_obj_align(settings_screen, LV_ALIGN_CENTER, 0, 0);

  // Create title
  lv_obj_t *title = lv_label_create(settings_screen);
  lv_label_set_text(title, "Settings");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_22, 0);
  lv_obj_set_style_text_color(title, COLOR_TEXT, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

  // Create main message label
  lv_obj_t *message_label = lv_label_create(settings_screen);
  lv_label_set_text(message_label, "Settings coming soon...");
  lv_obj_set_style_text_font(message_label, &lv_font_montserrat_18, 0);
  lv_obj_set_style_text_color(message_label, COLOR_ACCENT, 0);
  lv_obj_align(message_label, LV_ALIGN_CENTER, 0, -30);

  // Create feature list container
  lv_obj_t *feature_container = lv_obj_create(settings_screen);
  lv_obj_set_size(feature_container, 320, 140);
  lv_obj_set_style_bg_color(feature_container, lv_color_hex(0x404040), 0);
  lv_obj_set_style_border_width(feature_container, 2, 0);
  lv_obj_set_style_border_color(feature_container, COLOR_PRIMARY, 0);
  lv_obj_set_style_radius(feature_container, 10, 0);
  lv_obj_set_style_pad_all(feature_container, 15, 0);
  lv_obj_align(feature_container, LV_ALIGN_CENTER, 0, 40);

  // Create features label
  lv_obj_t *features_label = lv_label_create(feature_container);
  lv_label_set_text(features_label,
    "Planned Features:\n\n"
    LV_SYMBOL_RIGHT " Temperature unit (\xC2\xB0C/\xC2\xB0""F)\n"
    LV_SYMBOL_RIGHT " Update interval\n"
    LV_SYMBOL_RIGHT " WiFi configuration\n"
    LV_SYMBOL_RIGHT " Data logging");
  lv_obj_set_style_text_font(features_label, &lv_font_montserrat_14, 0);
  lv_obj_set_style_text_color(features_label, COLOR_TEXT_SECONDARY, 0);
  lv_obj_set_style_text_line_space(features_label, 4, 0);
  lv_obj_align(features_label, LV_ALIGN_TOP_LEFT, 0, 0);

  // Create back button
  settings_back_btn = lv_button_create(settings_screen);
  lv_obj_set_size(settings_back_btn, NAV_BUTTON_WIDTH, NAV_BUTTON_HEIGHT);
  lv_obj_align(settings_back_btn, LV_ALIGN_BOTTOM_LEFT, 15, -15);
  lv_obj_set_style_radius(settings_back_btn, 8, 0);
  lv_obj_set_style_bg_color(settings_back_btn, COLOR_PRIMARY, 0);
  lv_obj_add_event_cb(settings_back_btn, settings_back_event_cb, LV_EVENT_ALL, NULL);

  // Create back button label
  lv_obj_t *back_label = lv_label_create(settings_back_btn);
  lv_label_set_text(back_label, LV_SYMBOL_LEFT " Back");
  lv_obj_set_style_text_color(back_label, COLOR_TEXT, 0);
  lv_obj_center(back_label);

  #if DEBUG_NAVIGATION
    DEBUG_PRINTLN("[SETTINGS] Settings screen created successfully");
  #endif
}

/**
 * @brief Clean up settings screen resources
 */
void destroySettingsScreen() {
  if (settings_screen != NULL) {
    lv_obj_del(settings_screen);
    settings_screen = NULL;
    settings_back_btn = NULL;
  }
}

#endif // UI_SETTINGS_H
