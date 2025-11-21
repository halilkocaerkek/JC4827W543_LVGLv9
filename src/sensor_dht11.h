/**
 * @file sensor_dht11.h
 * @brief DHT11 Temperature and Humidity Sensor Module
 *
 * This module handles all DHT11 sensor operations including initialization,
 * reading values, error handling, and data filtering with moving average.
 *
 * Usage:
 *   1. Call initDHT11() in setup()
 *   2. Call updateDHT11() periodically in loop()
 *   3. Access current values via getCurrentTemperature() and getCurrentHumidity()
 *   4. Check sensor status via isDHT11Ready()
 */

#ifndef SENSOR_DHT11_H
#define SENSOR_DHT11_H

#include <DHT.h>
#include "config.h"

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

DHT dht(DHT11_PIN, DHT11_TYPE);

// Current sensor values
float currentTemperature = 0.0;
float currentHumidity = 0.0;
bool sensorReady = false;
unsigned long lastSensorRead = 0;

// Min/Max tracking
float minTemperature = 999.0;
float maxTemperature = -999.0;
float minHumidity = 999.0;
float maxHumidity = 0.0;

// Moving average buffers
float tempBuffer[SENSOR_MOVING_AVG_SAMPLES] = {0};
float humidBuffer[SENSOR_MOVING_AVG_SAMPLES] = {0};
int bufferIndex = 0;
bool bufferFilled = false;

// Error tracking
int consecutiveErrors = 0;
unsigned long lastSuccessfulRead = 0;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

/**
 * @brief Initialize the DHT11 sensor
 * @return true if initialization successful, false otherwise
 */
bool initDHT11() {
  DEBUG_PRINTLN("Initializing DHT11 sensor...");
  DEBUG_PRINT("DHT11 Pin: GPIO ");
  DEBUG_PRINTLN(DHT11_PIN);

  dht.begin();

  // Wait for sensor to stabilize
  delay(2000);

  // Try initial read
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    DEBUG_PRINTLN("DHT11 initialization failed - no response from sensor");
    sensorReady = false;
    return false;
  }

  currentTemperature = t;
  currentHumidity = h;
  minTemperature = t;
  maxTemperature = t;
  minHumidity = h;
  maxHumidity = h;

  sensorReady = true;
  lastSuccessfulRead = millis();

  DEBUG_PRINTLN("DHT11 initialized successfully!");
  DEBUG_PRINT("Initial Temperature: ");
  DEBUG_PRINT(t);
  DEBUG_PRINTLN("°C");
  DEBUG_PRINT("Initial Humidity: ");
  DEBUG_PRINT(h);
  DEBUG_PRINTLN("%");

  return true;
}

/**
 * @brief Calculate moving average for temperature
 * @param newValue New temperature reading
 * @return Averaged temperature value
 */
float calculateTempAverage(float newValue) {
  tempBuffer[bufferIndex] = newValue;

  float sum = 0;
  int count = bufferFilled ? SENSOR_MOVING_AVG_SAMPLES : (bufferIndex + 1);

  for (int i = 0; i < count; i++) {
    sum += tempBuffer[i];
  }

  return sum / count;
}

/**
 * @brief Calculate moving average for humidity
 * @param newValue New humidity reading
 * @return Averaged humidity value
 */
float calculateHumidAverage(float newValue) {
  humidBuffer[bufferIndex] = newValue;

  float sum = 0;
  int count = bufferFilled ? SENSOR_MOVING_AVG_SAMPLES : (bufferIndex + 1);

  for (int i = 0; i < count; i++) {
    sum += humidBuffer[i];
  }

  return sum / count;
}

/**
 * @brief Update buffer index for moving average
 */
void updateBufferIndex() {
  bufferIndex++;
  if (bufferIndex >= SENSOR_MOVING_AVG_SAMPLES) {
    bufferIndex = 0;
    bufferFilled = true;
  }
}

/**
 * @brief Read and update sensor values
 * @return true if read was successful, false otherwise
 */
