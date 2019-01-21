#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <brzo_i2c.h>
#include <SPI.h>
#include <OLEDDisplayFonts.h>
#include "WeatherStationImages.h"
#include "WeatherStationFonts.h"
#include <SSD1306Wire.h>
#include <OLEDDisplayUi.h>
#include "config.h"
#define IS_METRIC 1

#define W_DEBUG 0
#define debug_print(lvl, fmt)  \
    do                         \
    {                          \
        if (W_DEBUG >= lvl)    \
            Serial.print(fmt); \
    } while (0)
#define debug_println(lvl, fmt)  \
    do                           \
    {                            \
        if (W_DEBUG >= lvl)      \
            Serial.println(fmt); \
    } while (0)
#define debug_printf(lvl, fmt, ...)          \
    do                                       \
    {                                        \
        if (W_DEBUG >= lvl)                  \
            Serial.printf(fmt, __VA_ARGS__); \
    } while (0)

SSD1306Wire display(I2C_DISPLAY_ADDRESS, SDA_PIN, SDC_PIN);
OLEDDisplayUi ui(&display);

//declaring prototypes
void sd_loop();
void sd_error(String err);
void sd_drawProgress(OLEDDisplay *display, int percentage, String label);
void sd_drawActual(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void sd_drawWanted(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void sd_drawError(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
void sd_drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState *state);
void sd_drawFooterOverlay(OLEDDisplay *display, OLEDDisplayUiState *state);

extern double targetTemp;
extern double actualTemp;

// Add frames
// this array keeps function pointers to all frames
// frames are the single views that slide from right to left
FrameCallback frames[] = {sd_drawActual, sd_drawWanted};
int numberOfFrames = 2;

#define HEADERSIZE 13

int headerOffset = 0;
OverlayCallback overlays[] = {sd_drawFooterOverlay};
// To start with the overlay on top, use these instead:
//int headerOffset = HEADERSIZE;
//OverlayCallback overlays[] = { sd_drawHeaderOverlay };
int numberOfOverlays = 1;

void sd_setup()
{
    Serial.println();
    Serial.println();
    debug_println(2, "Entering sd_setup");

    // initialize display
    display.init();
    display.clear();
    display.display();

    //display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setContrast(255);

    #ifdef WIFI

    int counter = 0;
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Avoid use of delay since we've got wifi running
        delay(500);

        Serial.print(".");
        display.clear();
        display.drawString(64, 10, "Connecting to WiFi");
        display.drawXbm(46, 30, 8, 8, counter % 3 == 0 ? activeSymbole : inactiveSymbole);
        display.drawXbm(60, 30, 8, 8, counter % 3 == 1 ? activeSymbole : inactiveSymbole);
        display.drawXbm(74, 30, 8, 8, counter % 3 == 2 ? activeSymbole : inactiveSymbole);
        display.display();

        counter++;
    }
    #endif

    ui.setTargetFPS(30);
    ui.setActiveSymbol(activeSymbole);
    ui.setInactiveSymbol(inactiveSymbole);

    // You can change this to
    // TOP, LEFT, BOTTOM, RIGHT
    ui.setIndicatorPosition(BOTTOM);
    // Defines where the first frame is located in the bar.
    ui.setIndicatorDirection(LEFT_RIGHT);

    // You can change the transition that is used
    // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
    ui.setFrameAnimation(SLIDE_LEFT);

    ui.setFrames(frames, numberOfFrames);

    ui.setOverlays(overlays, numberOfOverlays);

    ui.setTimePerFrame(SECS_PER_FRAME * 1000); // Setup frame display time to 10 sec

    // Inital UI takes care of initalising the display too.
    ui.init();
    ui.enableAutoTransition();

    sd_loop();
    debug_println(2, "-- Leaving sd_setup");
}

void sd_loop()
{
    debug_println(5, "Entering sd_loop");
    ui.update();
}

void sd_drawProgress(OLEDDisplay *display, int percentage, String label)
{
    debug_println(2, "Entering sd_drawProgress");
    display->clear();
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_10);
    display->drawString(64, 10, label);
    display->drawProgressBar(2, 28, 124, 10, percentage);
    display->display();
    debug_println(2, "-- Leaving sd_drawProgress");
}

String error;

void sd_error(String err)
{
    error = err;
    FrameCallback frames[] = {sd_drawError};
    ui.setFrames(frames, 1);
    ui.setTimePerFrame(9000); // Setup frame display time to 90 sec
    ui.update();
}

void sd_drawActual(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    debug_println(4, "Entering sd_drawWanted");

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 36 + y + headerOffset, "Current Temp");

    // Temperature
    display->setFont(ArialMT_Plain_24);
    String temp = String(actualTemp, 1) + (IS_METRIC ? "°C" : "°F");
    display->drawString(60 + x, 5 + y + headerOffset, temp);

    debug_println(4, "-- Leaving sd_drawActual");
}

void sd_drawWanted(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    debug_println(4, "Entering sd_drawWanted");

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 36 + y + headerOffset, "Target");

    // Temperature
    display->setFont(ArialMT_Plain_24);
    String temp = String(targetTemp, 1) + (IS_METRIC ? "°C" : "°F");
    display->drawString(60 + x, 5 + y + headerOffset, temp);

    debug_println(4, "-- Leaving sd_drawActual");
}

void sd_drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
{
    debug_println(4, "Entering sd_drawHeaderOverlay");

    ui.setIndicatorPosition(TOP);

    display->setColor(WHITE);
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    String temp = String(actualTemp, 1) + (IS_METRIC ? "°C" : "°F");
    display->drawString(0, 0, String(temp));
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    temp = String(targetTemp, 1) + (IS_METRIC ? "°C" : "°F");
    display->drawString(128, 0, temp);
    display->drawHorizontalLine(0, 12, 128);
    debug_println(4, "-- Leaving sd_drawHeaderOverlay");
}

void sd_drawFooterOverlay(OLEDDisplay *display, OLEDDisplayUiState *state)
{
    debug_println(4, "Entering sd_drawFooterOverlay");

    ui.setIndicatorPosition(BOTTOM);

    display->setColor(WHITE);
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    String temp = String(actualTemp, 1) + (IS_METRIC ? "°C" : "°F");
    display->drawString(0, 54, String(temp));
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    temp = String(targetTemp, 1) + (IS_METRIC ? "°C" : "°F");
    display->drawString(128, 54, temp);
    display->drawHorizontalLine(0, 52, 128);
    debug_println(4, "-- Leaving sd_drawFooterOverlay");
}

void sd_drawError(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
    debug_println(4, "Entering sd_drawError");

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 36 + y + headerOffset, "ERROR");

    // Temperature
    display->setFont(ArialMT_Plain_24);
    display->drawString(60 + x, 5 + y + headerOffset, error);

    debug_println(4, "-- Leaving sd_drawError");
}