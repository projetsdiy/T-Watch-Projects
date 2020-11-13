#include <Arduino.h>
#include <LilyGoWatch.h>

// C++ object which will allow access to the functions of the Watch | Objet C++ qui permettra d'accéder aux fonctions du boitier
TTGOClass *ttgo;
String txt; 
int w = 240;    // Screen width | Largeur de l'écran
int h = 240;   // Screen height | Hauteur de l'écran
int16_t x, y;
char buf[128];

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
    
    // Uncomment function you want to test | Dé-commenter les fonctions que vous voulez tester
    //fillScreenBackground();
    //displayFontSize();
    //drawBasic();
    //drawStringDemo();
    //screenOrientationDemo();
    printAListToScreen();
    // Clean screen | efface l'écran 
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
    ttgo->tft->setTextDatum(MC_DATUM);
}

void loop(){
  // Display cursor position each time the screen is touched | Affiche les coordonnées du curseur chaque fois que l'écran est touché
  if (ttgo->getTouch(x, y)) {
    sprintf(buf, "  x:%u  y:%u   ", x, y);
    ttgo->tft->drawString(buf, w / 2, h / 2);
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
    ttgo->tft->fillScreen(TFT_RED);
    delay(1000);
    Serial.println("Green");
    ttgo->tft->fillScreen(TFT_GREEN);
    delay(1000);
    Serial.println("Black");
    ttgo->tft->fillScreen(TFT_BLACK);
    delay(1000);
}

void getScreenSize(){
  w = ttgo->tft->width();
  h = ttgo->tft->height();
  Serial.printf("Screen size %u * %u", w, h);
}

// Draw primitives
void drawBasic(){
  int margin = 20;
  int _delay = 500;
  getScreenSize();
  
  Serial.println("Draw a rectangle");
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->drawRect(margin,margin, w - ( 2* margin ), h - ( 2 * margin),TFT_RED);
  delay(_delay);

  Serial.println("Draw a filled rectangle");
  // fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
  ttgo->tft->fillRect(2 * margin, 2 * margin, w - ( 4 * margin), h - (4 * margin), TFT_VIOLET);
  delay(_delay);

  Serial.println("Draw a rounded corner rectangle outline");
  //drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color)
  ttgo->tft->drawRoundRect(3 * margin, 3 * margin, w - ( 6 * margin), h - (6 * margin), 8, TFT_YELLOW);
  delay(_delay);

  Serial.println("Draw a line");
  // drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
  ttgo->tft->drawLine(0, 0, w, h, TFT_DARKGREY);
  delay(_delay);  

  Serial.println("Draw a triangle outline using 3 arbitrary points");
  // drawTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color)
  ttgo->tft->drawTriangle(w,h - 1, w - 40 , h, w - 20, h - 30, TFT_PURPLE);
  delay(_delay); 

  Serial.println("Draw a circle outline");
  //drawCircle(int32_t x0, int32_t y0, int32_t r, uint32_t color)
  ttgo->tft->drawCircle(w / 2, h / 2, 80, TFT_SKYBLUE);
  delay(_delay);

  Serial.println("Draw a ellipse outline");
  //drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color)
  ttgo->tft->drawEllipse(w, h, 80, 60, TFT_SKYBLUE);
  delay(_delay);

  Serial.println("Draw horizontal line");
  // drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color)
  ttgo->tft->drawFastHLine(0, 0, w, TFT_DARKGREY);
  delay(_delay);  
  
  Serial.println("Draw Vertical line");
  // drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color)
  ttgo->tft->drawFastVLine(0, 0, h, TFT_DARKGREY);
  delay(_delay);
  
  Serial.println("Draw A char (ascii 65)");
  //drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size)
  // Use this to find char code http://www.asciitable.com/
  ttgo->tft->drawChar(margin, margin, 65, TFT_BLUE, TFT_WHITE, 2);
  delay(_delay);

  Serial.println("Draw a string");
  //drawString(const char *string, int32_t poX, int32_t poY)
  ttgo->tft->drawString("T-Watch",margin, margin * 2);
  delay(_delay);

  Serial.println("Draw a pixel");
  //drawPixel(int32_t x, int32_t y, uint32_t color)
  ttgo->tft->drawPixel(w / 2 , h / 2, TFT_RED);
  delay(_delay);
  /* Other usefull functions 
      invertDisplay(bool i)
      decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining)
  */
}

// Display font
void displayFontSize(){
  TFT_eSPI * tft = ttgo->tft;
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
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextColor(TFT_WHITE);

  for (size_t i = 0; i < 12; i++)
  {
    ttgo->tft->setTextDatum(i);
    Serial.printf("Display string with justification %u \n", ttgo->tft->getTextDatum());
    ttgo->tft->drawString("TFT_eSPI Demo", w / 2, h / 2, 2);
    delay(1000);
    ttgo->tft->fillScreen(TFT_BLACK);
  }
}

void screenOrientationDemo(){
  Serial.printf("Screen orientation %u \n", ttgo->tft->getRotation());
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->setTextDatum(MC_DATUM);
  ttgo->tft->drawString("Rotate Screen", w / 2, h / 2, 2);
  for (size_t i = 1; i < 4; i++)
  {
    // rotate the screen orientation m = 0-3 or 4-7 for BMP drawing
    //setRotation(uint8_t m)
    Serial.printf("Rotate the screen to %u \n", i);
    ttgo->tft->setRotation(i);
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->drawString("Rotate Screen", w / 2, h / 2, 2);
    delay(1000);
  }
  
  ttgo->tft->setRotation(2);
  ttgo->tft->fillScreen(TFT_BLACK);
  ttgo->tft->drawString("Rotate Screen", w / 2, h / 2, 2);
}

// Use printf or println to display a list to screen | utilise la fonction println or printf pour afficher une liste à l'écran
void printAListToScreen(){
  TFT_eSPI * tft = ttgo->tft;
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