#include <Arduino.h>
#include <LilyGoWatch.h>

// C++ object which will allow access to the functions of the Watch | Objet C++ qui permettra d'accéder aux fonctions du boitier
TTGOClass *watch;

void setup() {
    // Get Watch object and set up the display | Récupère l'objet et allume l'écran
    watch = TTGOClass::getWatch();
    watch->begin();     
    watch->openBL();
    
    // Use eTFT library to display text on screen | On utilise la librairie eTFT pour afficher du texte à l'écran
    watch->tft->fillScreen(TFT_BLACK);
    watch->tft->setTextFont(2);
    watch->tft->setTextSize(2);
    watch->tft->setTextColor(TFT_WHITE);
    watch->tft->setCursor(0, 0);
    watch->tft->println(F("Hello T-Watch"));
}

void loop() {}