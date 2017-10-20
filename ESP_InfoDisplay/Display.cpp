/**
  Library for showing data on the tft display
  Name: Display.cpp
  Purpose: Display data on ILI9341 compatible 2.8" display (320x240px) with XPT2046 touch interface.

  @author basti8909
  @version 1.0 16/08/17
*/

#include "Arduino.h"
#include "Display.h"

// Public functions


Base::Base() {}
Base::Base(TFT_eSPI *Disp, XPT2046 *Touch): _Disp(Disp), _Touch(Touch) {}

TFT_eSPI * Base::getDisplay() 
{
  return _Disp;
}

XPT2046 * Base::getTouch() 
{
  return _Touch;
}

void Base::initDispAndTouch()
{
  _Disp->init();
  _Disp->setRotation(1);
  
  _Touch->begin(240, 320);
  _Touch->setRotation(_Touch->ROT90);
  _Touch->setCalibration(1845,283,274,1807);

  SPI.setFrequency(40000000);
  clearScreen();
}

void Base::clearScreen()
{  
  _Disp->fillScreen(TFT_BLACK);
}

void Base::drawXbm(int x, int y, int width, int height, const char *xbm, uint32_t color) {
  if (width % 8 != 0) {
    width =  ((width / 8) + 1) * 8;
  }
  for (int i = 0; i < width * height / 8; i++ ){
    unsigned char charColumn = pgm_read_byte(xbm + i);
    for (int j = 0; j < 8; j++) { 
      int targetX = (i * 8 + (7-j)) % width + x;
      int targetY = (8 * i / (width)) + y;
      if (bitRead(charColumn, j)) {
        _Disp->drawPixel(targetX, targetY,color);
      }
    }
  }    
}
void Base::drawCenterXbm(int width, int height, const char *xbm, uint32_t color) {
  int x = _Disp->width()/2-width/2;
  int y = _Disp->height()/2-height/2;
  
  drawXbm(x, y, width, height, xbm, color);  
}

void Base::drawLogoSpinner(int count, const char *logo, int lw, int lh) {
  if (count == 0) drawXbm(_Disp->width()/2-lw/2,_Disp->height()/2-lh/2, lw, lh, logo,TFT_WHITE);
  for (int i = 0; i < 3; i++) {
    //const char *xbm;
    int x = _Disp->width()/2 + 16 * (i-1);
    int y = _Disp->height()*8/10;
    _Disp->fillRect(x-6, y-6, 12, 12, TFT_BLACK);
    if (count % 3 == i) {
       _Disp->fillCircle(x,y,5,TFT_LIGHTGREY);
    } else { 
       _Disp->drawCircle(x,y,5,TFT_LIGHTGREY);       
    }    
  }   
}

void Base::drawWeatherIcon(int x, int y, String weatherIcon, uint32_t color)
{
  drawXbm(x, y, 80, 80, getIconFromString(weatherIcon), color);
}


const char* Base::getIconFromString(String icon) {
  if (icon == "clear-day")    return wIco_clear_day_bits;
  if (icon == "clear-night")    return wIco_clear_night_bits;
  if (icon == "rain")    return wIco_rain_bits;
  if (icon == "snow")    return wIco_snow_bits;
  if (icon == "sleet")    return wIco_sleet_bits;
  if (icon == "wind")    return wIco_wind_bits;
  if (icon == "fog")    return wIco_fog_bits;
  if (icon == "cloudy")    return wIco_cloudy_bits;
  if (icon == "partly-cloudy-day")    return wIco_partly_cloudy_day_bits;
  if (icon == "partly-cloudy-night")    return wIco_partly_cloudy_night_bits;
  if (icon == "hail")    return wIco_hail_bits;
  if (icon == "thunderstorm")    return wIco_thunderstorm_bits;  
  else return wIco_partly_cloudy_day_bits;
}



Clock::Clock(Base * _b): B(_b) 
{
  setReDrawFlag();
}

void Clock::updateTime(TimeOfDay newTime)
{
  _ch = (newTime.hh != _time.hh);
  _cm = (newTime.mm != _time.mm);
  _cs = (newTime.ss != _time.ss);
  _time = newTime;
}

