/**
  KVG live API Library Header
  Name: KVGliveAPI.h
  Purpose: Fetch and parse data from KVG online API (For ESP8266)

  @author basti8909
  @version 1.0 16/08/17
*/

#ifndef KVGliveAPI_h
#define KVGliveAPI_h

#include "Arduino.h"
#include <String.h>
// JSON parser library
#include <ArduinoJson.h>
// WIFI connection for ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

typedef struct KVGbus 
{
  String number;
  String dir;
  String depTime;
  int8_t depMinutes;
  int8_t depDelay;
};

typedef struct KVGbusStop
{
  String stopName;
  KVGbus departures[6];
};

class KVGliveAPI
{
  public:
    KVGliveAPI(HTTPClient& http);
    bool queryBusStop(int stopNr);
    KVGbusStop readBusStop();
    String printBusStop();
    String payload;
  	struct KVGstop
  	{
  		static const int Wrangelstrasse = 105;
  		static const int Schauspielhaus = 150;
      static const int WaitzHoltenauer = 25;
  		static const int BotanischerGarten = 268;
  		static const int HbfA1 = 9001;
  		static const int HbfA2 = 9002;
  		static const int HbfB1 = 9004;
  		static const int HbfB2 = 9009;
  		static const int HbfB3 = 9008;
  		static const int HbfC1 = 8001;
  		static const int HbfC2 = 8002;
  		static const int HbfC3 = 8008;
  		static const int HbfD1 = 8004;
  		static const int HbfD2 = 8005;
  		static const int HbfD3 = 8007;
  		// Weitere Stops unter: http://kvg-kiel.de/internetservice/services/lookup/autocomplete?query=Haupt
  		// Hinter query entsprechenden Eintrag ändern und im Quelltext nachsehen
  	};
	
  private:
    KVGbusStop parseKVGbusString(String jsonStr);
  	int calcDelay(const char* planned, const char* actual);
  	const char* apiLink = "http://kvg-kiel.de/internetservice/services/passageInfo/stopPassages/stop?mode=departure&stop=";
	KVGbusStop _actualStop;
  	HTTPClient& _http;
};

#endif

