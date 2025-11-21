/**
 * @file ui_menu.h
 * @brief Main Menu Screen UI Module
 *
 * This module creates and manages the main menu screen with three options:
 * 1. UI Test - Test screen for LVGL widgets
 * 2. Temperature & Humidity - Sensor data display
 * 3. Settings - Configuration options
 *
 * Usage:
 *   Call createMenuScreen() to build the menu UI
 *   Menu buttons automatically handle screen transitions
 */

#ifndef UI_MENU_H
#define UI_MENU_H

#include <lvgl.h>
#include "config.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

lv_obj_t *menu_screen = NULL;
lv_obj_t *menu_btn1 = NULL;
lv_obj_t *menu_btn2 = NULL;
lv_obj_t *menu_btn3 = NULL;

// External screen creation functions (defined in other modules)
extern void createTestScreen();
extern void createTempHumidScreen();
extern void createSettingsScreen();

// ============================================================================
// EVENT HANDLERS
// ============================================================================

/**
 * @brief Event handler for UI Test menu button
 */
static void menu_btn1_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    #if DEBUG_NAVIGATION
      DEBUG_PRINTLN("[MENU] Navigating to UI Test screen");
    #endif
    createTestScreen();
  }
}

/**
 * @brief Event handler for Temperature/Humidity menu button
 */
static void menu_btn2_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    #if DEBUG_NAVIGATION
      DEBUG_PRINTLN("[MENU] Navigating to Temperature/Humidity screen");
    #endif
    createTempHumidScreen();
  }
}

/**
 * @brief Event handler for Settings menu button
 */
static void menu_btn3_event_cb(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    #if DEBUG_NAVIGATION
      DEBUG_PRINTLN("[MENU] Navigating to Settings screen");
    #endif
    createSettingsScreen();
  }
}

// ============================================================================
// SCREEN CREATION
// ============================================================================

/**
 * @brief Create menu button with icon and label
 * @param parent Parent object
 * @param x X position
 * @param y Y position
 * @param icon_symbol LV_SYMBOL for the button
 * @param label_text Button label text
 * @param event_cb Event callback function
 * @return Created button object
 */
lv_obj_t* createMenuButton(lv_obj_t *parent, lv_coord_t x, lv_coord_t y,
                           const char *icon_symbol, const char *label_text,
                           lv_event_cb_t event_cb) {
  // Create button
  lv_obj_t *btn = lv_button_create(parent);
  lv_obj_set_size(btn, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
  lv_obj_set_pos(btn, x, y);
  lv_obj_set_style_radius(btn, 15, 0);
  lv_obj_set_style_bg_color(btn, COLOR_PRIMARY, 0);
  lv_obj_set_style_shadow_width(btn, 10, 0);
  lv_obj_set_style_shadow_opa(btn, LV_OPA_30, 0);
  lv_obj_add_event_cb(btn, event_cb, LV_EVENT_ALL, NULL);

  // Create flex container inside button for vertical layout
  lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  // Create icon label
  lv_obj_t *icon = lv_label_create(btn);
  lv_label_set_text(icon, icon_symbol);
  lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(icon, COLOR_TEXT, 0);

  // Create text label
  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, label_text);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
  lv_obj_set_style_text_color(label, COLOR_TEXT, 0);
  lv_obj_set_style_pad_top(label, 10, 0);

  return btn;
}

/**
 * @brief Create and display the main menu screen
 */
void createMenuScreen() {
  #if DEBUG_NAVIGATION
    DEBUG_PRINTLN("[MENU] Creating menu screen");
  #endif

  // Clean up previous screen if it exists
  if (menu_screen != NULL) {
    lv_obj_clean(lv_screen_active());
  }

  // Create new screen
  menu_screen = lv_obj_create(lv_screen_active());
  lv_obj_set_size(menu_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_style_bg_color(menu_screen, COLOR_BACKGROUND, 0);
  lv_obj_set_style_border_width(menu_screen, 0, 0);
  lv_obj_set_style_pad_all(menu_screen, 0, 0);
  lv_obj_align(menu_screen, LV_ALIGN_CENTER, 0, 0);

  // Create title
  lv_obj_t *title = lv_label_create(menu_screen);
  lv_label_set_text(title, "Temperature & Humidity Tracker");
  lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(title, COLOR_TEXT, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 15);

  // Calculate button positions for centered layout
  int totalWidth = (MENU_BUTTON_WIDTH * 3) + (MENU_BUTTON_SPACING * 2);
  int startX = (SCREEN_WIDTH - totalWidth) / 2;
  int buttonY = 60;

  // Create menu button 1 - UI Test
  menu_btn1 = createMenuButton(menu_screen,
                                startX,
                                buttonY,
                                LV_SYMBOL_SETTINGS,
                                "UI Test",
                                menu_btn1_event_cb);

  // Create menu button 2 - Temperature & Humidity
  menu_btn2 = createMenuButton(menu_screen,
                                startX + MENU_BUTTON_WIDTH + MENU_BUTTON_SPACING,
                                buttonY,
                                LV_SYMBOL_HOME,
                                "Temp &\nHumidity",
                                menu_btn2_event_cb);

  // Apply different color to highlight main feature
  lv_obj_set_style_bg_color(menu_btn2, COLOR_SECONDARY, 0);

  // Create menu button 3 - Settings
  menu_btn3 = createMenuButton(menu_screen,
                                startX + (MENU_BUTTON_WIDTH + MENU_BUTTON_SPACING) * 2,
                                buttonY,
                                LV_SYMBOL_EDIT,
                                "Settings",
                                menu_btn3_event_cb);

  // Create footer with info
  lv_obj_t *footer = lv_label_create(menu_screen);
  lv_label_set_text(footer, "ESP32-S3 | LVGL v" GFX_STR(LVGL_VERSION_MAJOR) "." GFX_STR(LVGL_VERSION_MINOR));
  lv_obj_set_style_text_color(footer, COLOR_TEXT_SECONDARY, 0);
  lv_obj_set_style_text_font(footer, &lv_font_montserrat_12, 0);
  lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, -10);

  #if DEBUG_NAVIGATION
    DEBUG_PRINTLN("[MENU] Menu screen created successfully");
  #endif
}

/**
 * @brief Clean up menu screen resources
 */
void destroyMenuScreen() {
  if (menu_screen != NULL) {
    lv_obj_del(menu_screen);
    menu_screen = NULL;
    menu_btn1 = NULL;
    menu_btn2 = NULL;
    menu_btn3 = NULL;
  }
}

#endif // UI_MENU_H