void Clock::setReDrawFlag()
{
  _reDraw = true;
}

void Clock::Draw()
{
  // digital clock display of the time
  _Disp->setTextColor(TFT_WHITE);
  _Disp->setFreeFont(&FreeSans12pt7b);  
  
  if (_ch || _reDraw)
  {
    String sh = String(_time.hh);
    if (sh.length() == 1) sh = "0" + sh;
    _Disp->fillRect(_xleft, _ytop, 32, 30, TFT_BLACK);
    _Disp->drawCentreString(sh, _xleft+16, _ytop, 1);
  }
  if (_cm || _reDraw)
  {
    String sm = String(_time.mm);
    if (sm.length() == 1) sm = "0" + sm;
    _Disp->fillRect(_xleft+32, _ytop, 32, 30, TFT_BLACK);
    _Disp->drawCentreString(sm, _xleft+48, _ytop, 1);
  }
  if (_cs || _reDraw)
  {
    String ss = String(_time.ss);
    if (ss.length() == 1) ss = "0" + ss;
    _Disp->fillRect(_xleft+64, _ytop, 32, 30, TFT_BLACK);
    _Disp->drawCentreString(ss, _xleft+80, _ytop, 1);
  }      
  
  if (_time.ss % 2) _Disp->setTextColor(TFT_BLACK);
  _Disp->drawCentreString(":", _xleft+32, _ytop, 1);
  _Disp->drawCentreString(":", _xleft+64, _ytop, 1);

  _reDraw = false;
}


BusScreen::BusScreen(Base * _b): B(_b) 
{
  setReDrawFlag();
}

void BusScreen::updateBusStop(KVGbusStop newStop)
{
  _newStop = newStop;
}

void BusScreen::setReDrawFlag()
{
  _reDraw = true;
}

void BusScreen::DrawHeader()
{
  _Disp->fillRect(0, Y_HEAD, 320, 24, TFT_BLACK);
  _Disp->setTextColor(TFT_WHITE);
  _Disp->drawString("Linie", 6, Y_HEAD, 2);
  _Disp->drawFastVLine(0, Y_HEAD, 24, TFT_WHITE);
  _Disp->drawString("Richtung", 50, Y_HEAD, 2);
  _Disp->drawFastVLine(44, Y_HEAD, 24, TFT_WHITE);
  _Disp->drawString("Abfahrt", 250, Y_HEAD, 2);
  _Disp->drawFastVLine(244, Y_HEAD, 24, TFT_WHITE);
}

