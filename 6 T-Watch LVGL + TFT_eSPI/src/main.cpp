/* Switch between LVGL and TFT_eSPI screen on a TTGO T-Watch
 * Landing page is created with LVGL library
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

QueueHandle_t g_event_queue_handle = nullptr;

// Enum Queue events | Enumére les événements de la queue
enum {
  Q_EVENT_DISPLAY_TFT,
};

// Declare background image | Déclare l'image de fond
LV_IMG_DECLARE(WALLPAPER_1_IMG);

/**************************/
/*    Static variables    */
/**************************/
TTGOClass *ttgo = nullptr;
static bool onAir = true;
static lv_obj_t *lvglpage = NULL;

/**************************/
/*   STATIC PROTOTYPES    */
/**************************/
void createLVGLPage();
static void event_handler(lv_obj_t * obj, lv_event_t event);
void hideLVGLpage(bool hide);
bool openTFT();
/**************************/

void setup() {
  Serial.begin(115200);

  // Create an event queue that we will use to trigger the display of the TFT_eSPI screen 
  // Créé une file d'événement que l'on va utiliser pour déclencher l'affichage de l'écran TFT_eSPI
  g_event_queue_handle = xQueueCreate(20, sizeof(uint8_t));

  ttgo = TTGOClass::getWatch();
    // Initialize the hardware
  ttgo->begin();
  ttgo->lvgl_begin();

  // Turn on the backlight | Allume le rétro-éclairage
  ttgo->openBL();

  // and create main screen | et affiche l'écran principal
  createLVGLPage();
}

void loop() {
  uint8_t data;
  if (xQueueReceive(g_event_queue_handle, &data, 5 / portTICK_RATE_MS) == pdPASS) {
    switch (data) {
      case Q_EVENT_DISPLAY_TFT:{
        //Hide LVGL screen | cache l'écran LVGL
        hideLVGLpage(true);
        // Display TFT_eSPI screen and wait user touch the screen | afficher l'écran eSPI et attend que l'utilisateur touche l'écran
        // Inside the eSPI screen, LVGL handler in not executed | Dans l'écran eSPI, le superviseur LVGL n'est pas exécuté
        while ( openTFT() ) {}
        // Show LVGL main page | Affiche la page d'accueil LVGL
        hideLVGLpage(false);
        // Force refresh | force le rafraichissement de l'écran
        //lv_disp_trig_activity(nullptr);
        //lv_refr_now(nullptr);
      }
      break;
      default:
        break;
    }
  }  
  // LGVL handler | Superviseur LVGL
  lv_task_handler();
}

// Create a screen with LVGL library | Créé un écran avec la librairie LVGL
void createLVGLPage(){
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
  lv_label_set_text(label, "Go to TFT_eSPI");
}

// Switch screen button handler | Déclencheur du bouton pour passer d'un écran LVGL à TFT_eSPI
static void event_handler(lv_obj_t * obj, lv_event_t event){
  // Important !! always test event to avoid multiple signals
  // Il faut toujours tester l'évènement sinon plusieurs signaux sont envoyés dans la queue ce qui entraîne l'affichage de plusieurs écrans TFT_eSPI
  if (event == LV_EVENT_CLICKED) {
    Serial.println("event_handler => send open TFT Screen");
    uint8_t data = Q_EVENT_DISPLAY_TFT;
    xQueueSend(g_event_queue_handle, &data, portMAX_DELAY);
  }  
}

// Show / hide LVGL page | Masque / affiche la page LVGL
void hideLVGLpage(bool hide){
  lv_obj_set_hidden(lvglpage, hide);
}

// Create a screen with TFT_eSPI library | Créé un écran avec la librairie TFT_eSPI
bool openTFT(){    
    Serial.println("Display TFT_eSPI screen");    

    onAir = true;

    TTGOClass *ttgo = TTGOClass::getWatch();
    TFT_eSPI *tft = ttgo->tft;

    tft->fillScreen(TFT_BLACK);
    tft->setTextSize(2);
    tft->setTextColor(TFT_WHITE);
    tft->drawString("TFT_eSPI Screen", 0,0);
    // Toucher l'écran pour sortir
    tft->drawString("Touch screen to exit", 0,20);

    // Wait user touch the screen to exit | Attend que l'utilisateur touche l'écran pour sortir
    while (onAir) {

      if (!onAir) return false;
      
      int16_t x,y;

      if (ttgo->getTouch(x, y)) {
        while (ttgo->getTouch(x, y)) {}           // wait for user to release | Attend que l'utilisateur relaâche l'écran
        Serial.println("User touch the screen");
        onAir = false;
      }     
    }  
} 

