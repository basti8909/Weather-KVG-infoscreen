/**
  WiFi Info Monitor
  Name: ESP_InfoDislplay.ino
  Purpose: Displays local weather and live KVG bus timetables on an ILI9341 compatible display. Based on an ESP8266 NodeMCU board.

  @author basti8909
  @version 1.0 16/08/17

  The circuit:
  * ESP8266 NodeMCU
  * ILI9341 compatible 2.8" display (320x240px) with XPT2046 touch interface
  
  Connection of TFT display:
  * VCC   -> 3.3V
  * GND   -> GND
  * CS    -> D8
  * RESET -> 3.3V
  * DC    -> D3
  * SDI   -> D7
  * SCK   -> D5
  * LED   -> 3.3V
  * SDO   -> D6
  * T_CLK -> D5
  * T_CS  -> D2
  * T_DIN -> D7
  * T_DO  -> D6
  * T_IRQ -> D1
*/

// *** DEFINITIONS ***

#define TIMER_INTERVAL_WIFI  5000     // Wifi Loop 12x per minute
#define TIMER_INTERVAL_CLOCK 1000     // Refresh displayed clock time every second

// *** LIBRARIES ***

// Arduino
#include <Arduino.h>
#include <SPI.h>
#include <String.h>
#include <TimeLib.h>

// ESP8266 WiFi connection
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

// ILI9341 display
#include <TFT_eSPI.h>
// XPT2046 touch controller
#include <XPT2046.h>

// KVG live interface
#include "KVGliveAPI.h"
// DarkSky Weather Interface
#include "DarkSkyAPI.h"
// Display classes
#include "Display.h"

// *** PROTOTYPES ***

// NTP prototype for TimeLib syncClient
time_t getNtpTime();

// *** VARIABLES ***

// Multi Wifi Connection
//ESP8266WiFiMulti WiFiMulti;
// HTTP Client
HTTPClient http;
// TFT Display
TFT_eSPI tft = TFT_eSPI();
// Touch controller
XPT2046 touch(/*cs=*/ D2, /*irq=*/ D1);

// KVG live interface
KVGliveAPI KVG(http);
// DarkSky Weather interface
DarkSkyAPI Weather(http);
// Display base
Base B(&tft, &touch);
// Display elements
Clock clk(&B);
BusScreen bus(&B);
WeatherScreen wscr(&B);

// Global timer variable for WiFi request interval
uint32_t timerWifi;
// when the digital clock was displayed
uint32_t timerClock; 

// Current display mode (0: 1st KVG stop, 1: 2nd KVG stop, 2: Weather screen)
uint8_t mode = 2;


// *** MAIN FUNCTIONS ***

void setup(void) {
  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);
  
  Serial.begin(115200);

  B.initDispAndTouch();

  // Setup connection
  WiFi.begin("SSID", "Your Wifi Password");
  
  int counter = 0;
  // Wait for WiFi Connection
  while (WiFi.status() != WL_CONNECTED)
  {
    B.drawLogoSpinner(counter,WiFi_Logo_bits,WiFi_Logo_width,WiFi_Logo_height);
    delay(500);
    counter++;
  }
  B.clearScreen();

  // Start time synchronization with NTP server
  NTP_startSync();
  setSyncInterval(3);
 
  // Read weather information
  B.drawCenterXbm(W1_Logo_width, W1_Logo_height, W1_Logo_bits, TFT_WHITE);
  B.drawCenterXbm(W2_Logo_width, W2_Logo_height, W2_Logo_bits, TFT_YELLOW);
  if (timeStatus() != timeNotSet) Weather.UTCmidnightTime = now() - hour()*3600 - minute()*60 - second();
  Weather.queryWeather(DarkSkyAPI::Location::Kiel);  
  wscr.updateWeather(Weather.readWeather());
  B.clearScreen();    
  digitalWrite(D0, LOW);
}

void loop() {
  // Touch handling
  if (touch.isTouching()) {
    mode += 1;
    if (mode == 3) mode = 0;
    
    timerWifi = 0;

    B.clearScreen(); 
    bus.setReDrawFlag();  
    clk.setReDrawFlag(); 
    wscr.setReDrawFlag(); 
    // Don't use position reading
    //    uint16_t x, y;
    //    touch.getPosition(x, y);
    //    switch (tftMode)
    //
    //
    //    prev_x = x;
    //    prev_y = y;
    //  } else {
    //    prev_x = prev_y = 0xffff;
  }

  // Wifi connection
  if (millis() > timerWifi && WiFi.status() == WL_CONNECTED)
  {
    timerWifi = millis() + TIMER_INTERVAL_WIFI;
    int stopNr;
    // Mode 0/1: Show KVG bus stop
    if (mode < 2) {
      if (mode == 0) {stopNr = KVGliveAPI::KVGstop::Wrangelstrasse;}
      else {          stopNr = KVGliveAPI::KVGstop::Schauspielhaus;}  
  
      if (KVG.queryBusStop(stopNr))
      {
        bus.updateBusStop(KVG.readBusStop());
        bus.Draw();
      }
    }
    // Mode 2: Show weather forecast
    else 
    {
      timerWifi = millis() + 3600000;
      wscr.Draw();
    }
    

  }

  // Clock display on KVG screens
  if (timeStatus() != timeNotSet && mode != 2)
  {
    if (millis() > timerClock)
    {
      timerClock = millis() + TIMER_INTERVAL_CLOCK;
      clk.updateTime((TimeOfDay) {hour(), minute(), second()});
      clk.Draw();
    }
  }

  delay(50);
}