void BusScreen::Draw()
{
  if (_reDraw) DrawHeader();
  
  _newStop.stopName.replace("ß","ss");
  // Overvwrite bus number if there are any changes
  if (_newStop.stopName != _busStop.stopName || _reDraw)
  {
      _Disp->setTextColor(TFT_WHITE);
      _Disp->setFreeFont(&FreeSans12pt7b);  
      _Disp->fillRect(0, 0, Clock::_xleft-1, Y_HEAD-1, TFT_BLACK);
      _busStop.stopName = _newStop.stopName;
      _Disp->drawString(_busStop.stopName, 0, 4, 1);
  }

  
  _Disp->setTextColor(TFT_ORANGE);
  _Disp->setFreeFont(&FreeMonoBold9pt7b);
  uint16_t ypos = 0;

  for (int row=0; row < 6; row++)
  {
    ypos = row * ROW_HEIGHT + Y_LIST;
    _Disp->setTextDatum(TL_DATUM);

    // Overvwrite bus number if there are any changes
    if ((_newStop.departures[row].number != _busStop.departures[row].number) || _reDraw)
    {
        _Disp->fillRect(0, ypos, 44, ROW_HEIGHT, TFT_BLACK);
        _busStop.departures[row].number = _newStop.departures[row].number;
        if (_busStop.departures[row].number.length() != 0)
          _Disp->drawCentreString(_busStop.departures[row].number, 20, ypos, 1);
    }

    // Overvwrite bus destination if there are any changes
    if ((_newStop.departures[row].dir != _busStop.departures[row].dir) || _reDraw)
    {
        _Disp->fillRect(44, ypos, 244-44, ROW_HEIGHT, TFT_BLACK);
        _busStop.departures[row].dir = _newStop.departures[row].dir;
        if (_busStop.departures[row].dir.length() != 0)
          _Disp->drawString(_busStop.departures[row].dir, 50, ypos, 1);
    }


    // Overvwrite departure time if there are any changes
    if ((_newStop.departures[row].depMinutes != _busStop.departures[row].depMinutes) || (_newStop.departures[row].depTime != _busStop.departures[row].depTime) || _reDraw)
    {
        _Disp->fillRect(244, ypos, 320-244, ROW_HEIGHT, TFT_BLACK);
        _busStop.departures[row].depTime    = _newStop.departures[row].depTime;
        _busStop.departures[row].depMinutes = _newStop.departures[row].depMinutes;
        _busStop.departures[row].depDelay   = _newStop.departures[row].depDelay;
        if ((_busStop.departures[row].depMinutes >= -1) && (_busStop.departures[row].number.length() != 0))
        {
          String minStr = String(_busStop.departures[row].depMinutes);
          //char buf[4],bufMin[4],bufDelay[5];
          //itoa(_busStop.departures[row].depMinutes, bufMin, 10);
          if (_busStop.departures[row].depDelay != 0)
          {
            String delStr = "+" + String(_busStop.departures[row].depDelay);
            //itoa(actualStop.departures[row].depDelay, buf, 10);
            //strcpy(bufDelay,"+");
            //strcat(bufDelay, buf);
            _Disp->setTextFont(0);        // Select font 0 which is the Adafruit font
            _Disp->setTextColor(TFT_RED);
            _Disp->setTextDatum(TL_DATUM);
            _Disp->drawString(delStr, 244, ypos);
            _Disp->setFreeFont(&FreeMonoBold9pt7b);
            _Disp->setTextColor(TFT_ORANGE);
          }
          _Disp->setTextDatum(TR_DATUM);
          _Disp->drawString("min", 320, ypos, 1);
          _Disp->drawString(minStr, 280, ypos, 1);
        }
        else
        {
          _Disp->drawCentreString(_busStop.departures[row].depTime, 282, ypos, 1);
        }
    }

    // Draw separation lines
    if ((_busStop.departures[row].number.length() != 0) && (row > 0))
      _Disp->drawFastHLine(0,ypos-8,360,TFT_GREY);
  }

  _reDraw = false;
}


WeatherScreen::WeatherScreen(Base * _b): B(_b){}

void WeatherScreen::updateWeather(WeatherForecast newWeather)
{
  _actualWeather = newWeather;
}

void WeatherScreen::setReDrawFlag()
{
  _reDraw = true;
}

