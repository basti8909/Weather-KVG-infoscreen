/**
  DarkSky.net Data Listener Library
  Name: DarkSkyAPI_DataListener.cpp
  Purpose: Data Listener for JSON parser library

  @author basti8909
  @version 1.0 16/08/17
*/

#include "DarkSkyAPI_DataListener.h"
#include <JsonListener.h>

#define PARSER_DEBUG false  //Set to true to view parsed data via serial output. WARNING: may cause major latency if baud rate is low!

void DataListener::whitespace(char c) {
  if(PARSER_DEBUG) Serial.println("whitespace");
}

void DataListener::startDocument() {
  if(PARSER_DEBUG) Serial.println("start document");
}

void DataListener::key(String key) {
  if(PARSER_DEBUG) Serial.println("key: " + key);
  currentKey = String(key);
}

void DataListener::value(String value) {
  if(PARSER_DEBUG) Serial.println("value: " + value);

  if(currentKey == "offset")
    timezone = value.toInt();

  if(currentParent == "currently")
  { 
    if(currentKey == "time")
      currentTime = value.toInt();
      
    if(currentKey == "temperature")
      temperature = value.toFloat();

    if(currentKey == "windSpeed")
      windSpd = value.toInt();
      
    if(currentKey == "windBearing")
      windDir = value.toInt();

    if(currentKey == "icon")
      icon = value;
  }
  if(currentParent == "hourly")
  { 
    if(currentKey == "summary")
      desc = value;
  }

  if(enterArray)
  {
      if(index < limit) // Skip the first hourly forecast (the same hour as the current hour)
      {
          if(currentKey == "time")
          future_time[index] = value.toInt();
          
          if(currentKey == "temperature")
          future_temp[index] = value.toFloat();
          
          if(currentKey == "precipProbability")
          future_rainProb[index] = (value.toFloat()*100);
          
          if(currentKey == "precipIntensity")
          future_rainInt[index] = value.toFloat();
          
          if(currentKey == "windSpeed")
          future_windSpd[index] = value.toInt();
          
          if(currentKey == "windBearing")
          future_windDir[index] = value.toInt();

          if(currentKey == "cloudCover")
          future_cloudCover[index] = (value.toFloat()*100);
      }
   }
  
}

void DataListener::endArray() {
  if(PARSER_DEBUG) Serial.println("end array. ");
  enterArray = false;
  index = 0;
}

void DataListener::endObject() {
  if(PARSER_DEBUG) Serial.println("end object. ");
  if(!enterArray) currentParent = "";
  else index++;
}

void DataListener::endDocument() {
  if(PARSER_DEBUG) Serial.println("end document. ");
}

void DataListener::startArray() {
   if(PARSER_DEBUG) Serial.println("start array. ");
   enterArray = true;
}

void DataListener::startObject() {
   if(PARSER_DEBUG) Serial.println("start object. ");
   currentParent = currentKey;
}

String DataListener::getDesc() {
    return desc;
}

String DataListener::getIcon() {
    return icon;
}

uint32_t DataListener::getCurrentTime(){
    return currentTime;
}

uint8_t DataListener::getTimezone() {
    return timezone;
}

double DataListener::getTemperature() {
    return temperature;
}

uint8_t DataListener::getWindSpd() {
    return windSpd;
}

uint16_t DataListener::getWindDir() {
    return windDir;
}

float DataListener::getFutureTemp(int i) {
    return future_temp[i];
}

uint8_t DataListener::getFutureRainProb(int i) {
    return future_rainProb[i];
}

float DataListener::getFutureRainInt(int i) {
    return future_rainInt[i];
}

uint8_t DataListener::getFutureWindSpd(int i) {
    return future_windSpd[i];
}

uint16_t DataListener::getFutureWindDir(int i) {
    return future_windDir[i];
}

uint8_t DataListener::getFutureCloudCover(int i) {
    return future_cloudCover[i];
}
