/**
 * @file ui_test.h
 * @brief UI Test Screen Module for LVGL Demo Widgets
 *
 * This module provides a test screen that demonstrates LVGL widgets including
 * a button with click counter and an arc widget with dynamic label. This is
 * adapted from the original demo code in JC4827W543_LVGLv9.ino to work as a
 * separate screen with navigation capability.
 *
 * Features:
 * - Button widget with click counter (tracks number of button presses)
 * - Arc widget with rotating percentage label
 * - Back button to return to main menu
 * - Serial output for debugging widget interactions
 *
 * Widget Behavior:
 * - Button: Displays "Button: N" where N increments on each click
 * - Arc: Displays percentage value (0-100%) with label rotating to follow arc position
 * - Back Button: Returns to menu screen via createMenuScreen()
 *
 * Original Code: Lines 70-87 (button) and 177-188 (arc) from JC4827W543_LVGLv9.ino
 *
 * @date 2025-11-21
 */

#ifndef UI_TEST_H
#define UI_TEST_H

#include "config.h"
#include <lvgl.h>

// ============================================================================
// GLOBAL VARIABLES - Screen Objects
// ============================================================================

// Main screen container
lv_obj_t *test_screen = NULL;

// Widget objects
lv_obj_t *test_title_label = NULL;
lv_obj_t *test_button = NULL;
lv_obj_t *test_button_label = NULL;
lv_obj_t *test_arc = NULL;
lv_obj_t *test_arc_label = NULL;
lv_obj_t *test_back_button = NULL;
lv_obj_t *test_back_button_label = NULL;

// ============================================================================
// EXTERNAL FUNCTION DECLARATIONS
// ============================================================================

// Menu screen creation function (defined in ui_menu.h or main file)
extern void createMenuScreen();

// ============================================================================
// EVENT HANDLERS
// ============================================================================

/**
 * @brief Event handler for the back button
 *
 * Returns to the main menu screen when the back button is clicked.
 *
 * @param e Pointer to the LVGL event object
 */
static void back_button_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED)
  {
    Serial.println("Test Screen: Back button clicked - Returning to menu");
    createMenuScreen();
  }
}

/**
 * @brief Event handler for the test button click counter
 *
 * Original code from JC4827W543_LVGLv9.ino lines 70-87.
 * Increments and displays a click counter each time the button is pressed.
 *
 * @param e Pointer to the LVGL event object
 */
static void btn_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED)
  {
    static uint8_t cnt = 0;
    cnt++;

    // Get the first child of the button which is the label and change its text
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "Button: %d", cnt);

    // Print button click count to serial
    Serial.print("Button clicked - Count: ");
    Serial.println(cnt);
  }
}

/**
 * @brief Event handler for arc value changes
 *
 * Original code from JC4827W543_LVGLv9.ino lines 89-104.
 * Updates the percentage label and rotates it to follow the arc position.
 *
 * @param e Pointer to the LVGL event object
 */
static void value_changed_event_cb(lv_event_t *e)
{
  lv_obj_t *arc = lv_event_get_target_obj(e);
  lv_obj_t *label = (lv_obj_t *)lv_event_get_user_data(e);
  int32_t arc_value = lv_arc_get_value(arc);

  lv_label_set_text_fmt(label, "%" LV_PRId32 "%%", arc_value);

  // Rotate the label to the current position of the arc
  lv_arc_rotate_obj_to_angle(arc, label, 25);

  // Print arc value to serial
  Serial.print("Arc value changed: ");
  Serial.print(arc_value);
  Serial.println("%");
}

// ============================================================================
// SCREEN CREATION FUNCTION
// ============================================================================

/**
 * @brief Creates and displays the UI Test screen
 *
 * This function creates a test screen with demo widgets including a button
 * with click counter and an arc widget. The widgets are adapted from the
 * original demo code to work within a modular screen system.
 *
 * Screen Layout:
 * - Title at top: "UI Test Screen"
 * - Button with counter at top-left (original position: 10, 40)
 * - Arc widget centered in screen
 * - Back button at bottom center
 *
 * Colors used:
 * - Title: COLOR_TEXT (white)
 * - Button: COLOR_PRIMARY (blue)
 * - Arc: Default LVGL styling
 * - Back button: COLOR_ACCENT (orange)
 * - Background: COLOR_BACKGROUND (dark gray)
 */
