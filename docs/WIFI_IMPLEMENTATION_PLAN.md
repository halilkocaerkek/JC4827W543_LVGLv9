# WiFi Settings Implementation Plan

## Overview
This document outlines the complete plan for adding WiFi configuration functionality to the Temperature & Humidity Tracker application. The implementation will allow users to configure WiFi networks through the touchscreen interface, with credentials stored persistently in the ESP32.

## Project Goal
Add a fully functional WiFi configuration screen under the Settings menu that enables users to:
1. Scan and view available WiFi networks
2. Select a network from a list
3. Enter the password using an on-screen keyboard
4. Toggle password visibility (show/hide)
5. Store credentials in ESP32 non-volatile storage
6. Auto-connect to saved WiFi on startup

## Research Summary

### Available Resources Found

#### 1. WiFi Demo Examples (docs/JC4827W543/1-Demo/Demo_Arduino/)
The factory documentation includes several WiFi implementation examples:

**4_1_Wifi_AP** - Access Point Mode
```cpp
WiFi.softAP("ESP_AP", "12345678");
```

**4_2_Wifi_STA** - Station Mode (Basic Connection)
```cpp
WiFi.begin("SSID", "password");
WiFi.setAutoReconnect(true);
```

**4_3_Wifi_SmartConfig** - Auto-Configuration Pattern
```cpp
// Auto-reconnect logic
bool autoConfig() {
  WiFi.begin();
  for (size_t i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
    delay(1000);
  }
  return false;
}
```

#### 2. LVGL Keyboard Implementation
Found in `docs/JC4827W543/1-Demo/Demo_Arduino/3_3-3_TFT-LVGL-Widgets/LvglWidgets/lv_demo_widgets.c`:

**Keyboard Creation (Line 251):**
```c
lv_obj_t * kb = lv_keyboard_create(lv_scr_act());
lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
```

**Text Area with Keyboard (Lines 266-269):**
```c
lv_obj_t * user_name = lv_textarea_create(panel2);
lv_textarea_set_one_line(user_name, true);
lv_textarea_set_placeholder_text(user_name, "Your name");
lv_obj_add_event_cb(user_name, ta_event_cb, LV_EVENT_ALL, kb);
```

**Keyboard Event Handler Pattern (Lines 1202-1230):**
```c
static void ta_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    else if(code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}
```

## Architecture Design

### Module Structure
Following the existing modular architecture pattern:

```
src/
├── wifi_manager.h          # WiFi management module (NEW)
├── ui_wifi.h               # WiFi settings screen (NEW)
├── ui_settings.h           # Modified to launch WiFi screen
├── config.h                # Add WiFi configuration constants
└── ... (existing modules)
```

### Module 1: WiFi Manager (src/wifi_manager.h)

**Purpose:** Handle all WiFi operations and credential management

**Responsibilities:**
- WiFi network scanning
- Connection management
- Credential storage/retrieval (ESP32 Preferences)
- Auto-reconnect logic
- Status reporting

**Key Functions:**
```cpp
// Initialization and storage
bool initWiFiManager();                    // Initialize WiFi and load saved credentials
void saveWiFiCredentials(String ssid, String password);
bool loadWiFiCredentials(String &ssid, String &password);
void clearWiFiCredentials();

// Network scanning
int scanWiFiNetworks();                    // Returns number of networks found
String getScannedSSID(int index);         // Get SSID at index
int8_t getScannedRSSI(int index);         // Get signal strength at index
bool isScannedNetworkSecure(int index);   // Check if network is encrypted

// Connection management
bool connectToWiFi(const char* ssid, const char* password);
void disconnectWiFi();
bool isWiFiConnected();
String getWiFiStatus();                    // "Connected", "Connecting", "Disconnected", "Failed"
IPAddress getLocalIP();
int8_t getSignalStrength();                // Current connection RSSI

// Auto-connect
bool autoConnectWiFi();                    // Try to connect using saved credentials
```

**Implementation Details:**

**Credential Storage (ESP32 Preferences):**
```cpp
#include <Preferences.h>

Preferences preferences;

void saveWiFiCredentials(String ssid, String password) {
    preferences.begin("wifi", false);      // Read/Write mode
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.putBool("configured", true);
    preferences.end();
}

bool loadWiFiCredentials(String &ssid, String &password) {
    preferences.begin("wifi", true);       // Read-only mode
    bool configured = preferences.getBool("configured", false);
    if (configured) {
        ssid = preferences.getString("ssid", "");
        password = preferences.getString("password", "");
    }
    preferences.end();
    return configured;
}
```

