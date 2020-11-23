/* How to convert bitmap to XBM format and display image on TTGO T-Watch ith TFT_eSPI library
 * Arduino IDE or PlatformIO
 * 
 * Step by step tutorials
 * https://diyprojects.io/ttgo-t-watch-display-xbm-tft_espi-lvgl-images-esp32-arduino/
 * https://projetsdiy.fr/ttgo-t-watch-afficher-images-xbm-tft_espi-lvgl-esp32-arduino/
 * Licence : see licence file
 * /
 
/* Arduino IDE - uncomment your watch */
//#define LILYGO_WATCH_2019_WITH_TOUCH
//#define  LILYGO_WATCH_2019_NO_TOUCH
//#define LILYGO_WATCH_BLOCK
//#define LILYGO_WATCH_2020_V1

/* PlatformIO -> Select your watch in platformio.ini file */
#include <Arduino.h>
#include <LilyGoWatch.h>
#include "clock.h"
#include "clockico.h"

// C++ object which will allow access to the functions of the Watch | Objet C++ qui permettra d'accéder aux fonctions du boitier
TTGOClass *ttgo;

void setup() {
  Serial.begin(115200);
  // Get ttgp object and set up the display | Récupère l'objet et allume l'écran
  ttgo = TTGOClass::getWatch();
  ttgo->begin();    
  ttgo->openBL();

  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextSize(2);
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->drawString("XBM image", 0,0);
  //drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bgcolor)
  ttgo->tft->drawXBitmap(20,20, clockxbm, clock_width, clock_height, TFT_WHITE, TFT_BLACK);

  ttgo->tft->pushImage(20,20, clockwidth, clockheight, clockico);
}

void loop() {}
