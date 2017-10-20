/**
  KVG live API Library
  Name: KVGliveAPI.cpp
  Purpose: Fetch and parse data from KVG online API (For ESP8266)

  @author basti8909
  @version 1.0 16/08/17
*/

#include "Arduino.h"
#include "KVGliveAPI.h"

// Public functions

KVGliveAPI::KVGliveAPI(HTTPClient& http): _http(http)
{
}

bool KVGliveAPI::queryBusStop(int stopNr)
{
  String apiAddress = String(apiLink) + String(stopNr);

  _http.begin(apiAddress);
  
  // start connection and send HTTP header
  int httpCode = _http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) 
  {
    // file found at server
    if(httpCode == HTTP_CODE_OK) 
    {
      payload = _http.getString();
      // Read JSON String and write values into "actualStop"
      _actualStop = parseKVGbusString(payload);
    }        
  }
  return (httpCode == HTTP_CODE_OK);
}

KVGbusStop KVGliveAPI::readBusStop()
{
  return _actualStop;
}

String KVGliveAPI::printBusStop()
{
  String busStr;
  busStr += _actualStop.stopName + ":\n";
  for (int row=0; row < 6; row++)
  {
    if (_actualStop.departures[row].number.length() > 0)
    {
      busStr += _actualStop.departures[row].number + " ";
      busStr += _actualStop.departures[row].dir + " ";
      busStr += "(" + String(_actualStop.departures[row].depMinutes) + " min)";
      busStr += "\n";      
    }
  }
  return busStr;
}

// Private functions

KVGbusStop KVGliveAPI::parseKVGbusString(String jsonStr)
{
  KVGbusStop newStop;

  // use large buffer for json decoding (2000 should have been enough for normal bus stops)
  const size_t bufferSize = 10000;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // parse JSON return from KVG
  JsonObject& root = jsonBuffer.parseObject(jsonStr);

  // fetch array with bus departures
  JsonArray& actual = root["actual"];
  // Read name of bus stop
  newStop.stopName = root["stopName"].as<String>();
  
  uint16_t row = 0;          

  // iterate through bus departure array and print each line to tft
  for(JsonArray::iterator it=actual.begin(); it!=actual.end(); ++it) 
  {
      JsonObject& actual0 = *it;
      // Fill values into BusStop struct
      if (row < 6) 
      {
          newStop.departures[row].number = actual0["patternText"].as<String>();
          newStop.departures[row].dir = actual0["direction"].as<String>();
          newStop.departures[row].depTime = actual0["plannedTime"].as<String>();
          //const char* actual_status = actual0["status"]; // "PREDICTED"
          //Serial.println(actual_status);
          if (strcmp(actual0["status"],"PREDICTED") == 0) 
          {
            uint8_t len = strlen(actual0["mixedTime"]);
            char minLeft[6];
            strncpy (minLeft,actual0["mixedTime"],len-11);
            newStop.departures[row].depMinutes = atoi(minLeft);
            newStop.departures[row].depDelay = calcDelay(actual0["plannedTime"],actual0["actualTime"]);
          }
          else
          {
            newStop.departures[row].depMinutes = -2;
            newStop.departures[row].depDelay = 0;
          }
      }
      row++;
  }
  for (row; row < 6; row++)
  {
      newStop.departures[row].number = String();
      newStop.departures[row].dir = String();
      newStop.departures[row].depTime = String();
      newStop.departures[row].depMinutes = 0;
      newStop.departures[row].depDelay = 0;
  }
  return newStop;
}

int KVGliveAPI::calcDelay(const char* planned, const char* actual)
{
  char plH[3], plM[3], acH[3], acM[3];
  plH[0] = planned[0];plH[1] = planned[1];
  plM[0] = planned[3];plM[1] = planned[4];
  acH[0] = actual[0];acH[1] = actual[1];
  acM[0] = actual[3];acM[1] = actual[4];
  plH[2] = plM[2] = acH[2] = acM[2] = '\x00';
  uint16_t tP = 60 * atoi(plH) + atoi(plM);   
  uint16_t tA = 60 * atoi(acH) + atoi(acM);   
  return tA - tP;
}