**WiFi Scanning:**
```cpp
int scanWiFiNetworks() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    int n = WiFi.scanNetworks();
    return n;
}

String getScannedSSID(int index) {
    return WiFi.SSID(index);
}

int8_t getScannedRSSI(int index) {
    return WiFi.RSSI(index);
}

bool isScannedNetworkSecure(int index) {
    return WiFi.encryptionType(index) != WIFI_AUTH_OPEN;
}
```

**Connection Management:**
```cpp
bool connectToWiFi(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // Wait up to 10 seconds for connection
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        attempts++;
    }

    return WiFi.status() == WL_CONNECTED;
}

bool autoConnectWiFi() {
    String ssid, password;
    if (loadWiFiCredentials(ssid, password)) {
        return connectToWiFi(ssid.c_str(), password.c_str());
    }
    return false;
}
```

**Signal Strength Helper:**
```cpp
// Convert RSSI to percentage (0-100%)
int rssiToPercent(int8_t rssi) {
    if (rssi >= -50) return 100;
    if (rssi <= -100) return 0;
    return 2 * (rssi + 100);
}

// Get signal quality description
const char* getSignalQuality(int8_t rssi) {
    if (rssi >= -50) return "Excellent";
    if (rssi >= -60) return "Good";
    if (rssi >= -70) return "Fair";
    if (rssi >= -80) return "Weak";
    return "Very Weak";
}
```

### Module 2: WiFi Settings Screen (src/ui_wifi.h)

**Purpose:** User interface for WiFi configuration

**Layout Design:**
```
┌───────────────────────────────────────┐
│         WiFi Configuration            │
│                                       │
│  ┌─────────────────────────────┐     │
│  │ Available Networks:         │     │
│  │ ┌─────────────────────────┐ │     │
│  │ │ [≡≡≡] Home-Network      │ │     │
│  │ │ [≡≡=] Office-WiFi  🔒   │ │     │
│  │ │ [≡==] Guest-Network     │ │     │
│  │ │ [≡==] Neighbor-WiFi 🔒  │ │     │
│  │ └─────────────────────────┘ │     │
│  └─────────────────────────────┘     │
│                                       │
│  Selected: Home-Network               │
│                                       │
│  Password: [●●●●●●●●]  [👁]          │
│                                       │
│  Status: Not Connected                │
│  IP: ---                              │
│                                       │
│  [Back] [Scan] [Connect] [Forget]    │
└───────────────────────────────────────┘
```

**Key Widgets:**
```cpp
lv_obj_t *wifi_screen;              // Main screen container
lv_obj_t *network_list;             // Scrollable list of networks
lv_obj_t *selected_label;           // Shows selected network
lv_obj_t *password_textarea;        // Password input field
lv_obj_t *show_password_btn;        // Toggle password visibility
lv_obj_t *keyboard;                 // On-screen keyboard (hidden by default)
lv_obj_t *status_label;             // Connection status
lv_obj_t *ip_label;                 // IP address display
lv_obj_t *scan_btn;                 // Rescan networks button
lv_obj_t *connect_btn;              // Connect button
lv_obj_t *forget_btn;               // Forget saved credentials
lv_obj_t *back_btn;                 // Return to settings
```

**Key Functions:**
```cpp
void createWiFiScreen();            // Build the UI
void destroyWiFiScreen();           // Cleanup
void updateNetworkList();           // Populate list with scan results
void updateWiFiStatus();            // Refresh status display
```