void WeatherScreen::DrawGraphBG(int x,int y,int w,int h, float ymin, float ymax, float ystep, String description, bool showXscale = true)
{
  int i,x1,deltax,yt,lines,tempy;
  
  deltax = w/47;  

  // Graph background
  _Disp->fillRect(x, y, w, h, TFT_WHITE);
  _Disp->drawRect(x, y, w, h, TFT_LIGHTGREY);

  // Graph caption
  _Disp->setTextDatum(TR_DATUM);
  _Disp->setTextFont(0);
  _Disp->setTextColor(TFT_DARKGREY);
  _Disp->drawString(description, x+w-2, y+2);

  
  // Y-scale minimum and maximum
  _Disp->setTextDatum(CR_DATUM);
  _Disp->setTextFont(0);
  _Disp->setTextColor(TFT_WHITE);
  _Disp->drawString(String((int)ymax), x-2, y);
  _Disp->drawString(String((int)ymin), x-2, y+h);

  // Rest of Y-scale and horizontal lines
  lines = (ymax-ymin)/ystep;  
  tempy = (ystep/(ymax-ymin)) * h;
  for (i=1; i<lines; i++)
  {
    yt = y+h-i*tempy;
    _Disp->drawFastHLine(x,yt,w,TFT_LIGHTGREY);
    _Disp->drawString(String((int)(i*ystep+ymin)), x-2, yt);
  }

  x1 = x;
  
  _Disp->setTextDatum(TC_DATUM);
  for (i=1; i<48; i++)
  { 
    // Vertical lines every 12 hours 
    if (i % 12 == 0)
    {
      _Disp->drawFastVLine(x1,y,h,TFT_LIGHTGREY);
      // Thicker line at midnight of second day
      if (i == 24) {_Disp->drawFastVLine(x1-1,y,h,TFT_CBLUE);_Disp->drawFastVLine(x1,y,h,TFT_CBLUE);}
    }
    // X-scale every 6 hours
    if (i % 6 == 0 && showXscale)
    {
      if (i < 23) {_Disp->drawString(String(i), x1+1, y+h+4);}
      else        {_Disp->drawString(String(i-24), x1+1, y+h+4);}
    }
    // Green line at current hour
    if (i == hour())
    {
      _Disp->drawFastVLine(x1,y,h,TFT_GREEN);
      _Disp->drawFastVLine(x1+1,y,h,TFT_GREEN);
    }
      
    x1 = x1 + deltax;
  }  
  // X-scale at begin and end
  if (showXscale) _Disp->drawString("0", x, y+h+4);
  if (showXscale) _Disp->drawString("0", x+w, y+h+4);
  
  _Disp->setTextDatum(TL_DATUM);
}


void WeatherScreen::DrawTempGraph(int x,int y,int w,int h, float arr[])
{
  
  int i,l = sizeof(arr);
  float ymin=arr[0], ymax=arr[0];

  for (i=0; i<48; i++)
  {
    if (arr[i] < ymin) ymin = arr[i];
    if (arr[i] > ymax) ymax = arr[i];    
  }
  ymin = 5 * ((int)ymin / 5);
  if (ymin < 0) ymin -= 5;
  ymax = 5 * ((int)ymax / 5)+5;

  DrawGraphBG(x, y, w, h, ymin, ymax, 5, "Temperatur",false);
  
  int x1, y1, x2, y2;
  float tempy;
  int deltax = w/47;  
  x1 = x;
  tempy = ((arr[0]-ymin)/(ymax-ymin)) * h;
  y1 = y + h - tempy;
 
  for (int i=1; i<48; i++)
  {
    x2 = x1 + deltax;
    tempy = ((arr[i]-ymin)/(ymax-ymin)) * h;
    y2 = y + h - tempy;  
      
    _Disp->drawLine(x1, y1+1, x2, y2+1, TFT_BLACK);
    _Disp->drawLine(x1, y1, x2, y2, TFT_BLACK);
    _Disp->drawLine(x1, y1-1, x2, y2-1, TFT_BLACK);
      
    x1 = x2;
    y1 = y2;
  }  
}
void WeatherScreen::DrawRainGraph(int x,int y,int w,int h, float rInt[], float rProb[])
{
  
  int i;
  float ymin=0, ymax=0;

  for (i=0; i<48; i++)
  {
    if (rInt[i] > ymax) ymax = rInt[i];    
  }
  ymax = (int)ymax +1;

  DrawGraphBG(x, y, w, h, ymin, ymax, 1, "Regen (mm/h)", false);
  
  int x1, y1;
  float tempy;
  int deltax = w/47;  
  x1 = x;
 
  for (int i=0; i<48; i++)
  {
    tempy = ((rInt[i]-ymin)/(ymax-ymin)) * h;
    y1 = y + h - tempy;  
     //Serial.println("Liniev" + String(x1) + "," + String(y1) + "," + String(y) + "," + String(rInt[i]));
    _Disp->drawFastVLine(x1, y1, y+h-y1, TFT_BLUE);
    _Disp->drawFastVLine(x1+1, y1, y+h-y1, TFT_BLUE);
      
    x1 = x1 + deltax;
  }

  int x2, y2;
  ymax = 100; ymin = 0;
  x1 = x;
  tempy = ((rProb[0]-ymin)/(ymax-ymin)) * h;
  y1 = y + h - tempy;
 
  for (int i=1; i<48; i++)
  {
    x2 = x1 + deltax;
    tempy = ((rProb[i]-ymin)/(ymax-ymin)) * h;
    y2 = y + h - tempy;  
      
    _Disp->drawLine(x1, y1, x2, y2, TFT_BLACK);
      
    x1 = x2;
    y1 = y2;
  }  
}

