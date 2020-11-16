/* Get Started with the TFT_eSPI library for TTGO T-Watch
 * Arduino IDE or PlatformIO
 * Step by step tutorials
 *
 * https://projetsdiy.fr/ttgo-t-watch-debuter-librairie-esp32-tft_espi/
 * Licence : see licence file
 * /
 
/* Arduino IDE - uncomment your watch */
//#define LILYGO_WATCH_2019_WITH_TOUCH
//#define  LILYGO_WATCH_2019_NO_TOUCH
//#define LILYGO_WATCH_BLOCK=1
//#define LILYGO_WATCH_2020_V1

/* PlatformIO -> Select your watch in platformio.ini file */
#include <Arduino.h>
#include <LilyGoWatch.h>

// C++ object which will allow access to the functions of the Watch | Objet C++ qui permettra d'accéder aux fonctions du boitier
TTGOClass *ttgo;
// C++ object that wil contain TFT_eSPI methods for quick access | Objet C++ TFT_eSPI permettant un accès rapide aux fonctions
TFT_eSPI *tft;

String txt; 
int w = 240;    // Screen width | Largeur de l'écran
int h = 240;    // Screen height | Hauteur de l'écran
int16_t x, y;
char buf[128];

/* Prototypes */
void fillScreenBackground();
void displayFontSize();
void drawBasic();
void drawStringDemo();
void screenOrientationDemo();
void printAListToScreen();

void setup() {
    Serial.begin(115200);
    // Get Watch object and set up the display | Récupère l'objet Watch et allume l'écran
    ttgo = TTGOClass::getWatch();
    ttgo->begin();     
    ttgo->openBL();
    tft = ttgo->tft;
    // Uncomment function you want to test | Dé-commenter les fonctions que vous voulez tester
    fillScreenBackground();
    displayFontSize();
    drawBasic();
    drawStringDemo();
    screenOrientationDemo();
    printAListToScreen();
    
    // Clean screen | efface l'écran 
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->setTextDatum(MC_DATUM);
}

void loop(){
  // Display cursor position each time the screen is touched | Affiche les coordonnées du curseur chaque fois que l'écran est touché
  if (ttgo->getTouch(x, y)) {
    sprintf(buf, "  x:%u  y:%u   ", x, y);
    tft->drawString(buf, w / 2, h / 2);
    Serial.println(buf);
  }
}

// Available colors
/* 
 * TFT_BLACK         0,   0,   0 
 * TFT_NAVY          0,   0, 128 
 * TFT_DARKGREEN     0, 128,   0 
 * TFT_DARKCYAN      0, 128, 128 
 * TFT_MAROON      128,   0,   0 
 * TFT_PURPLE      128,   0, 128 
 * TFT_OLIVE       128, 128,   0 
 * TFT_LIGHTGREY   211, 211, 211 
 * TFT_DARKGREY    128, 128, 128 
 * TFT_BLUE          0,   0, 255 
 * TFT_GREEN         0, 255,   0 
 * TFT_CYAN          0, 255, 255 
 * TFT_RED         255,   0,   0 
 * TFT_MAGENTA     255,   0, 255 
 * TFT_YELLOW      255, 255,   0 
 * TFT_WHITE       255, 255, 255 
 * TFT_ORANGE      255, 180,   0 
 * TFT_GREENYELLOW 180, 255,   0 
 * TFT_PINK        255, 192, 203     
 * TFT_BROWN       150,  75,   0 
 * TFT_GOLD        255, 215,   0 
 * TFT_SILVER      192, 192, 192 
 * TFT_SKYBLUE     135, 206, 235 
 * TFT_VIOLET      180,  46, 226 
*/
void fillScreenBackground(){
    Serial.println("Red");
    tft->fillScreen(TFT_RED);
    delay(1000);
    Serial.println("Green");
    tft->fillScreen(TFT_GREEN);
    delay(1000);
    Serial.println("Black");
    tft->fillScreen(TFT_BLACK);
    delay(1000);
}

void getScreenSize(){
  w = tft->width();
  h = tft->height();
  Serial.printf("Screen size %u * %u", w, h);
}