**Event Handlers:**
```cpp
// Network list selection
static void network_list_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        uint32_t index = lv_obj_get_index(lv_event_get_target(e));
        String ssid = getScannedSSID(index);
        lv_label_set_text_fmt(selected_label, "Selected: %s", ssid.c_str());

        // If network is saved, auto-fill password
        String saved_ssid, saved_password;
        if (loadWiFiCredentials(saved_ssid, saved_password)) {
            if (saved_ssid == ssid) {
                lv_textarea_set_text(password_textarea, saved_password.c_str());
            }
        }
    }
}

// Password text area focus (show/hide keyboard)
static void password_textarea_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    lv_obj_t *kb = (lv_obj_t *)lv_event_get_user_data(e);

    if (code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
        // Move screen content up to make room for keyboard
        lv_obj_set_y(wifi_screen, -100);
    }
    else if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_y(wifi_screen, 0);
    }
}

// Show/hide password toggle
static void show_password_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        static bool password_visible = false;
        password_visible = !password_visible;

        if (password_visible) {
            lv_textarea_set_password_mode(password_textarea, false);
            lv_label_set_text(lv_obj_get_child(show_password_btn, 0), LV_SYMBOL_EYE_CLOSE);
        } else {
            lv_textarea_set_password_mode(password_textarea, true);
            lv_label_set_text(lv_obj_get_child(show_password_btn, 0), LV_SYMBOL_EYE_OPEN);
        }
    }
}

// Scan button
static void scan_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_label_set_text(status_label, "Scanning...");
        lv_task_handler();  // Force UI update

        int n = scanWiFiNetworks();
        updateNetworkList();

        lv_label_set_text_fmt(status_label, "Found %d networks", n);
    }
}

// Connect button
static void connect_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        const char *ssid = lv_label_get_text(selected_label);
        const char *password = lv_textarea_get_text(password_textarea);

        if (strlen(ssid) > 0) {
            lv_label_set_text(status_label, "Connecting...");
            lv_task_handler();  // Force UI update

            bool success = connectToWiFi(ssid, password);

            if (success) {
                saveWiFiCredentials(String(ssid), String(password));
                lv_label_set_text(status_label, "Connected!");
                lv_label_set_text_fmt(ip_label, "IP: %s", WiFi.localIP().toString().c_str());
            } else {
                lv_label_set_text(status_label, "Connection failed!");
            }

            updateWiFiStatus();
        }
    }
}

// Forget button
static void forget_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        clearWiFiCredentials();
        disconnectWiFi();
        lv_label_set_text(status_label, "Credentials cleared");
        lv_textarea_set_text(password_textarea, "");
        updateWiFiStatus();
    }
}
```

**Network List Creation:**
```cpp
void updateNetworkList() {
    // Clear existing list items
    lv_obj_clean(network_list);

    int n = getNetworkCount();

    for (int i = 0; i < n; i++) {
        // Create list item button
        lv_obj_t *btn = lv_button_create(network_list);
        lv_obj_set_width(btn, lv_pct(95));
        lv_obj_set_height(btn, 50);
        lv_obj_add_event_cb(btn, network_list_event_cb, LV_EVENT_CLICKED, NULL);

        // Create horizontal container for icon + text
        lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Signal strength icon
        lv_obj_t *signal_icon = lv_label_create(btn);
        int8_t rssi = getScannedRSSI(i);
        if (rssi >= -60) {
            lv_label_set_text(signal_icon, LV_SYMBOL_WIFI);
        } else if (rssi >= -70) {
            lv_label_set_text(signal_icon, "▂▄");
        } else {
            lv_label_set_text(signal_icon, "▂");
        }

        // SSID label
        lv_obj_t *ssid_label = lv_label_create(btn);
        lv_label_set_text(ssid_label, getScannedSSID(i).c_str());
        lv_obj_set_style_pad_left(ssid_label, 10, 0);

        // Lock icon if secured
        if (isScannedNetworkSecure(i)) {
            lv_obj_t *lock_icon = lv_label_create(btn);
            lv_label_set_text(lock_icon, LV_SYMBOL_LOCK);
        }
    }
}
```

