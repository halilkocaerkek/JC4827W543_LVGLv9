# DHT11 Temperature and Humidity Sensor - Wiring Guide

## Overview
This guide explains how to connect the DHT11 sensor to the JC4827W543 development board.

## Components Required

### Hardware
1. **JC4827W543 Development Board** (with capacitive touch)
2. **DHT11 Temperature and Humidity Sensor Module**
3. **3x Female-to-Female Jumper Wires** (or appropriate connectors)

### Optional (for breadboard setup)
- Breadboard (400 or 830 tie-points)
- Male-to-Female jumper wires
- 10kΩ pull-up resistor (if using bare DHT11 sensor)

## Pin Connections

### JC4827W543 Board Pinout Reference

Based on the board specification, the Extended IO connectors are located at the bottom of the board. Each connector provides access to GPIO pins.

**Selected Pin for DHT11:**
- **Data Pin:** GPIO 1 (Extended IO connector)

### DHT11 Module Connections

Most DHT11 modules come with 3 pins:

```
DHT11 Module          JC4827W543 Board
┌────────────┐        ┌─────────────────┐
│            │        │                 │
│   [VCC]────┼────────┼─── 5V or 3.3V   │
│            │        │    (Extended IO)│
│  [DATA]────┼────────┼─── GPIO 1       │
│            │        │    (Extended IO)│
│   [GND]────┼────────┼─── GND          │
│            │        │    (Extended IO)│
└────────────┘        └─────────────────┘
```

### Detailed Pin Assignment

| DHT11 Pin | Wire Color (typical) | JC4827W543 Pin | Notes |
|-----------|---------------------|----------------|-------|
| VCC       | Red                 | 3.3V or 5V     | Most DHT11 modules work with 3.3V-5V |
| DATA      | Yellow/White        | GPIO 1         | Data communication pin |
| GND       | Black               | GND            | Common ground |

## Board Interface Location

### Extended IO Connector Layout

The JC4827W543 board has 4 Extended IO connectors at the bottom edge:

```
Board Back View:
┌──────────────────────────────────────────────┐
│                                              │
│            ESP32-S3 Module                   │
│                                              │
│                                              │
│    [TF Card Slot]     [Battery Connector]    │
│                                              │
└──────────────────────────────────────────────┘
  [EXT IO] [EXT IO] [EXT IO] [EXT IO]
   Conn 1   Conn 2   Conn 3   Conn 4
```

**Connector Pin Access:**
Each Extended IO connector typically provides:
- Power (3.3V or 5V)
- Ground (GND)
- 2-3 GPIO pins

**Recommended Connector:** Use Extended IO Connector 1 or 2 for easy access.

## Step-by-Step Wiring Instructions

### Method 1: Direct Connection (Module to Board)

1. **Identify the Extended IO connector** on your board (bottom edge)

2. **Connect VCC (Power)**
   - Take a red jumper wire
   - Connect DHT11 VCC pin to 3.3V on Extended IO
   - Note: Check your DHT11 module datasheet - most work with 3.3V

3. **Connect DATA (Signal)**
   - Take a yellow/white jumper wire
   - Connect DHT11 DATA pin to GPIO 1 on Extended IO

4. **Connect GND (Ground)**
   - Take a black jumper wire
   - Connect DHT11 GND pin to GND on Extended IO

### Method 2: Using a Breadboard

If using a breadboard for easier prototyping:

```
Breadboard Layout:

         DHT11 Module
         ┌─────────┐
         │  [VCC]  │ ─── Red wire ──> 3.3V (JC4827W543)
         │         │
         │ [DATA]  │ ─── Yellow ───> GPIO 1 (JC4827W543)
         │         │
         │  [GND]  │ ─── Black ────> GND (JC4827W543)
         └─────────┘
```

**Steps:**
1. Insert DHT11 module into breadboard
2. Use jumper wires to connect from breadboard to board Extended IO pins
3. Ensure solid connections (no loose wires)

## Bare DHT11 Sensor (4-pin version)

If you have a bare DHT11 sensor (not a module), you'll need a pull-up resistor:

```
         Bare DHT11 (Front View)
         ┌───────────┐
         │  1  2  3  4  │
         └───────────┘
         │   │  │  │
         │   │  │  └─── Not connected (NC)
         │   │  └────── GND ───────────────> GND (Board)
         │   └───────── DATA ──┬──────────> GPIO 1 (Board)
         │                     │
         │                   [10kΩ]
         │                     │
         └─────────── VCC ─────┴──────────> 3.3V (Board)
```

**Additional Component:**
- 10kΩ resistor between VCC and DATA pin

## Visual Verification

### Before Powering On
- [ ] Double-check all connections match the wiring diagram
- [ ] Verify no short circuits between VCC and GND
- [ ] Ensure DHT11 is oriented correctly (check pin labels)
- [ ] Confirm GPIO 1 is not used by other peripherals

