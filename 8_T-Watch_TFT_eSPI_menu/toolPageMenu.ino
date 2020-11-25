// Full screen central menu with only 4 buttons (wifi, bluetooth, GPS, exit)
// Menu central en plein écran avec uniquement 4 boutons (wifi, bluetooth, GPS, sortie)
int toolPageMenu(){
  tft->fillScreen(TFT_BLACK);  // Fill background in black | colorie le fond de l'écran en noir
  tft->drawXBitmap(25, 25, wifi, icon_width, icon_height, TFT_WHITE, TFT_BLACK);
  tft->drawXBitmap(130, 25, bluetooth, icon_width, icon_height, TFT_WHITE, TFT_BLACK);
  tft->drawXBitmap(25, 130, gps, icon_width, icon_height, TFT_WHITE, TFT_BLACK);
  tft->drawXBitmap(130, 130, menuexit, icon_width, icon_height, TFT_WHITE, TFT_BLACK);

  int16_t x, y, tx, ty;

  int mSelect; 
  // Keep the TFT_eSPI page open until the user touches an option
  // Permet de maintenir la page TFT_eSPI ouverte jusqu'à ce que l'utilisateur touche une option
  boolean exitMenu = false;   
  
  while (!exitMenu) {
    // Wait for the user to touch the screen
    // Attend que l'utilisateur touche l'écan
    if (ttgo->getTouch(x, y)) { 
      while (ttgo->getTouch(tx, ty)) {} 
      Serial.printf("toolPageMenu clicked %u %u\n", x, y);
      int index = 0;
      if ( x < w / 2 && y < h / 2 ) {
        mSelect = 1;
        exitMenu = true;
      } else if ( x > w / 2 && y < h / 2 ) {
        mSelect = 2;
        exitMenu = true;
      } else if ( x < w / 2 && y > h / 2 ) {
        mSelect = 6;
        exitMenu = true;
      } else {
        exitMenu = true;
      } 
    }
  }
  ttgo->tft->fillScreen(TFT_BLACK);
  menuOpened = false;

  // Returns the index of the desired page
  // Renvoie l'index de la page désirée
  return mSelect;
}
