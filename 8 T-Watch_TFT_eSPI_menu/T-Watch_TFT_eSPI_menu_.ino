/* Get Started with the TFT_eSPI library for TTGO T-Watch
 * Arduino IDE or PlatformIO
 * Step by step tutorials
 *
 * https://projetsdiy.fr/ttgo-t-watch-debuter-librairie-esp32-tft_espi/#changeorientation
 * Licence : see licence file
 * /
 
/* Arduino IDE - uncomment your watch */
#define LILYGO_WATCH_2019_WITH_TOUCH
//#define  LILYGO_WATCH_2019_NO_TOUCH
//#define LILYGO_WATCH_BLOCK
//#define LILYGO_WATCH_2020_V1

/* PlatformIO -> Select your watch in platformio.ini file */
#include <LilyGoWatch.h>
#include "xbm.h"             // Sketch tab header for xbm images

// C++ object which will allow access to the functions of the Watch | Objet C++ qui permettra d'accéder aux fonctions du boitier
TTGOClass *ttgo;
// C++ object that wil contain TFT_eSPI methods for quick access | Objet C++ TFT_eSPI permettant un accès rapide aux fonctions
TFT_eSPI *tft;

//*************************//
//*       Prototypes      *//
//*************************//
void goToPage(int pageindex);
void createMainPage();
void createPage(int pageindex);
void createLeftMenuButton();
void createRightMenuButton();
String isMenuTouched(int x, int y);
int displayScrollingMenu();
const unsigned char* getMenuIcon(int icoIndex);
void createMenuList(int mSel);
void createMenuListIcon(int mSel);
//*************************//

int w, h;           // screen width, height resolution | résolution de l'écran
int pageindex = 0;  // page to display | page à afficher

#define menuSize 30
#define menuPadding 10
bool menuOpened = false;

const int appMax = 9; // number of apps
String appName[appMax] = {"Main", "WiFi", "Bluetooth", "Accel", "Settings", "Clock", "GPS", "Music", "Tools"}; 
//String appIcon[appMax] = {"menu", "wifi", bluetooth, menu, wifi, bluetooth, menu, wifi, bluetooth, menu, wifi }; 

int menuSelected = 0;
String menuType = "list";  // Menu type to display, list or Icon | Type de menu à afficher (liste ou icone)

void setup() {
  Serial.begin(115200);
  // Get ttgo object and set up the display | Récupère l'objet et allume l'écran
  ttgo = TTGOClass::getWatch();
  ttgo->begin();     
  ttgo->openBL();

  // Easy access to TFT library functions | Accès facile aux fonctions de la librairie TFT 
  tft = ttgo->tft;
  //tft->setRotation(0);
  tft->fillScreen(TFT_BLACK);
  //tft->setCursor(0, 0);

  // Get Screen resolution | récupère le résolution de l'écran
  w = tft->width();     // width  | largeur
  h = tft->height();    // height | hauteur
  
  createMainPage();   // Display main menu on startup | affiche la page principale au démarrage
}

void loop() {
  int16_t x, y, tx, ty;
  if (ttgo->getTouch(x, y)) {
    while (ttgo->getTouch(tx, ty)) {} // wait until you stop touching
    String menuindex = isMenuTouched(x, y);
    if  ( menuindex  == "leftmenu" ) {
      menuType = "list";
      menuSelected = displayScrollingMenu();
      goToPage(menuSelected);
    } else if ( menuindex == "rightmenu" ) {
      Serial.println("right menu ");
      menuType = "icon";
      menuSelected = displayScrollingMenu();
      goToPage(menuSelected);
    } else if ( menuindex == "mainmenu" ) { 
      Serial.println("display tool page"); 
      menuSelected = toolPageMenu();
      goToPage(menuSelected);
    }   
  } 
  delay(5);
}


void goToPage(int pageindex){
  Serial.printf("displayPage %u \n", pageindex);
  switch (pageindex){
    case 0:
      createMainPage();  // Main page  
      break;
    default:
      createPage(pageindex);
  }

}

//*************************//
//*         Menus         *//
//*************************//
void createLeftMenuButton(){
  for (size_t i = 0; i < 3; i++) {
    tft->fillRoundRect(menuPadding, menuPadding + i*8, menuSize, 4, 2, TFT_WHITE);
  }
}

void createRightMenuButton(){
    tft->drawXBitmap(w - 30 - menuPadding, menuPadding , menu30, 30, 30, TFT_WHITE, TFT_BLACK);
}

// Check if menu is clicked
String isMenuTouched(int x, int y){
  String menuindex = "no";
  Serial.printf("isMenuTouched at %u:%u \n", x, y);
  if ( x <= menuSize + 2 * menuPadding && y <= menuSize + 2 * menuPadding) {
    menuindex = "leftmenu";
  } else if ( x >= w - 30 - 2 * menuPadding && y <= menuSize + 2 * menuPadding)  {
    menuindex = "rightmenu";
  } else if (x >= w / 2 - icon_width / 2 && x <= w / 2 + icon_width / 2 && y >= h / 2 && y <= h / 2 + icon_height )  {
    menuindex = "mainmenu";
  }
  return menuindex;
}


//*************************//
//*        Pages          *//
//*************************//
void createMainPage(){
  tft->fillScreen(TFT_BLACK);  // Fill background in black | colorie le fond de l'écran en noir
  tft->setTextDatum(MC_DATUM);  
  tft->setTextColor(TFT_WHITE);
  tft->setTextSize(2);
  tft->drawString("Main Page", w / 2, h / 3, 1);
  tft->drawXBitmap(w / 2 - icon_width / 2  , h / 2, menu, icon_width, icon_height, TFT_WHITE, TFT_BLACK);
  createLeftMenuButton();
  createRightMenuButton();
}

