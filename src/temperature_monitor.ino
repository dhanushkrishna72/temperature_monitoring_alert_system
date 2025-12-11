/*
  Temperature Monitoring & Alert System
  Author: Dhanush K
  Date: 2025
  MCU: ATmega328P (Arduino Uno/Nano)
  Sensor: LM35 (default) OR NTC (optional)
*/

#include <Arduino.h>

// -------- CONFIG -----------
#define SENSOR_PIN A0
#define BUZZER_PIN 8      // digital pin to drive transistor base (via resistor)
#define LED_PIN 13        // optional LED for alarm
#define BAUD_RATE 9600

// Choose sensor type: uncomment LM35 or NTC
#define SENSOR_LM35
// #define SENSOR_NTC

// Threshold: in Celsius
float TEMP_THRESHOLD_C = 40.0;  // change as needed

// Calibration offset (if your sensor reads a bit high/low)
float calibration_offset = 0.0;

// Timing
unsigned long lastPrint = 0;
const unsigned long PRINT_INTERVAL = 1000; // ms

// Hysteresis to avoid chattering
const float HYSTERESIS = 1.0; // degrees C

// ---------- SETUP ----------
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(BAUD_RATE);
  while (!Serial) {
    ; // wait for serial
  }
  Serial.println(F("Temperature Monitoring & Alert System"));
  Serial.print(F("Sensor pin: A0, Buzzer pin: "));
  Serial.println(BUZZER_PIN);
  Serial.print(F("Threshold = "));
  Serial.print(TEMP_THRESHOLD_C);
  Serial.println(F("C"));
  Serial.println(F("Send 't:<value>' to set threshold (example: t:37.5)"));
  Serial.println(F("Send 'c:<value>' to set calibration offset (example: c:-0.5)"));
}

// ---------- HELPER FUNCTIONS ----------
float readTemperatureC() {
#ifdef SENSOR_LM35
  // LM35: Vout = 10mV per °C. ADC (10-bit) => 1023 => 5V
  int raw = analogRead(SENSOR_PIN);
  float voltage = (raw * 5.0) / 1023.0; // V
  float tempC = voltage * 100.0; // 10mV per °C -> multiply by 100
  tempC += calibration_offset;
  return tempC;
#elif defined(SENSOR_NTC)
  // Example for simple NTC divider (user must adjust R and Beta)
  const float R_FIXED = 10000.0; // fixed resistor in ohms
  const float BETA = 3950.0;     // Beta of the thermistor
  const float T0 = 298.15;       // 25C in Kelvin
  const float R0 = 10000.0;      // resistance at 25C (in ohms)
  int raw = analogRead(SENSOR_PIN);
  float vout = (raw * 5.0) / 1023.0;
  float rntc = (R_FIXED * vout) / (5.0 - vout);
  float tempK = 1.0 / ( (1.0 / T0) + (1.0 / BETA) * log(rntc / R0) );
  float tempC = tempK - 273.15;
  tempC += calibration_offset;
  return tempC;
#else
  return 0.0;
#endif
}

void buzzerBeep(unsigned int ms) {
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  delay(ms);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void successTone() {
  // two short beeps
  buzzerBeep(100);
  delay(70);
  buzzerBeep(100);
}

void failTone() {
  // long beep
  buzzerBeep(600);
}

// parse simple serial commands
void checkSerialCommands() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    // set threshold: t:37.5
    if (line.startsWith("t:") || line.startsWith("T:")) {
      String v = line.substring(2);
      float val = v.toFloat();
      if (val != 0.0 || v == "0" || v == "0.0") {
        TEMP_THRESHOLD_C = val;
        Serial.print(F("Threshold set to: "));
        Serial.print(TEMP_THRESHOLD_C);
        Serial.println(F(" C"));
      } else {
        Serial.println(F("Invalid threshold value"));
      }
    } else if (line.startsWith("c:") || line.startsWith("C:")) {
      String v = line.substring(2);
      float val = v.toFloat();
      calibration_offset = val;
      Serial.print(F("Calibration offset set to: "));
      Serial.print(calibration_offset);
      Serial.println(F(" C"));
    } else {
      Serial.print(F("Unknown command: "));
      Serial.println(line);
    }
  }
}

// ---------- MAIN LOOP ----------
bool alertActive = false;
void loop() {
  unsigned long now = millis();
  if (now - lastPrint >= PRINT_INTERVAL) {
    lastPrint = now;
    float tempC = readTemperatureC();
    Serial.print(F("Temperature: "));
    Serial.print(tempC, 2);
    Serial.println(F(" C"));

    // Alerting with hysteresis
    if (!alertActive && tempC >= TEMP_THRESHOLD_C) {
      // trigger alert
      Serial.println(F("ALERT: Temperature threshold exceeded!"));
      successTone(); // short confirmation before long alert
      alertActive = true;
      // continuous beep until temp drops below threshold - HYSTERESIS
      for (int i = 0; i < 3; ++i) { // beep 3 times as persistent notification
        buzzerBeep(400);
        delay(150);
      }
    } else if (alertActive && tempC < (TEMP_THRESHOLD_C - HYSTERESIS)) {
      // clear alert
      Serial.println(F("INFO: Temperature back to normal range."));
      failTone(); // short clear tone
      alertActive = false;
    }
  }

  // non-blocking check for serial commands
  checkSerialCommands();
}
