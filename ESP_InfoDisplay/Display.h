/**
  Library header for showing data on the tft display
  Name: Display.h
  Purpose: Display data on ILI9341 compatible 2.8" display (320x240px) with XPT2046 touch interface.

  @author basti8909
  @version 1.0 16/08/17
*/

#ifndef Display_h
#define Display_h

#define TFT_CBLUE   0x04FB      /* 0, 159, 218 */

#include "Arduino.h"
#include <String.h>
#include <TimeLib.h>

// Grafik und Schrift für ILI9341 Displays
#include <TFT_eSPI.h>
// Touch funktion des Displays mit XPT2046 Controller
#include <XPT2046.h>
// KVG live interface
#include "KVGliveAPI.h"
// DarkSky interface
#include "DarkSkyAPI.h"
// Display classes
#include "icons.h"


typedef struct TimeOfDay {uint8_t hh;  uint8_t mm;  uint8_t ss;};

class Base
{
  public:
    Base();
    Base(TFT_eSPI *Disp, XPT2046 *Touch);
    TFT_eSPI * getDisplay();
    XPT2046 * getTouch();
    void initDispAndTouch();
    void clearScreen();
    void drawXbm(int x, int y, int width, int height, const char *xbm, uint32_t color);
    void drawCenterXbm(int width, int height, const char *xbm, uint32_t color);
    void drawLogoSpinner(int count, const char *logo, int lw, int lh);
    void drawWeatherIcon(int x, int y, String weatherIcon, uint32_t color);

  private:  
    const char* getIconFromString(String icon);

  protected:
    TFT_eSPI *_Disp;
    XPT2046  *_Touch;
};


class Clock : public Base
{
  public:
    Clock(Base * _b);
    void updateTime(TimeOfDay newTime);
    void setReDrawFlag();
    void Draw();     
    static const int _xleft = 227;
    static const int _ytop = 4;

  private:
    TimeOfDay _time;
    bool _ch,_cm,_cs;
    bool _reDraw;

  protected:
    Base * B;
    TFT_eSPI *_Disp  = B->getDisplay(); 
    XPT2046  *_Touch = B->getTouch();
    bool State;
};


class BusScreen : public Base
{
  public:
    BusScreen(Base * _b);
    void updateBusStop(KVGbusStop newStop);
    void setReDrawFlag();
    void Draw();

  private:
    void DrawHeader(); 
    KVGbusStop _busStop;
    KVGbusStop _newStop;
    bool _reDraw;
    static const int Y_HEAD = 34;
    static const int Y_LIST = 68;
    static const int ROW_HEIGHT = 30;
    static const uint16_t TFT_GREY = 0x5AEB;

  protected:
    Base * B;
    TFT_eSPI *_Disp  = B->getDisplay(); 
    XPT2046  *_Touch = B->getTouch();
    bool State;
};


class WeatherScreen : public Base
{
  public:
    WeatherScreen(Base * _b);
    void updateWeather(WeatherForecast newWeather);
    void setReDrawFlag();
    void Draw();

  private:
    void DrawGraphBG(int x,int y,int w,int h, float ymin, float ymax, float ystep, String description, bool showXscale);
    void DrawTempGraph(int x,int y,int w,int h, float data[]);
    void DrawRainGraph(int x,int y,int w,int h, float rInt[], float rProb[]);
    void DrawWindGraph(int x,int y,int w,int h, int wDir[], int wSpd[]);
    void DrawWindArrow(int x, int y, float rad, int dir, uint32_t color);
    bool _reDraw;
    WeatherForecast _actualWeather;

  protected:
    Base * B;
    TFT_eSPI *_Disp  = B->getDisplay(); 
    XPT2046  *_Touch = B->getTouch();
    bool State;
};

#endif

