/**
  DarkSky.net API Library Header
  Name: DarkSkyAPI.h
  Purpose: Fetch and parse data from DarkSky online API (For ESP8266)

  @author basti8909
  @version 1.0 16/08/17
*/

#ifndef DarkSkyAPI_h
#define DarkSkyAPI_h

#define DEBUG_DarkSkyAPI 0

#include "Arduino.h"
#include <String.h>
#include <TimeLib.h>
// Data Listener for JSON parser library
#include "DarkSkyAPI_DataListener.h"
// WIFI connection for ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
// JSON parser library
#include <JsonStreamingParser.h>

typedef struct WeatherForecast 
{
  String location;
  String actDesc;
  String actIco;
  float actTemp;
  int actWindSpd;
  int actWindDir;
  float fTemp[48];
  float fRainProb[48];
  float fRainInt[48];
  int fWindSpd[48];
  int fWindDir[48];
  int fCloudCover[48];
};

class DarkSkyAPI
{
  public:
    DarkSkyAPI(HTTPClient& http);
    bool queryWeather(const char* location);
    WeatherForecast readWeather();
    String printWeather();
    DataListener listener;
    uint32_t UTCmidnightTime = 0;
    struct Location
    {
      static const char* Kiel;
    };
  private:
    JsonStreamingParser parser;
    WeatherForecast parseDarkSkyString(String jsonStr);
    bool httpsQueryOnAPI(const char* location, uint32_t startTime);
    const int _httpsPort = 443;
    const char* _apiLink = "/forecast/00112233445566778899aabbccddeeff/";
    const char* _apiHost = "api.darksky.net";
    const char* _apiHostFingerprint = "16 D2 B8 63 6A B7 05 26 96 CF BD 55 79 2C 52 24 2D 51 2B 3A";
    const char* _apiOptions = "?lang=de&units=ca&exclude=minutely,daily,alerts,flags";
    WeatherForecast _actualWeather;
    HTTPClient& _http;
};


#endif