// Draw primitives | dessine des primitives géométriques
void drawBasic(){
  int margin = 20;
  int _delay = 500;
  getScreenSize();
  
  Serial.println("Draw a rectangle");
  tft->fillScreen(TFT_BLACK);
  tft->drawRect(margin,margin, w - ( 2* margin ), h - ( 2 * margin),TFT_RED);
  delay(_delay);

  Serial.println("Draw a filled rectangle");
  // fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
  tft->fillRect(2 * margin, 2 * margin, w - ( 4 * margin), h - (4 * margin), TFT_VIOLET);
  delay(_delay);

  Serial.println("Draw a rounded corner rectangle outline");
  //drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color)
  tft->drawRoundRect(3 * margin, 3 * margin, w - ( 6 * margin), h - (6 * margin), 8, TFT_YELLOW);
  delay(_delay);

  Serial.println("Draw a line");
  // drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
  tft->drawLine(0, 0, w, h, TFT_DARKGREY);
  delay(_delay);  

  Serial.println("Draw a triangle outline using 3 arbitrary points");
  // drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
  tft->drawTriangle(w,h - 1, w - 40 , h, w - 20, h - 30, TFT_PURPLE);
  delay(_delay); 

  Serial.println("Draw a circle outline");
  //drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color)
  tft->drawCircle(w / 2, h / 2, 80, TFT_SKYBLUE);
  delay(_delay);

  Serial.println("Draw a ellipse outline");
  //drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color)
  tft->drawEllipse(w, h, 80, 60, TFT_SKYBLUE);
  delay(_delay);

  Serial.println("Draw horizontal line");
  // drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color)
  tft->drawFastHLine(0, 0, w, TFT_DARKGREY);
  delay(_delay);  
  
  Serial.println("Draw Vertical line");
  // drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color)
  tft->drawFastVLine(0, 0, h, TFT_DARKGREY);
  delay(_delay);
  
  Serial.println("Draw A char (ascii 65)");
  //drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size)
  // Use this to find char code http://www.asciitable.com/
  tft->drawChar(margin, margin, 65, TFT_BLUE, TFT_WHITE, 2);
  delay(_delay);

  Serial.println("Draw a string");
  //drawString(const char *string, int32_t poX, int32_t poY)
  tft->drawString("T-Watch",margin, margin * 2);
  delay(_delay);

  Serial.println("Draw a pixel");
  //drawPixel(int32_t x, int32_t y, uint32_t color)
  tft->drawPixel(w / 2 , h / 2, TFT_RED);
  delay(_delay);
  /* Other usefull functions 
      invertDisplay(bool i)
      decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining)
  */
}

// Display font | Affiche une police en augmentant la taille progressivement de 1 à 7 (max)
void displayFontSize(){
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_WHITE);
    for (size_t i = 1; i <= 7; i++){  // Size from 1 to 7 (includes) | La taille doit être comprise entre 1 et 7
      tft->setTextFont(1);     // Only font 1 is available | Uniquement 1
      tft->setTextSize(i);     // Change size | change la taille        
      tft->setCursor(0, 0);
      txt = "Text with size "; txt += i;
      Serial.print("Display"); Serial.println(txt);
      tft->println(txt);
      delay(1000);
      tft->fillScreen(TFT_BLACK);
    }
}

void drawStringDemo(){
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_WHITE);
  tft->setTextSize(2); 
  for (size_t i = 0; i < 12; i++)
  {
    tft->setTextDatum(i);
    Serial.printf("Display string with justification %u \n", tft->getTextDatum());
    tft->drawString("TFT_eSPI Demo", w / 2, h / 2, 2);
    delay(1000);
    tft->fillScreen(TFT_BLACK);
  }
}

void screenOrientationDemo(){
  Serial.printf("Current screen orientation %u \n", tft->getRotation());
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_WHITE);
  tft->setTextDatum(MC_DATUM);
  tft->setTextSize(2); 
  tft->drawString("Rotate Screen", w / 2, h / 2, 2);
  for (size_t i = 1; i < 4; i++)
  {
    // rotate the screen orientation m = 0-3 or 4-7 for BMP drawing
    //setRotation(uint8_t m)
    Serial.printf("Rotate the screen to %u \n", i);
    tft->setRotation(i);
    tft->fillScreen(TFT_BLACK);
    tft->drawString("Rotate Screen", w / 2, h / 2, 2);
    delay(1000);
  }
  
  tft->setRotation(2);
  tft->fillScreen(TFT_BLACK);
  tft->drawString("Rotate Screen", w / 2, h / 2, 2);
}

// Use printf or println to display a list to screen | utilise la fonction println or printf pour afficher une liste à l'écran
void printAListToScreen(){
  //TFT_eSPI * tft = tft;
  tft->fillScreen(TFT_BLACK);
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->setTextSize(2);
  tft->setCursor(0,0);
  
  for (size_t i = 0; i <= 20; i++) {
    // Return cursor on the top of the screen | Renvoi le curseur en haut de l'écran 
    if ( tft->getCursorY() >= h ) {
      tft->fillScreen(TFT_BLACK);
      tft->setCursor(0,0);
    }
    tft->printf("This is the line %u \n", i);
    delay(200);
  }
  tft->setTextSize(1);

  tft->println(" \n *** End of the list ***");
  delay(2000);
}