**Screen Creation:**
```cpp
void createWiFiScreen() {
    // Clean up previous screen
    lv_obj_clean(lv_screen_active());

    // Create screen container
    wifi_screen = lv_obj_create(lv_screen_active());
    lv_obj_set_size(wifi_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(wifi_screen, COLOR_BACKGROUND, 0);
    lv_obj_set_style_border_width(wifi_screen, 0, 0);

    // Title
    lv_obj_t *title = lv_label_create(wifi_screen);
    lv_label_set_text(title, "WiFi Configuration");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    // Network list (scrollable)
    network_list = lv_obj_create(wifi_screen);
    lv_obj_set_size(network_list, SCREEN_WIDTH - 20, 100);
    lv_obj_set_pos(network_list, 10, 30);
    lv_obj_set_flex_flow(network_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(network_list, LV_DIR_VER);

    // Selected network label
    selected_label = lv_label_create(wifi_screen);
    lv_label_set_text(selected_label, "Selected: None");
    lv_obj_set_pos(selected_label, 10, 140);

    // Password input
    lv_obj_t *pwd_label = lv_label_create(wifi_screen);
    lv_label_set_text(pwd_label, "Password:");
    lv_obj_set_pos(pwd_label, 10, 160);

    password_textarea = lv_textarea_create(wifi_screen);
    lv_obj_set_size(password_textarea, 320, 40);
    lv_obj_set_pos(password_textarea, 10, 180);
    lv_textarea_set_one_line(password_textarea, true);
    lv_textarea_set_password_mode(password_textarea, true);
    lv_textarea_set_placeholder_text(password_textarea, "Enter password");

    // Show/hide password button
    show_password_btn = lv_button_create(wifi_screen);
    lv_obj_set_size(show_password_btn, 50, 40);
    lv_obj_set_pos(show_password_btn, 340, 180);
    lv_obj_add_event_cb(show_password_btn, show_password_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *eye_icon = lv_label_create(show_password_btn);
    lv_label_set_text(eye_icon, LV_SYMBOL_EYE_OPEN);
    lv_obj_center(eye_icon);

    // Create keyboard (hidden)
    keyboard = lv_keyboard_create(lv_screen_active());
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(password_textarea, password_textarea_event_cb, LV_EVENT_ALL, keyboard);

    // Status labels
    status_label = lv_label_create(wifi_screen);
    lv_label_set_text(status_label, "Status: Not connected");
    lv_obj_set_pos(status_label, 10, 225);

    ip_label = lv_label_create(wifi_screen);
    lv_label_set_text(ip_label, "IP: ---");
    lv_obj_set_pos(ip_label, 10, 245);

    // Bottom buttons
    int btn_y = SCREEN_HEIGHT - 50;

    back_btn = lv_button_create(wifi_screen);
    lv_obj_set_size(back_btn, 80, 40);
    lv_obj_set_pos(back_btn, 10, btn_y);
    lv_obj_add_event_cb(back_btn, back_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT " Back");
    lv_obj_center(back_label);

    scan_btn = lv_button_create(wifi_screen);
    lv_obj_set_size(scan_btn, 80, 40);
    lv_obj_set_pos(scan_btn, 100, btn_y);
    lv_obj_add_event_cb(scan_btn, scan_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *scan_label = lv_label_create(scan_btn);
    lv_label_set_text(scan_label, "Scan");
    lv_obj_center(scan_label);

    connect_btn = lv_button_create(wifi_screen);
    lv_obj_set_size(connect_btn, 90, 40);
    lv_obj_set_pos(connect_btn, 190, btn_y);
    lv_obj_set_style_bg_color(connect_btn, COLOR_SECONDARY, 0);
    lv_obj_add_event_cb(connect_btn, connect_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *connect_label = lv_label_create(connect_btn);
    lv_label_set_text(connect_label, "Connect");
    lv_obj_center(connect_label);

    forget_btn = lv_button_create(wifi_screen);
    lv_obj_set_size(forget_btn, 90, 40);
    lv_obj_set_pos(forget_btn, 290, btn_y);
    lv_obj_set_style_bg_color(forget_btn, COLOR_DANGER, 0);
    lv_obj_add_event_cb(forget_btn, forget_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *forget_label = lv_label_create(forget_btn);
    lv_label_set_text(forget_label, "Forget");
    lv_obj_center(forget_label);

    // Initial scan
    lv_label_set_text(status_label, "Scanning...");
    lv_task_handler();
    int n = scanWiFiNetworks();
    updateNetworkList();
    lv_label_set_text_fmt(status_label, "Found %d networks", n);

    // Check if already connected
    updateWiFiStatus();
}
```

### Module 3: Configuration Updates (src/config.h)

**Add WiFi-related constants:**
```cpp
// ============================================================================
// WIFI CONFIGURATION
// ============================================================================

#define WIFI_CONNECT_TIMEOUT 10000      // Connection timeout in milliseconds
#define WIFI_SCAN_TIMEOUT 5000          // Scan timeout in milliseconds
#define WIFI_MAX_NETWORKS 20            // Maximum networks to display
#define WIFI_RECONNECT_INTERVAL 30000   // Auto-reconnect attempt interval

// WiFi status refresh interval
#define WIFI_STATUS_UPDATE_INTERVAL 2000

// Debug flags
#define DEBUG_WIFI true                 // Print WiFi operations to serial

// Helper macro for WiFi debug
#if DEBUG_WIFI
  #define WIFI_DEBUG_PRINT(x) Serial.print("[WIFI] "); Serial.print(x)
  #define WIFI_DEBUG_PRINTLN(x) Serial.print("[WIFI] "); Serial.println(x)
#else
  #define WIFI_DEBUG_PRINT(x)
  #define WIFI_DEBUG_PRINTLN(x)
#endif
```

