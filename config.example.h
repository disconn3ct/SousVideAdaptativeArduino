#pragma once

// ------------------------- CONFIGURATION

#define WIFI // Comment this out to disable wifi (including OTA and influx)
// Wifi configuration
#define HOSTNAME "esp8266-sousvide"
#define WIFI_SSID "WIFIRules"
#define WIFI_PASSWORD "TerSekretestPassw3rd"

// influxdb host
#ifdef WIFI

#define INFLUXDB // Comment this out to disable influx logging
#define INFLUX_HOST {192, 168, 1, 5}
#define INFLUX_PORT 8086

// WARNING DANGER AHEAD SERIOUSLY DON'T DO THIS!!
// DO NOT UPDATE WHILE IN USE!
// DO NOT UPDATE WHILE UNATTENDED!
// In general, DO NOT USE THIS. It is only present if you know the risks and are prepared
// for fire, property damage, destruction of your hardware, and other calamities.
// The unit may behave unpredictably during and after an update.
// No state is saved, and no checks are done to ensure the relay is off before updating!
// OTA based on https://arduino-esp8266.readthedocs.io/en/2.5.0-beta2/ota_updates/readme.html
// #define OTA // Uncomment this to enable OTA updates
// Also be careful that the build does not take over 50% of program space:
// Sketch uses 299024 bytes (28%) of program storage space.

#endif // No influx or OTA without Wifi

// Wiring configuration
// Use the board-specific pin names/numbers here

// push-buttons
#define BT_TEMP_MORE_PIN D2 //INPUT_PULLUP mode
#define BT_TEMP_LESS_PIN D5 //INPUT_PULLUP mode

// piezo
#define PIEZO_PIN D6

// temperature sensor
#define ONE_WIRE_BUS D1
#define TEMPERATURE_PRECISION 9
#define SAMPLE_DELAY 5000
#define OUTPUT_TO_SERIAL true

// relay
#define RELAY_OUT_PIN D7

// relay configiguration
#define RELAY_ON HIGH
#define RELAY_OFF LOW

// Display
#define SDA_PIN D3
#define SDC_PIN D4
#define I2C_DISPLAY_ADDRESS 0x3c

#define SECS_PER_FRAME 7
