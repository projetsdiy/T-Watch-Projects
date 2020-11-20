/* Switch between TFT_eSPI and LVGL screen on a TTGO T-Watch
 * Landing page is created with TFT_eSPI library
 * Arduino IDE or PlatformIO
 * Step by step tutorials
 * 
 * Licence : see licence file
 * /
 
/* Arduino IDE - uncomment your watch */
//#define LILYGO_WATCH_2019_WITH_TOUCH
//#define  LILYGO_WATCH_2019_NO_TOUCH
//#define LILYGO_WATCH_BLOCK
//#define LILYGO_WATCH_2020_V1

// Arduino IDE - uncomment to activate LVGL librarie
//#define LILYGO_WATCH_LVGL

/* PlatformIO -> Select your watch in platformio.ini file */
#include <Arduino.h>
#include <LilyGoWatch.h>

// Declare background image | Déclare l'image de fond
LV_IMG_DECLARE(WALLPAPER_1_IMG);

/**************************/
/*    Static variables    */
/**************************/
TTGOClass *ttgo = nullptr;
static lv_obj_t *lvglpage = NULL;
bool KeyPressed = false;

/**************************/
/*   STATIC PROTOTYPES    */
/**************************/
bool gotoLVGLPage();
static void event_handler(lv_obj_t * obj, lv_event_t event);
void buildTFTPage();
/**************************/

void setup() {
  Serial.begin(115200);

  ttgo = TTGOClass::getWatch();
    // Initialize the hardware
  ttgo->begin();
  ttgo->lvgl_begin();

  // Turn on the backlight | Allume le rétro-éclairage
  ttgo->openBL();
  // Build landing page | créé la page principale
  buildTFTPage();
}

void loop() {
  int16_t x, y;
  if (ttgo->getTouch(x, y)) {
    while (ttgo->getTouch(x, y)) {} // wait for user to release
    if ( gotoLVGLPage() ) buildTFTPage();
  }  
}

void buildTFTPage(){
  TFT_eSPI *tft = ttgo->tft;
  tft->fillScreen(TFT_BLACK);
  tft->setTextSize(2);
  tft->setTextColor(TFT_WHITE);
  tft->drawString("TFT_eSPI Screen", 0,0);
  // Toucher l'écran pour sortir
  tft->drawString("Touch screen to open LVGL page", 0,20);
}

// Create a screen with LVGL library | Créé un écran avec la librairie LVGL
bool gotoLVGLPage(){
  KeyPressed = false;
  // Container that contain all displayed elements. Makes it easier to show or hide the page 
  // Conteneur qui contient tous les élements affiché. Permet d'afficher ou masquer plus facilement la page
  lvglpage = lv_cont_create( lv_scr_act(), NULL );
  lv_obj_set_width( lvglpage, lv_disp_get_hor_res( NULL )  );  // Horizontal resolution | résolution horizontale
  lv_obj_set_height( lvglpage, lv_disp_get_ver_res( NULL ) );  // Vertical resolution | résolution verticale

  // Background Image  | Image de fond
  lv_obj_t * img1 = lv_img_create(lvglpage, NULL);
  lv_img_set_src(img1, &WALLPAPER_1_IMG);
  lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 0);

  // Button in center of the screen | Bouton au centre de l'écran
  lv_obj_t * btn1 = lv_btn_create(lvglpage, NULL);
  lv_obj_set_event_cb(btn1, event_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, 0);
  
  // Display a circuar scrolling welcome message 
  // Affiche un message défilant de bienvenue
  lv_obj_t * welcomemessage;
  welcomemessage = lv_label_create(lvglpage, NULL);
  lv_label_set_long_mode(welcomemessage, LV_LABEL_LONG_SROLL_CIRC);     /*Circular scroll*/
  lv_obj_set_width(welcomemessage, lv_disp_get_hor_res( NULL ));
  lv_label_set_text(welcomemessage, "Welcome on LVGL Demo Screen for TTGO T-Wach");
  lv_obj_align(welcomemessage, btn1, LV_ALIGN_CENTER, 0, -60);

  // Button label | libellé du bouton
  lv_obj_t * label;
  label = lv_label_create(btn1, NULL);
  lv_label_set_text(label, "Exit");

  while (!KeyPressed) {lv_task_handler(); delay(20);} // Wait for touch
  Serial.print("Exit LVGL page");
  lv_obj_del(lvglpage);
  return true;
}

// Switch screen button handler | Déclencheur du bouton pour passer d'un écran LVGL à TFT_eSPI
static void event_handler(lv_obj_t * obj, lv_event_t event){
  // Important !! always test event to avoid multiple signals
  // Il faut toujours tester l'évènement sinon plusieurs signaux sont envoyés dans la queue ce qui entraîne l'affichage de plusieurs écrans TFT_eSPI
  if (event == LV_EVENT_CLICKED) {
    Serial.println("event_handler => return main page");
    KeyPressed = true;
  }  
}



