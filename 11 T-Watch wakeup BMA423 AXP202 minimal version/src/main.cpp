/* 
 * This code only run is designed to run with TFT_eSPI library
 * Some function are not running with LVGL library
 * Arduino IDE or PlatformIO
 * 
 * Step by step tutorials

 * Licence : see licence file
 * /
 
/* Arduino IDE - uncomment your watch */
//#define LILYGO_WATCH_2019_WITH_TOUCH
//#define  LILYGO_WATCH_2019_NO_TOUCH
//#define LILYGO_WATCH_BLOCK
//#define LILYGO_WATCH_2020_V1
/* PlatformIO -> Select your watch in platformio.ini file */
#include <Arduino.h>
#include <LilyGoWatch.h>
#include "esp_sleep.h"


/**************************/
/*    Static variables    */
/**************************/
TTGOClass *watch = nullptr;
BMA *sensor;
AXP20X_Class *power;
TFT_eSPI *tft = nullptr;
bool KeyPressed = false;
bool lenergy = false;
int awake = 0;
int seconde = 0;
int timeleft = 0;
static bool irq_axp202 = false;
static bool irq_bma = false;
static bool return_to_deepsleep = false;

#define DEFAULT_SCREEN_TIMEOUT  5*1000

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30          /* Time ESP32 will go to sleep (in seconds) */

/**************************/
/*   STATIC PROTOTYPES    */
/**************************/
void buildTFTPage(int timeleft);
void goToDeepSleep();
void resetChrono();
int get_wakeup_reason();

/**************************/
/*   Switch On/Off power  */
/**************************/
void low_energy()
{
    if ( watch->bl->isOn()) {
      if ( return_to_deepsleep ) {
        goToDeepSleep();
      } else {  
        // If the backlighting is active >> switches to light sleep mode. The ESP32 Core remains active
        // Si le rétro-éclairage est actif >> passe en mode light sleep. Le Core de l'ESP32 reste actif
        Serial.println("BL is ON >> activate light sleep");
        watch->closeBL();
        watch->bma->enableStepCountInterrupt(false);
        watch->displaySleep();
        lenergy = true;
        // Decrease CPU frequency to 10MHz to reduce consumption
        // Diminue la fréquence du CPU à 10MHz pour réduire la consommation
        setCpuFrequencyMhz(10);
        Serial.println("ENTER IN LIGHT SLEEP MODE");
        delay(50);
        // Alarm clock with the home button
        // Réveil avec le bouton principal
        gpio_wakeup_enable ((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);  
        // Waking up with the accelerometer
        // Réveil avec l'accéléromètre
        gpio_wakeup_enable ((gpio_num_t)BMA423_INT1, GPIO_INTR_LOW_LEVEL);
        // Active le réveille depuis le GPIO  
        esp_sleep_enable_gpio_wakeup();
        // Puts into light standby. The CPU Core keeps running
        // Met en veille légère. Le Core du CPU continue de fonctionner
        esp_light_sleep_start();
        // Continue reading Deep Sleep / Light Sleep and ESP32
        // https://diyprojects.io/esp32-arduino-code-for-deep-sleep-and-wake-ups-timer-touch-pad-gpio/
        // https://projetsdiy.fr/esp32-code-arduino-sommeil-deep-sleep-reveils-timer-touch-pad-gpio/
      }  
    } else {
      // The backlight is off   
      // Le rétro-éclairage est éteint    
      lenergy = false;
      setCpuFrequencyMhz(160);
      resetChrono();
      Serial.println("Wake-up ESP32 and accessories");
      watch->displayWakeup();
      watch->openBL();
      watch->rtc->syncToSystem();
      delay(100);
    }
}
void setup() {
  Serial.begin(115200);

  // Retourne en sommeil profond si le réveil a été causé par le timer
  if ( get_wakeup_reason() == 4 ) return_to_deepsleep = true;

  watch = TTGOClass::getWatch();
    // Initialize the hardware
  watch->begin();
  power = watch->power;

  // Turn on the backlight | Allume le rétro-éclairage
  watch->openBL();
  
  // T-Watch with user button only
  watch->button->setClickHandler(goToDeepSleep);
  
  // Turn on the IRQ used
  watch->power->adc1Enable(AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);
  watch->power->enableIRQ(AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_FINISHED_IRQ, AXP202_ON);
  watch->power->clearIRQ();
  
  // Turn off unused power
  watch->power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
  watch->power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
  watch->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
  watch->power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);   
  
  //Connection interrupted to the specified pin
  // Learn more about interruption and ESP32
  // https://diyprojects.io/esp32-how-use-external-interrupts-arduino-code/
  // https://projetsdiy.fr/esp32-interruptions-externes-code-arduino/
  pinMode(BMA423_INT1, INPUT);
  attachInterrupt(BMA423_INT1, [] {
      irq_bma = true;
  }, RISING);

  sensor = watch->bma;

  // Accel parameter structure
  Acfg cfg;
  cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
  cfg.range = BMA4_ACCEL_RANGE_4G;
  cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
  cfg.perf_mode = BMA4_CONTINUOUS_MODE;
  sensor->accelConfig(cfg);
  
  // Active and BMA 423
  // Active le BMA 423
  sensor->enableAccel();

  // Activates the wake-up function of the BMA423
  // Active la fonction de réveil du BMA423
  sensor->enableFeature(BMA423_WAKEUP, true);
  
  // Send a signal on pin 39 of the ESP32 as soon as movement is detected
  // Envoi un signal sur la broche 39 de l'ESP32 dès qu'un mouvement est détecté
  sensor->enableWakeupInterrupt();

  // Interrupt that allows you to lightly sleep or wake up the screen
  // Interruption qui permet de mettre en veille légère ou réveiller l'écran
  pinMode(AXP202_INT, INPUT);
  attachInterrupt(AXP202_INT, [] {
      irq_axp202 = true;
  }, FALLING);
  watch->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_IRQ, true);
  watch->power->clearIRQ();

  // Démarre le chronomètre pour la mise en veille automatique
  awake = millis();
  resetChrono();
  // Build landing page 
  //créé la page principale
  buildTFTPage(timeleft);
}