void createTestScreen()
{
  Serial.println("Creating Test Screen...");

  // Clean up the previous screen
  lv_obj_clean(lv_screen_active());

  // Create the test screen container
  test_screen = lv_screen_active();
  lv_obj_set_style_bg_color(test_screen, COLOR_BACKGROUND, 0);

  // -------------------------------------------------------------------------
  // Title Label
  // -------------------------------------------------------------------------
  test_title_label = lv_label_create(test_screen);
  lv_label_set_text(test_title_label, "UI Test Screen");
  lv_obj_set_style_text_color(test_title_label, COLOR_TEXT, 0);
  lv_obj_set_style_text_font(test_title_label, &lv_font_montserrat_20, 0);
  lv_obj_align(test_title_label, LV_ALIGN_TOP_MID, 0, 10);

  // -------------------------------------------------------------------------
  // Button Widget (Original code: lines 177-184)
  // -------------------------------------------------------------------------
  test_button = lv_button_create(test_screen);         // Add a button to the screen
  lv_obj_set_pos(test_button, 10, 40);                 // Set its position (adjusted for title)
  lv_obj_set_size(test_button, 120, 50);               // Set its size
  lv_obj_set_style_bg_color(test_button, COLOR_PRIMARY, 0); // Blue button
  lv_obj_add_event_cb(test_button, btn_event_cb, LV_EVENT_ALL, NULL); // Assign callback

  test_button_label = lv_label_create(test_button);    // Add a label to the button
  lv_label_set_text(test_button_label, "Button");      // Set the label's text
  lv_obj_center(test_button_label);

  // -------------------------------------------------------------------------
  // Arc Widget (Original code: lines 187-198)
  // -------------------------------------------------------------------------
  test_arc_label = lv_label_create(test_screen);

  test_arc = lv_arc_create(test_screen);
  lv_obj_set_size(test_arc, 150, 150);
  lv_arc_set_rotation(test_arc, 135);
  lv_arc_set_bg_angles(test_arc, 0, 270);
  lv_arc_set_value(test_arc, 10);
  lv_obj_center(test_arc);
  lv_obj_add_event_cb(test_arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, test_arc_label);

  // Manually update the label for the first time
  lv_obj_send_event(test_arc, LV_EVENT_VALUE_CHANGED, NULL);

  // -------------------------------------------------------------------------
  // Back Button
  // -------------------------------------------------------------------------
  test_back_button = lv_button_create(test_screen);
  lv_obj_set_size(test_back_button, NAV_BUTTON_WIDTH, NAV_BUTTON_HEIGHT);
  lv_obj_set_style_bg_color(test_back_button, COLOR_ACCENT, 0); // Orange button
  lv_obj_align(test_back_button, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_obj_add_event_cb(test_back_button, back_button_event_cb, LV_EVENT_ALL, NULL);

  test_back_button_label = lv_label_create(test_back_button);
  lv_label_set_text(test_back_button_label, "Back");
  lv_obj_set_style_text_color(test_back_button_label, COLOR_TEXT, 0);
  lv_obj_center(test_back_button_label);

  Serial.println("Test Screen created successfully");
}

// ============================================================================
// SCREEN CLEANUP FUNCTION
// ============================================================================

/**
 * @brief Destroys the test screen and cleans up resources
 *
 * This function should be called when switching to another screen to
 * properly release memory and resources. Note that lv_obj_clean() in
 * createTestScreen() handles most cleanup automatically.
 */
void destroyTestScreen()
{
  Serial.println("Destroying Test Screen...");

  // Clean up is handled by lv_obj_clean() in createTestScreen()
  // This function is provided for explicit cleanup if needed

  // Null out pointers for safety
  test_screen = NULL;
  test_title_label = NULL;
  test_button = NULL;
  test_button_label = NULL;
  test_arc = NULL;
  test_arc_label = NULL;
  test_back_button = NULL;
  test_back_button_label = NULL;

  Serial.println("Test Screen destroyed");
}

#endif // UI_TEST_H
