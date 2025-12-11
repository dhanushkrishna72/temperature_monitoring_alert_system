# Temperature Monitoring & Alert System

**Author:** Dhanush K  
**Institution:** JAIN (Deemed-to-be University)  
**Year:** 2025

## Project summary
A compact embedded system that monitors temperature in real-time using an LM35 (or NTC thermistor) connected to an Arduino Nano/Uno. The system reads analog sensor values via the ATmega328P ADC, converts them into °C, and raises an audible alert via a buzzer when the temperature exceeds a configurable threshold. Serial output is used for debugging and calibration.
checkout: [project drive link](https://drive.google.com/drive/folders/17z5s0GhKM8v2nqOlKFtRh3XV308lrLTc?usp=sharing)

## Features
-> Real-time temperature reading (°C)
-> Threshold-based alert (buzzer via BC547 transistor)
-> UART serial debug output (for calibration and logging)
-> Simple calibration routine (for thermistor setups)
-> Modular, commented Arduino code suitable for further extension

## Hardware
See `hardware/components.md` for the full list.

### Wiring overview (LM35)
-> LM35 VCC ->> 5V
-> LM35 GND ->> GND
-> LM35 Vout ->> A0 (Arduino)
-> Buzzer ->> BC547 transistor (collector) ->> buzzer ->> +5V, emitter ->> GND, base ->> 1k resistor ->> Arduino digital pin (D8)
-> Optionally add an LED to indicate alarm.

(If using NTC thermistor, add an appropriate resistor divider and change `SENSOR_TYPE` in code to `NTC`)

## Usage
1. Open the Arduino IDE.
2. Load `src/temperature_monitor.ino`.
3. Connect board and upload the sketch.
4. Open Serial Monitor at `9600` baud to view readings and logs.
5. The default alert threshold is defined in code as `TEMP_THRESHOLD_C`. Change and re-upload as needed.

## Code structure
-> `src/temperature_monitor.ino` —> main Arduino sketch
-> `hardware/` —> circuit images & BOM
-> `docs/` —> calibration notes and test logs

## How to run (quick)
-> Upload sketch
-> Monitor serial output
-> Trigger alerts by heating sensor gently (e.g., cup of warm water for LM35)

## Future improvements (ideas)
-> Add LCD/OLED to show current temp
-> Log data to SD card or send via ESP32/ESP8266
-> Implement hysteresis to prevent rapid toggling of buzzer
-> Add push button to change threshold at runtime


