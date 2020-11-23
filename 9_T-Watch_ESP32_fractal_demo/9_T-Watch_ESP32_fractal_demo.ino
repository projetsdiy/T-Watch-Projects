/* Switch between LVGL and TFT_eSPI screen on a TTGO T-Watch
 * Landing page is created with LVGL library
 * Arduino IDE or PlatformIO
 * 
 * Step by step tutorials
 * Licence : see licence file
 * /
 
/* uncomment your watch */
#define LILYGO_WATCH_2019_WITH_TOUCH
//#define  LILYGO_WATCH_2019_NO_TOUCH
//#define LILYGO_WATCH_BLOCK
//#define LILYGO_WATCH_2020_V1

// Arduino IDE - uncomment to activate LVGL library
#define LILYGO_WATCH_LVGL
#include <Arduino.h>
#include <LilyGoWatch.h>


/**************************/
/*    Static variables    */
/**************************/
TTGOClass *ttgo = nullptr;
TFT_eSPI *tft = nullptr;


bool irq = false;
bool KeyPressed = false;
int mSelect = 0;


const int TFT_Width  = 240;
const int TFT_Height = 240;
const int TFT_MINX   =   0;
const int TFT_MAXX   = 240;  
const int TFT_MINY   =   0;
const int TFT_MAXY   = TFT_Height;
int boutonY;
#define nb_colors 6
enum colors {RED, GREEN, BLUE, GRAY, PAL1, PAL2};
byte Palette = 1;
// Touch
boolean wastouched = true;

// Paramètres Mandel
const float Mandel_MINX = -2.3;
const float Mandel_MAXX =  0.75;
const float Mandel_MINY = -1.2;
const float Mandel_MAXY =  1.2;
float Current_MINX = Mandel_MINX;
float Current_MINY = Mandel_MINY;
float Current_MAXX = Mandel_MAXX;
float Current_MAXY = Mandel_MAXY;
float Prev_MINX = Current_MINX;
float Prev_MINY = Current_MINY;
float Prev_MAXX = Current_MAXX;
float Prev_MAXY = Current_MAXY;
float Jcx = 0.3;
float Jcy = 0.01;
int itermin = 100;
int itermax = 0;
boolean Mandel = true;
byte Puissance = 2;
#define Num_puiss 6
#define Max_iter 64
#define Escape 100
char texte[40];

static bool touched = false;
void IRAM_ATTR isr()
{
  touched = true;
}


/* STATIC PROTOTYPE */
void Draw_Mandel();
int Calcul_Mandel(float cx, float cy);
uint32_t Couleur(int i, int mini, int maxi);
void VisuZone(int i, int j); 
void CalculZone(int xtft, int ytft);
void resetParameters();
void buttonClicked();
void buttonLongClick();
void buttonDoubleClick();
void coeffJulia();
uint8_t menu(void);
static void menu_event_handler(lv_obj_t * obj, lv_event_t event);
void incrementColor();
void previousZoom();

void Draw_Mandel() {
  unsigned long temps = millis();
  float deltaX = (Current_MAXX - Current_MINX) / (TFT_MAXX - TFT_MINX - 1);
  float deltaY = (Current_MAXY - Current_MINY) / (TFT_MAXY - TFT_MINY - 1);
  for (int i = TFT_MINX; i < TFT_MAXX; i++) { // Premier tracé à blanc
    float cx = Current_MINX + i * deltaX;
    for (int j = TFT_MINY; j < TFT_MAXY; j++) {
      float cy = Current_MINY + j * deltaY;
      int iteration = Calcul_Mandel (cx, cy);
      if (iteration < itermin && iteration > 0) itermin = iteration;
      if (iteration > itermax) itermax = iteration;
      uint32_t color = Couleur(iteration, 0, Max_iter);
      tft->drawPixel(i, j, color);
    }
  }
  temps = millis() - temps;

  Serial.print ("Temps de calcul : ");
  Serial.print(temps);
  Serial.println(" ms.");
}