void WeatherScreen::DrawWindGraph(int x,int y,int w,int h, int wDir[], int wSpd[])
{
  
  int i;
  int x1, y1, x2, y2;
  float tempy;
  int deltax = w/47; 
  float ymin=0, ymax=0;

  for (i=0; i<48; i++)
  {
    if (wSpd[i] > ymax) ymax = wSpd[i];    
  }
  ymax = 5 * ((int)ymax / 5)+5;

  DrawGraphBG(x, y, w, h, ymin, ymax, 10, "Wind (km/h)");

  x1 = x;
  tempy = ((wSpd[0]-ymin)/(ymax-ymin)) * h;
  y1 = y + h - tempy;
 
  for (int i=1; i<48; i++)
  {
    x2 = x1 + deltax;
    tempy = ((wSpd[i]-ymin)/(ymax-ymin)) * h;
    y2 = y + h - tempy;  
    float dy = (y1 - y2)/deltax;
    for (int j=0;j<deltax;j++)
      _Disp->drawFastVLine(x1+j, y1-dy*j, y+h-y1+dy*j, TFT_LIGHTGREY);
      
    x1 = x2;
    y1 = y2;
  }  
   
  x1 = x;
  y1 = y+h/2;
 
  for (int i=1; i<48; i++)
  {
    if (i % 3 == 0)
      DrawWindArrow(x1, y1, 6, wDir[i], TFT_BLACK);
      
    x1 = x1 + deltax;
  }
}

void WeatherScreen::DrawWindArrow(int x, int y, float rad, int dir, uint32_t color)
{
  float dx,dy,dxArr,dyArr;
  float theta;

  theta = (dir-180) * PI/ 180;
  dx = sin(theta)*rad;
  dy = cos(theta)*rad;

  _Disp->drawLine(x-dx, y+dy, x+dx, y-dy, color);

  theta = (dir+180-90) * PI/ 180;
  dxArr = sin(theta)*rad/3;
  dyArr = cos(theta)*rad/3;
  _Disp->drawLine(x+dx, y-dy, x-dxArr,y+dyArr,color);
  
  theta = (dir+180+90) * PI/ 180;
  dxArr = sin(theta)*rad/3;
  dyArr = cos(theta)*rad/3;
  _Disp->drawLine(x+dx, y-dy, x-dxArr,y+dyArr,color);
  
}


void WeatherScreen::Draw()
{
  _Disp->fillScreen(TFT_CBLUE);
  //_Disp->fillRect(0,0,110,240,TFT_BLACK);
  
  B->drawWeatherIcon(20, 40, _actualWeather.actIco, TFT_WHITE);
     
  _Disp->setFreeFont(&FreeSansBold9pt7b);   
  _Disp->setTextColor(TFT_WHITE); 
  _Disp->setTextDatum(TL_DATUM);
  int tX = 82, tY = 140;
  _Disp->drawString(" C", tX+6, tY, 1); 
  _Disp->drawCircle(tX+7, tY+3, 2, TFT_WHITE);        
  _Disp->setTextDatum(TR_DATUM);
  _Disp->setFreeFont(&FreeSansBold18pt7b); 
  _Disp->drawString(String(_actualWeather.actTemp,1), tX, tY, 1);
  _Disp->setTextDatum(TL_DATUM);
  
      
      
  DrawTempGraph(128,6,188,100,_actualWeather.fTemp);
  DrawRainGraph(128,116,188,50,_actualWeather.fRainInt,_actualWeather.fRainProb);
  DrawWindGraph(128,176,188,50,_actualWeather.fWindDir,_actualWeather.fWindSpd);


  _reDraw = false;
}