### After Powering On
- [ ] DHT11 should not get hot (sign of short circuit)
- [ ] Board powers on normally with display active
- [ ] Serial monitor shows sensor readings (after code upload)

## Troubleshooting

### Common Issues

| Problem | Possible Cause | Solution |
|---------|---------------|----------|
| No sensor readings | Wrong pin connection | Verify DATA is on GPIO 1 |
| "Checksum error" | Loose connection | Check all wire connections |
| Random readings | No pull-up resistor | Add 10kΩ resistor (bare sensor) |
| Sensor not responding | Wrong voltage | Use 3.3V instead of 5V |
| Display not working | Power issue | Check if board is drawing too much current |

### GPIO Conflict Check

**Pins to AVOID (already in use):**
- GPIO 3: Touch INT
- GPIO 4: Touch SCL
- GPIO 8: Touch SDA
- GPIO 38: Touch RST
- GPIO 2, 42, 41, 40, 39, 14, 13, 12, 11, 10, 9, 46, 45, 48, 47: Display pins
- GPIO 43, 44: USB

**Safe GPIO pins for DHT11:**
- GPIO 1 ✓ (Recommended - used in this project)
- GPIO 5
- GPIO 6
- GPIO 7
- GPIO 15
- GPIO 16
- GPIO 17
- GPIO 18

## Wiring Diagram

### ASCII Art Diagram

```
┌─────────────────────────────────────────┐
│      JC4827W543 Development Board       │
│  ┌──────────────────────────────┐       │
│  │                              │       │
│  │     480x272 TFT Display      │       │
│  │     with Touch Screen        │       │
│  │                              │       │
│  └──────────────────────────────┘       │
│                                         │
│         [ESP32-S3-WROOM-1]              │
│                                         │
│  [USB-C]           [Battery] [Speaker]  │
│                                         │
└─────┬───────┬───────┬───────┬───────────┘
      │       │       │       │
      │ Ext   │ Ext   │ Ext   │ Ext
      │ IO 1  │ IO 2  │ IO 3  │ IO 4
      │       │       │       │
      │ GPIO1 ├───────┤       │
      │ 3.3V  │       │       │
      │ GND   │       │       │
      │       │       │       │
      └───┬───┘       │       │
          │           │       │
          │ Yellow    │       │
          └───┐       │       │
              │       │       │
          ┌───┴───────┴───────┴───┐
          │                       │
          │   ┌─────────────┐     │
          │   │   DHT11     │     │
          │   │   MODULE    │     │
          │   └─────────────┘     │
          │    VCC DATA GND       │
          │     │    │    │       │
          └─────┼────┼────┼───────┘
               Red Yellow Black
                │    │    │
       3.3V ────┘    │    └──── GND
                     │
              GPIO 1 ┘
```

## Safety Precautions

⚠️ **Important Safety Notes:**

1. **Always disconnect power** before making wiring changes
2. **Do not connect VCC directly to GND** - this will damage the sensor
3. **Use the correct voltage** - Check your DHT11 module specifications
4. **Avoid loose connections** - They can cause intermittent failures
5. **Do not bend pins excessively** - This can break the sensor

## Testing the Connection

After wiring, upload the test sketch and verify:

1. Open Arduino Serial Monitor (115200 baud)
2. You should see output like:
   ```
   DHT11 Sensor Test
   Temperature: 23.0°C
   Humidity: 45.0%
   ```
3. Breathe on the sensor - humidity should increase
4. Touch the sensor - temperature should increase

## Physical Installation Tips

### Mounting Options
1. **Tape/Adhesive**: Use double-sided tape to mount sensor near the board
2. **Standoffs**: Use 3mm spacers to mount sensor on breadboard
3. **Case Integration**: Design a case with sensor opening for air flow

### Sensor Placement Considerations
- **Avoid heat sources**: Keep away from voltage regulators, ESP32 module
- **Allow air flow**: Don't cover the sensor grid
- **Protect from liquids**: DHT11 is not waterproof
- **Stable mounting**: Vibration can affect readings

## Next Steps

After wiring is complete:
1. Verify connections match this guide
2. Upload the temperature tracker firmware
3. Test sensor readings via serial monitor
4. Calibrate if necessary using a reference thermometer

## Reference Images

For actual board photos and pinout diagrams, refer to:
- `docs/JC4827W543/2-Specification/JC4827W543 Specifications-EN.pdf` (Page 5)
- `docs/JC4827W543/6-User_Manual/Getting started JC4827W543.pdf`

## Questions?

If you encounter issues not covered in the troubleshooting section:
1. Check all connections against this guide
2. Verify GPIO 1 is not used elsewhere in your code
3. Test with a simple DHT11 example sketch first
4. Measure voltage at sensor VCC pin (should be 3.3V)