void loop() {
  int16_t x, y;
  bool rlst;
  
  if ( watch->getTouch(x,y) ) {
    while (watch->getTouch(x, y) ) {}
    // Relance le chrono lorsque l'utilisateur touche l'écran
    awake = millis();
  }
  // Lorsqu'on dépasse le temps d'inactivité >> met en veille l'écran et les périphériques de la montre
  if ( millis() - awake > DEFAULT_SCREEN_TIMEOUT) {
    if ( !lenergy) {
      awake = millis();
      low_energy();
    }  
  }
  if ( millis() - seconde >= 1000 ) {
    if ( !lenergy) {
      seconde = millis();
      timeleft --;
      buildTFTPage(timeleft);
    }  
  }
  // L'utilisateur vient d'appuyer sur le bouton principal
  // L'utilisateur vient d'appuyer sur le bouton principal
  if (irq_axp202) {
    Serial.println("irq detected");
    irq_axp202 = false;
    watch->power->readIRQ();
    if ( watch->power->isPEKShortPressIRQ() ) {
      Serial.println("Power button pressed >> wakeup / switch on light sleep");
      low_energy();
    }     
    watch->power->clearIRQ();
  }  
  // L'utilisateur vient de tapoter l'écran
  if ( irq_bma ) {
    Serial.println("irq_bma detectee");
    irq_bma = false;   
    resetChrono();   
    Serial.println("Power button pressed >> wakeup / switch on light sleep");
    do {
      rlst = watch->bma->readInterrupt();
    } while (!rlst);
    low_energy();
  }  
  // The state of the user button is checked at each passage through the mouth
  // On vérifie à chaque passage dans la bouche l'état du bouton utilisateur
  watch->button->loop();  
}
// Reset of the stopwatch for automatic standby of the screen (light sleep mode)
// Reset du chronomètre pour la mise en veille automatique de l'écran (mode light sleep)
void resetChrono()
{
  seconde = millis();
  awake = millis();
  timeleft = DEFAULT_SCREEN_TIMEOUT / 1000;
  buildTFTPage(timeleft);
}
void buildTFTPage(int timeleft){
  TFT_eSPI *tft = watch->tft;
  tft->fillScreen(TFT_BLACK);
  tft->setTextSize(2);
  tft->setTextColor(TFT_WHITE);
  tft->drawString("T-Watch Sleep Demo", 0,0);
  tft->drawFastHLine(0,20,240,TFT_WHITE);
  tft->drawString("Light sleep in", 10,60);
  char buf[20];
  tft->setTextSize(4);
  sprintf(buf, "%u s", timeleft);
  tft->drawString(buf, 50,100);
}

// Allows you to activate the deep sleep mode and the timer for a periodic awakening (data logger for example)
// Permet d'activer le mode deep sleep et le timer pour un éveil périodique (enregistreur de données par exemple)
void goToDeepSleep()
{
    Serial.println("Go To Deep Sleep Mode");
    
    // Set screen and touch to sleep mode
    watch->displaySleep();
    /*
    When using T - Watch2020V1, you can directly call power->powerOff(),
    if you use the 2019 version of TWatch, choose to turn off
    according to the power you need to turn off
    */
#ifdef LILYGO_WATCH_2020_V1
    watch->powerOff();
    // LDO2 is used to power the display, and LDO2 can be turned off if needed
    // power->setPowerOutPut(AXP202_LDO2, false);
#else
    power->setPowerOutPut(AXP202_LDO3, false);
    power->setPowerOutPut(AXP202_LDO4, false);
    power->setPowerOutPut(AXP202_LDO2, false);
    // The following power channels are not used
    power->setPowerOutPut(AXP202_EXTEN, false);
    power->setPowerOutPut(AXP202_DCDC2, false);
#endif
    esp_sleep_enable_ext0_wakeup((gpio_num_t)AXP202_INT, LOW);
    
    // Activate Timer Wakeup. Usefull for a GPS trcker for example
    // Active le réveil automatique. Utile pour un tracker GPS
    esp_sleep_enable_timer_wakeup(uS_TO_S_FACTOR * TIME_TO_SLEEP);

    esp_deep_sleep_start();
}

// Function that prints the reason by which ESP32 has been awaken from sleep
// Indique la raison du réveil de l'ESP32
int get_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case 1  : Serial.printf("Wakeup caused by external signal using RTC_IO %u \n", wakeup_reason); break;
    case 2  : Serial.printf("Wakeup caused by external signal using RTC_CNTL %u \n", wakeup_reason); break;
    case 3  : Serial.printf("Wakeup caused by touchpad %u \n", wakeup_reason); break;
    case 4  : Serial.printf("Wakeup caused by timer %u \n", wakeup_reason); break;
    case 5  : Serial.printf("Wakeup caused by ULP program %u \n", wakeup_reason); break;
    default : Serial.printf("Wakeup was not caused by deep sleep %u \n", wakeup_reason); break;
  }
  return wakeup_reason;
}