### Module 4: Settings Screen Update (src/ui_settings.h)

**Replace placeholder with WiFi launcher:**
```cpp
// Add extern declaration
extern void createWiFiScreen();

// Replace content of createSettingsScreen():
void createSettingsScreen() {
    // ... existing screen setup ...

    // WiFi Configuration button
    lv_obj_t *wifi_btn = lv_button_create(settings_screen);
    lv_obj_set_size(wifi_btn, SCREEN_WIDTH - 40, 60);
    lv_obj_set_pos(wifi_btn, 20, 60);
    lv_obj_set_style_bg_color(wifi_btn, COLOR_PRIMARY, 0);
    lv_obj_add_event_cb(wifi_btn, wifi_btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *wifi_label = lv_label_create(wifi_btn);
    lv_label_set_text(wifi_label, LV_SYMBOL_WIFI " WiFi Configuration");
    lv_obj_set_style_text_font(wifi_label, &lv_font_montserrat_18, 0);
    lv_obj_center(wifi_label);

    // Temperature Unit button (future)
    lv_obj_t *temp_btn = lv_button_create(settings_screen);
    lv_obj_set_size(temp_btn, SCREEN_WIDTH - 40, 60);
    lv_obj_set_pos(temp_btn, 20, 130);
    lv_obj_set_state(temp_btn, LV_STATE_DISABLED);  // Disabled for now

    lv_obj_t *temp_label = lv_label_create(temp_btn);
    lv_label_set_text(temp_label, LV_SYMBOL_SETTINGS " Temperature Unit (Coming Soon)");
    lv_obj_center(temp_label);

    // ... rest of settings screen ...
}

// Add event callback
static void wifi_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        #if DEBUG_NAVIGATION
            DEBUG_PRINTLN("[SETTINGS] Navigating to WiFi configuration");
        #endif
        createWiFiScreen();
    }
}
```

### Module 5: Main Sketch Updates (JC4827W543_LVGLv9.ino)

**Add WiFi initialization in setup():**
```cpp
#include "src/wifi_manager.h"
#include "src/ui_wifi.h"

void setup() {
    // ... existing initialization ...

    // ========================================
    // Initialize WiFi
    // ========================================
    Serial.println("\nInitializing WiFi...");
    if (initWiFiManager()) {
        Serial.println("WiFi manager initialized");

        // Try to auto-connect to saved network
        if (autoConnectWiFi()) {
            Serial.println("Auto-connected to saved WiFi network");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("No saved WiFi credentials or connection failed");
            Serial.println("Configure WiFi through Settings menu");
        }
    }

    // ... rest of setup ...
}
```

**Add WiFi status updates in loop():**
```cpp
void loop() {
    // ... existing loop code ...

    // Update WiFi status if WiFi screen is active
    if (wifi_screen != NULL) {
        static unsigned long lastWiFiUpdate = 0;
        if (millis() - lastWiFiUpdate >= WIFI_STATUS_UPDATE_INTERVAL) {
            updateWiFiStatus();
            lastWiFiUpdate = millis();
        }
    }

    // ... rest of loop ...
}
```

## Implementation Phases

### Phase 1: WiFi Manager Module (Core Functionality)
**Estimated Effort:** 2-3 hours

**Tasks:**
1. Create `src/wifi_manager.h` file
2. Implement credential storage functions (Preferences API)
3. Implement WiFi scanning functions
4. Implement connection management
5. Add auto-reconnect logic
6. Add signal strength helpers
7. Add debug logging

**Testing:**
- Test credential save/load cycle
- Test WiFi scanning
- Test connection to known network
- Test auto-reconnect after power cycle

### Phase 2: WiFi Settings UI (User Interface)
**Estimated Effort:** 3-4 hours

**Tasks:**
1. Create `src/ui_wifi.h` file
2. Implement screen layout with all widgets
3. Create network list with signal indicators
4. Implement LVGL keyboard integration (from demo code)
5. Add password show/hide toggle
6. Create all event handlers
7. Add status display updates