void coeffJulia(){
  int16_t xtft, ytft;
  tft->fillRect(0,  0, TFT_MAXX, TFT_MAXY, TFT_WHITE);
  tft->setTextSize(1);
  tft->setTextColor(TFT_BLACK);
  tft->drawString("Choisir le coefficient C", 75, 30);
  for (int i = 0; i < 4; i++) {
    tft->drawFastVLine(i * TFT_MAXX / 4, 0, TFT_MAXY, TFT_BLACK);
    tft->drawFastHLine(0, i * TFT_MAXY / 4, TFT_MAXX, TFT_BLACK);
  }
  tft->drawNumber(-2, 5, TFT_MAXY / 2 + 5);
  tft->drawNumber(-1, TFT_MAXX / 5 + 15, TFT_MAXY / 2 + 5);
  tft->drawNumber( 1, 4 * TFT_MAXX / 5, TFT_MAXY / 2 + 5);
  tft->drawNumber( 2, 5 * TFT_MAXX / 5 - 10, TFT_MAXY / 2 + 5);
  tft->drawNumber(-1, TFT_MAXX / 2 + 5, TFT_MAXY - 15);
  tft->drawNumber( 0, TFT_MAXX / 2 + 5, TFT_MAXY / 2 + 5);
  tft->drawNumber( 1, TFT_MAXX / 2 + 5, 5);

  boolean istouched = false;
  while (!istouched) istouched = ttgo->getTouch(xtft, ytft);
  delay(200);
  istouched = false;
  while (!istouched) istouched = ttgo->getTouch(xtft, ytft);
  float coefX = 4.0 / (TFT_MAXX - TFT_MINX);
  float coefY = 2.0 / (TFT_MAXY - TFT_MINY);
  Jcx = -2.0 + xtft * coefX;
  Jcy =  1.0 - ytft * coefY;
  tft->fillCircle(xtft, ytft, 3, TFT_BLUE);
  sprintf (texte, "Coefficient : ( %f , %f )", Jcx, Jcy);
  tft->drawString(texte, 25, 50);
  delay(3000);
  float dx = 0.7;
  Current_MINX = Mandel_MINX + dx;
  Current_MINY = Mandel_MINY;
  Current_MAXX = Mandel_MAXX + dx;
  Current_MAXY = Mandel_MAXY;
}

int Calcul_Mandel(float cx, float cy) {
  float x1 = 0;
  float y1 = 0;
  float x2 = 0;
  float y2 = 0;
  if (!Mandel) {
    x1 = cx;
    y1 = cy;
    x2 = cx;
    y2 = cy;
    cx = Jcx;
    cy = Jcy;
  }
  float temp = 0;
  int iteration = 0;

  while (true) {
    if (iteration >= Max_iter) return Max_iter;
    if (x2 * x2 + y2 * y2 > Escape) return iteration;
    for (int i = 0; i < Puissance - 1; i++) { // Calcul z**n
      temp = x1 * x2 - y1 * y2;
      y2 = x1 * y2 + x2 * y1;
      x2 = temp;
    }
    x1 = x2 + cx;
    y1 = y2 + cy;
    x2 = x1;
    y2 = y1;
    ++iteration;
  } // Fin Mandelbrot
}

uint32_t Couleur(int i, int mini, int maxi) {
  byte red = 0;
  byte green = 0;
  byte blue = 0;
  // color picker : https://www.w3schools.com/colors/colors_picker.asp
  byte col = 255 - map(i, mini, maxi, 0, 255); // de 0 à 255
  switch (Palette) {
    case RED :
      red   = col;
      green = 0;
      blue  = 0;
      break;
    case GREEN :
      red   = 0;
      green = col % 64;
      blue  = 0;
      break;
    case BLUE :
      red   = 0;
      green = 0;
      blue  = col % 32;
      break;
    case GRAY :
      red   = col % 50;
      green = red * 2;
      blue  = red;
      break;
    case PAL1 :
      red   = col % 60;
      green = col % 40;
      blue  = col % 10;
      break;
    case PAL2 : // passage bleu - vert - rouge
      red   = 0;
      green = 0;
      blue  = 0;
      if (col < 33) {
        red  = col;
      } else if (col < 200) {
        green  = col % 64; // / 4;
      } else {
        blue  = col - 200;
      }
      break;
  }
  return red << 11 | green << 5 | blue; //tft.color565(red, green, blue);
}

void VisuZone(int i, int j) {
  tft->drawCircle(i, j, 4, TFT_WHITE);
  tft->drawRect(i - (TFT_MAXX - TFT_MINX) / 8, j - (TFT_MAXY - TFT_MINY) / 8,
               (TFT_MAXX - TFT_MINX) / 4, (TFT_MAXY - TFT_MINY) / 4, TFT_WHITE);
}

void resetParameters(){
  Current_MINX = Mandel_MINX;
  Current_MINY = Mandel_MINY;
  Current_MAXX = Mandel_MAXX;
  Current_MAXY = Mandel_MAXY;
}

void CalculZone(int xtft, int ytft) {
  float coefX = (Current_MAXX - Current_MINX) / (TFT_MAXX - TFT_MINX);
  float Mandel_centerX = Current_MINX + (xtft - TFT_MINX) * coefX;
  float coefY = (Current_MAXY - Current_MINY) / (TFT_MAXY - TFT_MINY);
  float Mandel_centerY = Current_MINY + (ytft - TFT_MINY) * coefY;
  float DeltaX = (Current_MAXX - Current_MINX) / 10.;
  float DeltaY = (Current_MAXY - Current_MINY) / 10.;
  Prev_MINX = Current_MINX;
  Prev_MINY = Current_MINY;
  Prev_MAXX = Current_MAXX;
  Prev_MAXY = Current_MAXY;
  Current_MINX = Mandel_centerX - DeltaX;
  Current_MAXX = Mandel_centerX + DeltaX;
  Current_MINY = Mandel_centerY - DeltaY;
  Current_MAXY = Mandel_centerY + DeltaY;
}

