/* How to convert in C++ color image with LVGL
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
// Arduino IDE - uncomment to activate LVGL library
//#define LILYGO_WATCH_LVGL

/* PlatformIO -> Select your watch in platformio.ini file */
#include <Arduino.h>
#include <LilyGoWatch.h>

// C++ object which will allow access to the functions of the Watch | Objet C++ qui permettra d'accéder aux fonctions du boitier
TTGOClass *ttgo;
// C++ object that wil contain TFT_eSPI methods for quick access | Objet C++ TFT_eSPI permettant un accès rapide aux fonctions
TFT_eSPI *tft;

LV_IMG_DECLARE(WALLPAPER_1_IMG);
LV_IMG_DECLARE(WALLPAPER_2_IMG);
LV_IMG_DECLARE(WALLPAPER_3_IMG);
LV_IMG_DECLARE(R2D2);

void setup() {
  Serial.begin(115200);
  // Get ttgp object and set up the display | Récupère l'objet et allume l'écran
  ttgo = TTGOClass::getWatch();
  ttgo->begin();    
  ttgo->lvgl_begin(); 
  ttgo->openBL();
  
  lv_obj_t *img1 = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img1, &WALLPAPER_1_IMG);
  lv_img_set_zoom(img1, 128);
  lv_obj_align(img1, NULL, LV_ALIGN_CENTER, -60, -60);
  
  delay(200);

  lv_obj_t *img2 = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img2, &WALLPAPER_2_IMG);
  lv_img_set_zoom(img2, 128);
  lv_obj_align(img2, NULL, LV_ALIGN_CENTER, 60, -60);

  delay(250);

  lv_obj_t *img3 = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img3, &WALLPAPER_3_IMG);
  lv_img_set_zoom(img3, 128);
  lv_obj_align(img3, NULL, LV_ALIGN_CENTER, -60, 60);

  lv_obj_t *img4 = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src(img4, &R2D2);
  lv_img_set_zoom(img4, 128);
  lv_obj_align(img4, NULL, LV_ALIGN_CENTER, 60, 60);

}


void loop() {
  lv_task_handler();
}