**Testing:**
- Test touch-based network selection
- Test keyboard input for password
- Test password visibility toggle
- Test all button interactions
- Verify screen transitions

### Phase 3: Integration (Connecting Everything)
**Estimated Effort:** 1-2 hours

**Tasks:**
1. Update `src/config.h` with WiFi constants
2. Modify `src/ui_settings.h` to launch WiFi screen
3. Update main sketch with WiFi initialization
4. Add WiFi status updates in main loop
5. Test complete workflow

**Testing:**
- Test Settings → WiFi Configuration navigation
- Test Back button returns to Settings
- Test auto-connect on boot
- Test credential persistence across restarts
- Test connection to multiple different networks

### Phase 4: Documentation and Polish
**Estimated Effort:** 1 hour

**Tasks:**
1. Update `docs/PROJECT_PLAN.md` with WiFi features
2. Create `docs/WIFI_SETUP.md` user guide
3. Update `docs/ARCHITECTURE.md` with WiFi modules
4. Update `README.md` feature list
5. Add inline code comments
6. Test and fix any edge cases

## Keyboard Implementation Details

### LVGL Keyboard Pattern (From Demo Code)
Based on `lv_demo_widgets.c` lines 251, 1202-1230:

**Key Principles:**
1. Create keyboard once, hide it by default
2. Link text areas to keyboard via event callbacks
3. Show keyboard when text area is focused
4. Hide keyboard when done/cancelled
5. Adjust screen layout to accommodate keyboard

**Code Pattern:**
```cpp
// 1. Create keyboard (hidden)
lv_obj_t *kb = lv_keyboard_create(lv_screen_active());
lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

// 2. Create text area with event callback
lv_obj_t *ta = lv_textarea_create(parent);
lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);

// 3. Event callback handles show/hide
static void ta_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    lv_obj_t *kb = lv_event_get_user_data(e);

    if (code == LV_EVENT_FOCUSED) {
        // Show keyboard, link to text area
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    else if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY) {
        // Hide keyboard
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}
```

### Password Text Area Specifics
```cpp
// Create password text area
password_textarea = lv_textarea_create(parent);
lv_textarea_set_one_line(password_textarea, true);      // Single line
lv_textarea_set_password_mode(password_textarea, true);  // Show dots
lv_textarea_set_placeholder_text(password_textarea, "Enter password");

// Toggle password visibility
void togglePasswordVisibility() {
    static bool visible = false;
    visible = !visible;
    lv_textarea_set_password_mode(password_textarea, !visible);
}
```

## Credential Storage Details

### ESP32 Preferences API
The ESP32 Preferences library provides key-value storage in NVS (Non-Volatile Storage).

**Header:**
```cpp
#include <Preferences.h>
```

**Usage Pattern:**
```cpp
Preferences preferences;

// Write mode
preferences.begin("namespace", false);  // false = read/write
preferences.putString("key", "value");
preferences.putBool("key", true);
preferences.putInt("key", 123);
preferences.end();

// Read mode
preferences.begin("namespace", true);   // true = read-only
String value = preferences.getString("key", "default");
bool flag = preferences.getBool("key", false);
preferences.end();

// Clear all data in namespace
preferences.begin("namespace", false);
preferences.clear();
preferences.end();
```

**WiFi Credentials Structure:**
```cpp
// Namespace: "wifi"
// Keys:
//   "configured" (bool)   - Flag indicating if WiFi is configured
//   "ssid" (String)       - Network SSID
//   "password" (String)   - Network password
```

## WiFi Status Reporting

### Connection States
```cpp
enum WiFiConnectionState {
    WIFI_STATE_IDLE,           // Not initialized
    WIFI_STATE_SCANNING,       // Scanning for networks
    WIFI_STATE_CONNECTING,     // Attempting to connect
    WIFI_STATE_CONNECTED,      // Successfully connected
    WIFI_STATE_DISCONNECTED,   // Not connected
    WIFI_STATE_FAILED          // Connection attempt failed
};
```

### Status Display Format
```cpp
void updateWiFiStatus() {
    if (WiFi.status() == WL_CONNECTED) {
        lv_label_set_text_fmt(status_label,
            "Status: Connected (%s)",
            getSignalQuality(WiFi.RSSI()));
        lv_label_set_text_fmt(ip_label,
            "IP: %s",
            WiFi.localIP().toString().c_str());
        lv_obj_set_style_text_color(status_label, COLOR_SECONDARY, 0);
    } else {
        lv_label_set_text(status_label, "Status: Not connected");
        lv_label_set_text(ip_label, "IP: ---");
        lv_obj_set_style_text_color(status_label, COLOR_DANGER, 0);
    }
}
```

