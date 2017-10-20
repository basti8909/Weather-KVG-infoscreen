/**
  DarkSky.net API Library
  Name: DarkSkyAPI.cpp
  Purpose: Fetch and parse data from DarkSky online API (For ESP8266)

  @author basti8909
  @version 1.0 16/08/17
*/

#include "Arduino.h"
#include "DarkSkyAPI.h"

// Public functions


const char* DarkSkyAPI::Location::Kiel = "54.3422,10.1375"; //Kiel, Blücherplatz

DarkSkyAPI::DarkSkyAPI(HTTPClient& http): _http(http)
{
}

bool DarkSkyAPI::queryWeather(const char* location)
{
  httpsQueryOnAPI(location, 0);
  _actualWeather.location = "Kiel";
  _actualWeather.actDesc = listener.getDesc();
  _actualWeather.actIco = listener.getIcon();
  _actualWeather.actTemp = listener.getTemperature();
  _actualWeather.actWindSpd = listener.getWindSpd();
  _actualWeather.actWindDir = listener.getWindDir();
  int timezone = listener.getTimezone();
  // If NTP time fails, use current weather timestamp
  if (UTCmidnightTime < 86400) 
  {
    UTCmidnightTime = listener.getCurrentTime();
    if (timeStatus() == timeNotSet) setTime(listener.getCurrentTime()+3600*timezone);
    }
  
  
  int row,i;
  for (i = 0; i<2; i++)
  {
    httpsQueryOnAPI(location, UTCmidnightTime+24*i*3600);
    for (row=0; row < 24; row++)
    {
      _actualWeather.fTemp[row+24*i] = listener.getFutureTemp(row);
      _actualWeather.fRainProb[row+24*i] = listener.getFutureRainProb(row);
      _actualWeather.fRainInt[row+24*i] = listener.getFutureRainInt(row);
      _actualWeather.fWindSpd[row+24*i] = listener.getFutureWindSpd(row);
      _actualWeather.fWindDir[row+24*i] = listener.getFutureWindDir(row);
      _actualWeather.fCloudCover[row+24*i] = listener.getFutureCloudCover(row);
    }
  }
  
  if (DEBUG_DarkSkyAPI) Serial.println(printWeather());
}

bool DarkSkyAPI::httpsQueryOnAPI(const char* location, uint32_t startTime)
{
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  if (DEBUG_DarkSkyAPI) Serial.print("connecting to ");
  if (DEBUG_DarkSkyAPI) Serial.println(_apiHost);
  if (!client.connect(_apiHost, _httpsPort)) {
    Serial.println("connection failed");
    return false;
  }
  if (DEBUG_DarkSkyAPI) {
    if (client.verify(_apiHostFingerprint, _apiHost)) {
      Serial.println("certificate matches");
    } else {
      Serial.println("certificate doesn't match");
    }
  }
  String sTime = "", excl = ",hourly";
  if (startTime > 0) 
  {
    sTime = "," + String(startTime);
    excl = "";
  }
  
  String apiAddress = String(_apiLink) + String(location) + sTime + "/" + String(_apiOptions) + excl;
  if (DEBUG_DarkSkyAPI) Serial.print("requesting URL: ");
  if (DEBUG_DarkSkyAPI) Serial.println(apiAddress);

  client.print(String("GET ") + apiAddress + " HTTP/1.1\r\n" +
               "Host: " + _apiHost + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  if (DEBUG_DarkSkyAPI) Serial.println("request sent");
  if (DEBUG_DarkSkyAPI) Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
  String line;
  while (client.connected()) {
    line = client.readStringUntil('\n');
    if (DEBUG_DarkSkyAPI) Serial.println(line);
    if (line == "\r") {
      if (DEBUG_DarkSkyAPI) Serial.println("- headers received -");
      break;
    }
  }
  if (DEBUG_DarkSkyAPI) Serial.println("Start getting data");


  if (DEBUG_DarkSkyAPI) Serial.println(String(ESP.getFreeHeap()));
  parser.setListener(&listener);

  boolean isBody = false;
  char c;
  int size = 0, index = 0;
  if (DEBUG_DarkSkyAPI) Serial.print("Reading and parsing data from api.darksky.net...");

  // Parse the JSON data retrieved from the server
  while(client.connected()) {
    while((size = client.available()) > 0) {
      c = client.read();
      if (c == '{' || c == '[') {
        isBody = true;
      }
      if (isBody) {
        parser.parse(c);
      }
    }
  }
  if (DEBUG_DarkSkyAPI) Serial.println(" Done.");
  if (DEBUG_DarkSkyAPI) Serial.println(String(ESP.getFreeHeap()));
  
  // Reset the parser so that it starts from the beginning the next time around
  parser.reset();
}

WeatherForecast DarkSkyAPI::readWeather()
{
  return _actualWeather;
}

String DarkSkyAPI::printWeather()
{
  String wStr;
  wStr += _actualWeather.location + ": " + _actualWeather.actDesc + " (" + String(_actualWeather.actTemp) + "°C, " + String(_actualWeather.actWindSpd) + "km/h aus " + String(_actualWeather.actWindDir) + "°)" + ":\n";
  for (int row=0; row < 48; row++)
  {
    wStr += String(row) + "h:\t";
    wStr += String(_actualWeather.fTemp[row]) + "°C,\t";
    wStr += String(_actualWeather.fWindSpd[row]) + "km/h,\t";
    wStr += String(_actualWeather.fWindDir[row]) + "°,\t";
    wStr += String(_actualWeather.fRainProb[row]) + "%\t";
    wStr += String(_actualWeather.fRainInt[row]) + "mm,\t";
    wStr += "\n";   
  }
  return wStr;
}



