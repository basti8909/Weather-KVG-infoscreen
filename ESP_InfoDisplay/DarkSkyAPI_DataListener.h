/**
  DarkSky.net Data Listener Library Header
  Name: DarkSkyAPI_DataListener.h
  Purpose: Data Listener for JSON parser library

  @author basti8909
  @version 1.0 16/08/17
*/

#pragma once

#include <JsonListener.h>

class DataListener: public JsonListener {

  private:
    String currentKey;
    String currentParent = "";
    uint32_t currentTime;
    String desc;
    String icon;
    uint8_t timezone;
    double temperature;
    uint8_t windSpd;
    uint16_t windDir;
    int limit = 25;
    long future_time[24];      // Array to store the time of future forecasts
    float future_temp[24];
    uint8_t future_rainProb[24];
    float future_rainInt[24];
    uint8_t future_windSpd[24];
    uint16_t future_windDir[24];
    uint8_t future_cloudCover[24];
    bool enterArray = false;

  public:
    int index = 0;
    virtual void whitespace(char c);
    virtual void startDocument();
    virtual void key(String key);
    virtual void value(String value);
    virtual void endArray();
    virtual void endObject();
    virtual void endDocument();
    virtual void startArray();
    virtual void startObject();

    // Functions to retrieve parsed data
    String getDesc();
    String getIcon();
    uint32_t getCurrentTime();
    uint8_t getTimezone();
    double getTemperature();
    uint8_t getWindSpd();
    uint16_t getWindDir();
    float getFutureTemp(int i);
    uint8_t getFutureRainProb(int i);
    float getFutureRainInt(int i);
    uint8_t getFutureWindSpd(int i);
    uint16_t getFutureWindDir(int i);
    uint8_t getFutureCloudCover(int i);
};