## Signal Strength Visualization

### RSSI to Visual Indicators
```cpp
// Convert RSSI to bar count (1-4 bars)
int rssiToBars(int8_t rssi) {
    if (rssi >= -55) return 4;  // Excellent
    if (rssi >= -65) return 3;  // Good
    if (rssi >= -75) return 2;  // Fair
    if (rssi >= -85) return 1;  // Weak
    return 0;                   // Very weak/no signal
}

// Visual representation
const char* getSignalIcon(int8_t rssi) {
    int bars = rssiToBars(rssi);
    switch (bars) {
        case 4: return LV_SYMBOL_WIFI;      // ≡≡≡
        case 3: return "▂▄▆";               // Three bars
        case 2: return "▂▄";                // Two bars
        case 1: return "▂";                 // One bar
        default: return LV_SYMBOL_WARNING;  // No signal
    }
}
```

### Color Coding
```cpp
lv_color_t getSignalColor(int8_t rssi) {
    if (rssi >= -60) return COLOR_SECONDARY;  // Green - Good
    if (rssi >= -70) return COLOR_ACCENT;     // Orange - Fair
    return COLOR_DANGER;                      // Red - Weak
}
```

## Security Considerations

### Password Storage
- Passwords stored in ESP32 NVS are **not encrypted**
- NVS is flash-based storage, survives power cycles
- Data can be read if physical access to chip
- For production: Consider using ESP32 flash encryption feature

### WiFi Security
- Support WPA/WPA2 secured networks
- Display lock icon for secured networks
- Warn user when connecting to open networks
- Consider adding "Remember this network" checkbox

### Best Practices
```cpp
// Clear password from text area after successful connection
void clearPasswordInput() {
    lv_textarea_set_text(password_textarea, "");
}

// Option to forget saved credentials
void forgetNetwork() {
    preferences.begin("wifi", false);
    preferences.clear();
    preferences.end();
    WiFi.disconnect(true, true);  // Disconnect and erase stored credentials
}
```

## Error Handling

### Connection Failures
```cpp
void handleConnectionFailure(int reason) {
    switch (WiFi.status()) {
        case WL_NO_SSID_AVAIL:
            lv_label_set_text(status_label, "Error: Network not found");
            break;
        case WL_CONNECT_FAILED:
            lv_label_set_text(status_label, "Error: Wrong password");
            break;
        case WL_CONNECTION_LOST:
            lv_label_set_text(status_label, "Error: Connection lost");
            break;
        case WL_DISCONNECTED:
            lv_label_set_text(status_label, "Error: Disconnected");
            break;
        default:
            lv_label_set_text(status_label, "Error: Connection failed");
    }
}
```

### Scan Failures
```cpp
int scanWithRetry(int maxAttempts = 3) {
    for (int i = 0; i < maxAttempts; i++) {
        int n = WiFi.scanNetworks();
        if (n >= 0) return n;
        delay(1000);
    }
    return -1;  // Scan failed
}
```

## Testing Plan

### Unit Testing
1. **Credential Storage**
   - Save credentials → Load credentials → Verify match
   - Clear credentials → Verify cleared
   - Test with special characters in password

2. **WiFi Scanning**
   - Scan networks → Verify count > 0
   - Test SSID retrieval
   - Test RSSI retrieval
   - Test security detection

3. **Connection Management**
   - Connect to known network → Verify success
   - Connect with wrong password → Verify failure
   - Disconnect → Verify disconnected state

### Integration Testing
1. **User Workflow**
   - Navigate Settings → WiFi Configuration
   - Scan networks
   - Select network from list
   - Enter password using keyboard
   - Toggle password visibility
   - Connect to network
   - Verify connection status
   - Return to menu

2. **Persistence Testing**
   - Connect to network
   - Save credentials
   - Reboot device
   - Verify auto-connect on boot
   - Verify IP address displayed

3. **Edge Cases**
   - Network out of range
   - Wrong password entry
   - Network with no password (open)
   - Special characters in SSID/password
   - Very long SSID names (>32 chars)
   - Rapid connect/disconnect cycles

### UI Testing
1. **Keyboard Functionality**
   - Keyboard appears on text area focus
   - Keyboard hides on done/cancel
   - Keyboard input appears in text area
   - Backspace/delete works correctly