void createPage(int pageindex){
  tft->fillScreen(TFT_BLACK);  // Fill background in black | colorie le fond de l'écran en noir
  tft->setTextDatum(TL_DATUM);  
  tft->setTextColor(TFT_WHITE);
  tft->setTextSize(2);
  tft->drawString(appName[pageindex], menuSize + 2 * menuPadding, menuPadding, 1);
  tft->drawFastHLine(0, menuSize + menuPadding, w, TFT_DARKGREY);
  createLeftMenuButton();
  createRightMenuButton();
}

//*************************//
//*    Scrolling Menu     *//
//*************************//
int displayScrollingMenu(){
  int16_t x, y, tx, ty;
  int mSelect = menuSelected; // The currently highlighted app
  boolean exitMenu = false;   // used to stay in the menu until user selects app
    
  refreshMenu(menuSelected); // display vertical scrolling menu | affiche le menu défilant vertical
  
  while (!exitMenu) {
    if (ttgo->getTouch(x, y)) { // If you have touched something...
      while (ttgo->getTouch(tx, ty)) {} // wait until you stop touching
      Serial.printf("Menu y %u \n", y);
      Serial.printf("Height %u \n", tft->height());
      
      if (y >= 160 ) { // you want the menu list shifted up
        mSelect += 1;
        if (mSelect == appMax) mSelect = 0;
        refreshMenu(mSelect);
        //setMenuDisplay(mSelect);
      }

      if (y <= 80 ) { // you want the menu list shifted down
        mSelect -= 1;
        if (mSelect < 0) mSelect = appMax - 1;
        refreshMenu(mSelect);
        //setMenuDisplay(mSelect);
      }
      // Middle of the screen is selected
      if (y > 80 && y <160 ) { // You selected the middle
        exitMenu = true;
      }
    }
    delay(5);
  }
  //Return with mSelect containing the desired mode
  ttgo->tft->fillScreen(TFT_BLACK);
  menuOpened = false;

  return mSelect;
}

void refreshMenu(int selectPosition){
  if ( menuType == "list" ) {
    createMenuList(selectPosition);
  } else {
    createMenuListIcon(selectPosition);
  }
}

void createMenuList(int mSel) {
  menuOpened = true;
  int curSel = 0;
  // Display mode header
  ttgo->tft->fillScreen(TFT_BLUE);
  ttgo->tft->fillRect(0, 80, 239, 80, TFT_BLACK);

  // Display apps
  if (mSel == 0) curSel = appMax - 1;
  else curSel = mSel - 1;

  ttgo->tft->setTextSize(2);
  ttgo->tft->setTextColor(TFT_GREEN);
  ttgo->tft->setCursor(50, 30);
  ttgo->tft->println(appName[curSel]);

  ttgo->tft->setTextSize(3);
  ttgo->tft->setTextColor(TFT_RED);
  ttgo->tft->setCursor(40, 110);
  ttgo->tft->println(appName[mSel]);

  if (mSel == appMax - 1) curSel = 0;
  else curSel = mSel + 1;

  ttgo->tft->setTextSize(2);
  ttgo->tft->setTextColor(TFT_GREEN);
  ttgo->tft->setCursor(50, 190);
  ttgo->tft->print(appName[curSel]);
}

void createMenuListIcon(int mSel) {
  menuOpened = true;
  int curSel = 0;
  // Create blue background with a black line in the middle of the screen | Créé un fond bleu avec une ligne noire au milieu
  ttgo->tft->fillScreen(TFT_BLUE);
  ttgo->tft->fillRect(0, 80, 240, 80, TFT_BLACK);

  // First line of the scrolling menu | première ligne du menu déroulant
  if (mSel == 0) curSel = appMax - 1;
  else curSel = mSel - 1;
  
  ttgo->tft->setTextSize(2);
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawXBitmap(10, 20, getMenuIcon(curSel), 32, 32, TFT_WHITE, TFT_BLUE);
  ttgo->tft->setCursor(50, 30);
  ttgo->tft->println(appName[curSel]);

  // Central line (selected option) | Ligne centrale (option sélectionnée)
  ttgo->tft->setTextSize(3);
  ttgo->tft->setTextColor(TFT_RED);
  ttgo->tft->drawXBitmap(10, 100, getMenuIcon(mSel), 32, 32, TFT_WHITE, TFT_BLACK);
  ttgo->tft->setCursor(50, 110);  // 40
  ttgo->tft->println(appName[mSel]);

  // Last line | Dernière ligne du menu
  if (mSel == appMax - 1) curSel = 0;
  else curSel = mSel + 1;

  ttgo->tft->setTextSize(2);
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawXBitmap(10, 180, getMenuIcon(curSel), 32, 32, TFT_WHITE, TFT_BLUE);
  ttgo->tft->setCursor(50, 190);
  ttgo->tft->print(appName[curSel]);
}

const unsigned char* getMenuIcon(int icoIndex){
  Serial.printf("getMenuIcon %u \n", icoIndex);
  switch (icoIndex) {
    case 0:
      return main32;
      break;
    case 1:
      return wifi32;
      break; 
    case 2:
      return bluetooth32;
      break; 
    case 3:
      return accel32;
      break;      
    case 4:
      return settings32;
      break; 
    case 5:
      return clock32;
      break; 
    case 6:
      return gps32;
      break; 
    case 7:
      return music32;
      break;    
    case 8:
      return tools32;
      break;            
    default:
      return main32;  
  }
}