void setup() {
  Serial.begin(115200);
  // Get ttgp object and set up the display | Récupère l'objet et allume l'écran
  ttgo = TTGOClass::getWatch();
  ttgo->begin();     
  ttgo->openBL();
  ttgo->lvgl_begin();
  // Easy access to TFT library functions | Accès facile aux fonctions de la librairie TFT 
  tft = ttgo->tft;
  tft->fillScreen(TFT_BLACK);

  Draw_Mandel();

  //pinMode(36, INPUT_PULLUP);
  //attachInterrupt(36, isr, FALLING);

  ttgo->button->setLongClickHandler(buttonLongClick);
  ttgo->button->setClickHandler(buttonClicked);  
  ttgo->button->setDoubleClickHandler(buttonDoubleClick);

  pinMode(AXP202_INT, INPUT_PULLUP);
  attachInterrupt(AXP202_INT, [] {
      irq = true;
  }, FALLING);
  //!Clear IRQ unprocessed  first
  ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_IRQ, true);
  ttgo->power->clearIRQ();
}

void buttonClicked(){
  Serial.println("Change color");
  incrementColor();
  Draw_Mandel();
  
}
void buttonDoubleClick(){
  previousZoom();
  Draw_Mandel();
  Serial.println("Previous zoom");
}

void buttonLongClick(){
  Serial.println("Long click...");
}

void previousZoom(){
  Serial.println ("Previous");
  Current_MINX = Prev_MINX;
  Current_MINY = Prev_MINY;
  Current_MAXX = Prev_MAXX;
  Current_MAXY = Prev_MAXY;
}

void incrementColor(){
  Palette = (Palette + 1) % nb_colors;
}

void loop() {
  int16_t x, y;
  
  if (irq) {
      irq = false;
      ttgo->power->readIRQ();
      if ( ttgo->power->isPEKLongtPressIRQ() ) {
          Serial.println("Power button long press -> Reset Parameters");
          resetParameters();
          Draw_Mandel();
      }
      if (ttgo->power->isPEKShortPressIRQ()) {
        Serial.println("Power button pressed -> Display Menu");
        int choix = menu();
        float dx = 0;
        switch ( choix ){
          case 0: //Reset
            resetParameters();
            break;
          case 1: // Previous Zoom
            previousZoom();
            break;
          case 2:
            incrementColor();
            break;  
          case 3:
            //Choix Mandebrot / Julia
            Mandel = !Mandel;
            if (!Mandel) dx = 0.7;
            Current_MINX = Mandel_MINX + dx;
            Current_MINY = Mandel_MINY;
            Current_MAXX = Mandel_MAXX + dx;
            Current_MAXY = Mandel_MAXY; 
            break; 
          case 4:  // Coefficient Julia
            coeffJulia();
            break;
          case 5:
            // Puissance
            Puissance = 2 + (Puissance - 1) % Num_puiss;
            if (Puissance != 2) dx = 0.65;
            Current_MINX = Mandel_MINX + dx;
            Current_MINY = Mandel_MINY;
            Current_MAXX = Mandel_MAXX + dx;
            Current_MAXY = Mandel_MAXY;
            break;
          default:
            break;  
        }
        Draw_Mandel();
          //ttgo->tft->fillRect(20, 100, 200, 85, TFT_BLACK);
          //ttgo->tft->drawString("PowerKey Press", 25, 100);
      }
      ttgo->power->clearIRQ();
  }

  if ( touched ) {
    Serial.println("Button :)");
    touched = false;
  }

  if ( ttgo->getTouch(x, y) ) {
    int startup = millis();
    while ( ttgo->getTouch(x, y) ) {}
    if ( millis() - startup >= 1500 ) {
      Serial.println("Long touch -> Reset Fractal");
      
    } else {
      VisuZone(x, y);
      CalculZone(x, y);
      Draw_Mandel();
    }
  }
  ttgo->button->loop();
}

uint8_t menu(void)
{
    const char* typeFractal = "MANDEL";
    if ( !Mandel ) typeFractal = "JULIA";
    static const char * btnm_map[] = 
      {"RESET", "\n",
       "UNDO", "\n",
       "COLOR", "\n",
       typeFractal, "Coeff. Julia", "\n",
       "PARAM", ""};
                                  
    KeyPressed = false;
    lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act(), NULL);
    
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_btnmatrix_set_btn_width(btnm1, 10, 2);        /*Make "Action1" twice as wide as "Action2"*/
    lv_btnmatrix_set_btn_ctrl(btnm1, 10, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(btnm1, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
    lv_obj_set_size(btnm1, 240, 240);
    lv_obj_align(btnm1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btnm1, menu_event_handler);
    
    while (!KeyPressed) {lv_task_handler(); delay(20);} // Wait for touch
    //ttgo->tft->fillScreen(TFT_BLACK);
    return mSelect;
}

static void menu_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        mSelect = lv_btnmatrix_get_active_btn(obj);
        KeyPressed = true;
    }
}