2. **Touch Responsiveness**
   - All buttons respond to touch
   - List scrolling works smoothly
   - Network selection highlights correctly
   - Password toggle works immediately

3. **Visual Feedback**
   - Status messages update correctly
   - Signal strength icons display properly
   - Color coding matches signal quality
   - Lock icons show for secured networks

## Debug and Troubleshooting

### Serial Debug Output
With `DEBUG_WIFI` enabled in config.h:
```
[WIFI] Initializing WiFi manager...
[WIFI] Loading saved credentials...
[WIFI] Found saved network: Home-Network
[WIFI] Attempting to connect...
[WIFI] Connected successfully
[WIFI] IP Address: 192.168.1.100
[WIFI] Signal strength: -45 dBm (Excellent)
```

### Common Issues and Solutions

**Issue: WiFi scan returns 0 networks**
- Solution: Ensure WiFi mode is set to STA
- Solution: Add delay after WiFi.disconnect()
- Solution: Check antenna connection

**Issue: Connection timeout**
- Solution: Increase WIFI_CONNECT_TIMEOUT
- Solution: Verify password is correct
- Solution: Check if network is in range

**Issue: Keyboard doesn't appear**
- Solution: Verify keyboard is created
- Solution: Check event callback is registered
- Solution: Ensure keyboard is not permanently hidden

**Issue: Credentials not saved**
- Solution: Verify Preferences.end() is called
- Solution: Check NVS partition is not full
- Solution: Test with Preferences.clear() first

## Future Enhancements

### Phase 2 Features (Later)
1. **WiFi Network Management**
   - Save multiple networks with priority
   - Auto-switch to strongest known network
   - Network profiles (home, work, guest)

2. **Advanced Configuration**
   - Static IP configuration
   - DNS server settings
   - Hostname customization
   - WiFi power save mode options

3. **Remote Access**
   - Web server for remote monitoring
   - mDNS/Bonjour discovery
   - OTA (Over-The-Air) firmware updates

4. **Data Logging**
   - Log temperature/humidity to cloud
   - MQTT integration
   - ThingSpeak/Adafruit IO support

5. **UI Enhancements**
   - Signal strength graph over time
   - Network speed test
   - Connected devices list (if in AP mode)
   - QR code for quick WiFi sharing

## Dependencies

### Required Libraries
```cpp
#include <WiFi.h>          // ESP32 WiFi library (built-in)
#include <Preferences.h>   // ESP32 Preferences library (built-in)
```

### No Additional Library Installation Required
Both WiFi and Preferences are part of the ESP32 Arduino core and don't need separate installation.

## File Size Estimates

**Code Size:**
- `wifi_manager.h`: ~500 lines
- `ui_wifi.h`: ~600 lines
- Total new code: ~1,100 lines

**Documentation:**
- This plan document: ~2,000 lines
- User guide: ~300 lines
- Architecture updates: ~200 lines

## Completion Checklist

### Implementation Complete When:
- [ ] WiFi manager module fully functional
- [ ] WiFi settings screen with all UI elements
- [ ] Keyboard integration working smoothly
- [ ] Password show/hide toggle functional
- [ ] Network scanning and selection working
- [ ] Credential storage and retrieval working
- [ ] Auto-connect on boot functional
- [ ] Settings menu launches WiFi screen
- [ ] All navigation (back buttons) working
- [ ] Status display shows correct information
- [ ] Error handling for all failure cases
- [ ] Serial debug output comprehensive
- [ ] Code fully commented and documented
- [ ] Testing completed successfully
- [ ] Documentation updated

## Summary

This implementation plan provides a complete blueprint for adding WiFi configuration functionality to the Temperature & Humidity Tracker. The design:

✅ **Follows existing architecture patterns** - Modular structure matching current codebase
✅ **Reuses demo code** - LVGL keyboard implementation from factory examples
✅ **Well-tested approach** - Based on working WiFi examples from docs
✅ **User-friendly** - Touch-based interface with on-screen keyboard
✅ **Persistent storage** - Credentials survive power cycles
✅ **Auto-reconnect** - Seamless WiFi connection on boot
✅ **Comprehensive** - Complete error handling and status reporting

**Ready for implementation when you are!**

---

**Document Version:** 1.0
**Date:** 2025-11-21
**Status:** Planning Complete - Ready for Implementation
