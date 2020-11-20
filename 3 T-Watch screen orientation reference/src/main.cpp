/* Get Started with the TFT_eSPI library for TTGO T-Watch
 * Arduino IDE or PlatformIO
 * Step by step tutorials
 * https://diyprojects.io/ttgo-t-watch-get-started-esp32-tft_espi-library/#changeorientation
 * https://projetsdiy.fr/ttgo-t-watch-debuter-librairie-esp32-tft_espi/#changeorientation
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

// C++ object which will allow access to the functions of the Watch | Objet C++ qui permettra d'accéder aux fonctions du boitier
TTGOClass *ttgo;
// C++ object that wil contain TFT_eSPI methods for quick access | Objet C++ TFT_eSPI permettant un accès rapide aux fonctions
TFT_eSPI *tft;

int w, h, orientation; // Screen resolution and orientation | résolution et orientation de l'écran
int p = 5 ;  // padding | marge

// Prototype
void drawReference();

void setup() {
  Serial.begin(115200);
  // Get ttgp object and set up the display | Récupère l'objet et allume l'écran
  ttgo = TTGOClass::getWatch();
  ttgo->begin();     
  ttgo->openBL();

  // Easy access to TFT library functions | Accès facile aux fonctions de la librairie TFT 
  tft = ttgo->tft;
  
  // Get Screen resolution | récupère le résolution de l'écran
  w = tft->width();     // width  | largeur
  h = tft->height();    // height | hauteur

  // Current screen orientation | orientation de l'écran
  orientation = tft->getRotation();
  drawReference();
}

void loop() {
  int16_t x, y;
  if (ttgo->getTouch(x, y)) {
    if ( orientation == 3 ) {
      orientation = 0;
    } else {
      orientation += 1;
    }
    tft->setRotation(orientation);
    Serial.printf("Change screen orientation %u \n", orientation);
    drawReference();
    delay(100);
  }
}

void drawReference(){
    // Clean Screen  | Efface l'écran
  tft->fillScreen(TFT_BLACK);  // Fill background in black | colorie le fond de l'écran en noir
  tft->setTextFont(1);
  tft->setTextSize(2);
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->setCursor(0, 0);

  // Draw reference | dessine le référentiel X/Y
  tft->drawFastHLine(p,p, w - 2*p, TFT_WHITE);
  tft->drawFastVLine(p,p, h - 2*p, TFT_WHITE);

  // Draw X and Y | Affiche les lettres X et Y (en utilisant le code ascii)
  // Use http://www.asciitable.com/ (for example) to find ascii char code
  // drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
  tft->drawChar(w - 3*p, 3*p, 88, TFT_WHITE, TFT_BLACK, 2); // X
  tft->fillTriangle(w, p, w - 2*p, 0, w - 2*p, 2*p, TFT_RED ); // Triangle -> X arrow | flèche X
  tft->drawChar(3*p, w - 3*p, 89, TFT_WHITE, TFT_BLACK, 2);  // Y
  tft->fillTriangle(p, h, 0, h - 2*p, 2*p, h - 2*p, TFT_RED ); // Y arraw | flèche Y
  // Origine
  tft->fillCircle(p,p,p,TFT_RED);

  // Indicate user to touch screen to rotate || 
  tft->setTextDatum(MC_DATUM); 
  tft->drawString("Touch Screen", w / 2, h / 2 - 10, 1);
  tft->drawString("to rotate", w / 2, h / 2 + 10, 1);
  tft->drawString(String(orientation), w / 2, h / 3 + 10, 1);
}