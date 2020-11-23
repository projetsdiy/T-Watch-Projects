int toolPageMenu(){
  tft->fillScreen(TFT_BLACK);  // Fill background in black | colorie le fond de l'écran en noir
  tft->drawXBitmap(25, 25, wifi, icon_width, icon_height, TFT_WHITE, TFT_BLACK);
  tft->drawXBitmap(130, 25, bluetooth, icon_width, icon_height, TFT_WHITE, TFT_BLACK);
  tft->drawXBitmap(25, 130, gps, icon_width, icon_height, TFT_BLACK, TFT_WHITE);
  tft->drawXBitmap(130, 130, menuexit, icon_width, icon_height, TFT_WHITE, TFT_BLACK);

  int16_t x, y, tx, ty;
  int mSelect = menuSelected; // The currently highlighted app
  boolean exitMenu = false;   // used to stay in the menu until user selects app
  
  //setMenuDisplay(menuSelected); // display the list of Apps

  while (!exitMenu) {
    if (ttgo->getTouch(x, y)) { // If you have touched something...
      while (ttgo->getTouch(tx, ty)) {} // wait until you stop touching
      Serial.printf("toolPageMenu clicked %u %u\n", x, y);
      int index = 0;
      //if ( x < w / 2 && y < h / 2 ) {
        
      //}
      
      
      // Middle of the screen is selected
      if (x > 120 && y > 120 ) { // You selected the middle
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