bool readDHT11() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Check for read errors
  if (isnan(t) || isnan(h)) {
    consecutiveErrors++;
    DEBUG_PRINT("DHT11 read error (consecutive errors: ");
    DEBUG_PRINT(consecutiveErrors);
    DEBUG_PRINTLN(")");

    if (consecutiveErrors >= SENSOR_RETRY_ATTEMPTS) {
      sensorReady = false;
      DEBUG_PRINTLN("DHT11 sensor marked as not ready");
    }
    return false;
  }

  // Valid reading - reset error counter
  consecutiveErrors = 0;
  sensorReady = true;
  lastSuccessfulRead = millis();

  // Apply moving average filter
  float avgTemp = calculateTempAverage(t);
  float avgHumid = calculateHumidAverage(h);
  updateBufferIndex();

  currentTemperature = avgTemp;
  currentHumidity = avgHumid;

  // Update min/max values
  if (avgTemp < minTemperature) minTemperature = avgTemp;
  if (avgTemp > maxTemperature) maxTemperature = avgTemp;
  if (avgHumid < minHumidity) minHumidity = avgHumid;
  if (avgHumid > maxHumidity) maxHumidity = avgHumid;

  #if DEBUG_SENSOR_VALUES
    DEBUG_PRINT("[DHT11] T: ");
    DEBUG_PRINT(avgTemp);
    DEBUG_PRINT("°C, H: ");
    DEBUG_PRINT(avgHumid);
    DEBUG_PRINT("%, Min T: ");
    DEBUG_PRINT(minTemperature);
    DEBUG_PRINT("°C, Max T: ");
    DEBUG_PRINT(maxTemperature);
    DEBUG_PRINT("°C");
    DEBUG_PRINTLN();
  #endif

  return true;
}

/**
 * @brief Update sensor readings if interval has elapsed
 * Call this function in loop()
 */
void updateDHT11() {
  unsigned long currentTime = millis();

  if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = currentTime;
    readDHT11();
  }
}

/**
 * @brief Get current temperature reading
 * @return Temperature in Celsius
 */
float getCurrentTemperature() {
  return currentTemperature;
}

/**
 * @brief Get current humidity reading
 * @return Humidity in percentage
 */
float getCurrentHumidity() {
  return currentHumidity;
}

/**
 * @brief Convert Celsius to Fahrenheit
 * @param celsius Temperature in Celsius
 * @return Temperature in Fahrenheit
 */
float celsiusToFahrenheit(float celsius) {
  return (celsius * 9.0 / 5.0) + 32.0;
}

/**
 * @brief Get temperature in specified unit
 * @param unit Temperature unit (TEMP_CELSIUS or TEMP_FAHRENHEIT)
 * @return Temperature in specified unit
 */
float getTemperature(TempUnit unit) {
  if (unit == TEMP_FAHRENHEIT) {
    return celsiusToFahrenheit(currentTemperature);
  }
  return currentTemperature;
}

/**
 * @brief Check if sensor is ready and functioning
 * @return true if sensor is operational
 */
bool isDHT11Ready() {
  return sensorReady;
}

/**
 * @brief Get minimum recorded temperature
 * @return Minimum temperature in Celsius
 */
float getMinTemperature() {
  return minTemperature;
}

/**
 * @brief Get maximum recorded temperature
 * @return Maximum temperature in Celsius
 */
float getMaxTemperature() {
  return maxTemperature;
}

/**
 * @brief Get minimum recorded humidity
 * @return Minimum humidity in percentage
 */
float getMinHumidity() {
  return minHumidity;
}

/**
 * @brief Get maximum recorded humidity
 * @return Maximum humidity in percentage
 */
float getMaxHumidity() {
  return maxHumidity;
}

/**
 * @brief Reset min/max tracking values
 */
void resetMinMax() {
  minTemperature = currentTemperature;
  maxTemperature = currentTemperature;
  minHumidity = currentHumidity;
  maxHumidity = currentHumidity;

  DEBUG_PRINTLN("Min/Max values reset");
}

/**
 * @brief Get time since last successful sensor read
 * @return Milliseconds since last successful read
 */
unsigned long getTimeSinceLastRead() {
  return millis() - lastSuccessfulRead;
}

/**
 * @brief Get sensor error status message
 * @return Status string
 */
const char* getDHT11Status() {
  if (!sensorReady) {
    return "Sensor Error";
  }

  unsigned long timeSince = getTimeSinceLastRead();
  if (timeSince > SENSOR_READ_INTERVAL * 3) {
    return "Sensor Timeout";
  }

  return "OK";
}

#endif // SENSOR_DHT11_H
