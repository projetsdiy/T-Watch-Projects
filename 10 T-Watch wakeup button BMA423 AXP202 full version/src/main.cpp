/*
    T-Watch. Sleep and wake-up with BMA423 accelerometer or ESP32 button

    Putting the ESP32 and accessories to sleep. Programmed wake-up demo (Timer) 
    in deep sleep mode. Waking up with the home button with an interrupt on the 
    AXP202 power management controller or by detecting motion using the BMA423 
    accelerometer
    
    Step by step tutorials
    https://diyprojects.io/t-watch-sleep-and-wake-up-with-bma423-accelerometer-or-esp32-button/
    https://projetsdiy.fr/ttgo-t-watch-veille-deep-sleep-reveil-accelerometre-bma423-axp202-esp32/

    Licence : see licence file
    Original source code from lewis he
    This is just a demonstration. Most of the functions are not implemented.
    The main implementation is low-power standby.
    The off-screen standby (not deep sleep) current is about 4mA.
    Select standard motherboard and standard backplane for testing.
    Created by Lewis he on October 10, 2019.
*/

#include <Arduino.h>
#include <LilyGoWatch.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include <soc/rtc.h>
#include "esp_wifi.h"
#include "esp_sleep.h"
#include <WiFi.h>
#include "gui.h"


#define G_EVENT_VBUS_PLUGIN         _BV(0)
#define G_EVENT_VBUS_REMOVE         _BV(1)
#define G_EVENT_CHARGE_DONE         _BV(2)

#define G_EVENT_WIFI_SCAN_START     _BV(3)
#define G_EVENT_WIFI_SCAN_DONE      _BV(4)
#define G_EVENT_WIFI_CONNECTED      _BV(5)
#define G_EVENT_WIFI_BEGIN          _BV(6)
#define G_EVENT_WIFI_OFF            _BV(7)

enum {
    Q_EVENT_WIFI_SCAN_DONE,
    Q_EVENT_WIFI_CONNECT,
    Q_EVENT_BMA_INT,
    Q_EVENT_AXP_INT,
} ;

#define DEFAULT_SCREEN_TIMEOUT  5*1000

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  20          /* Time ESP32 will go to sleep (in seconds) */

#define WATCH_FLAG_SLEEP_MODE   _BV(1)
#define WATCH_FLAG_SLEEP_EXIT   _BV(2)
#define WATCH_FLAG_BMA_IRQ      _BV(3)
#define WATCH_FLAG_AXP_IRQ      _BV(4)

/*******************************************/
/*       Pointers | Pointeurs              */
/*******************************************/

TTGOClass *watch;
BMA *sensor;
AXP20X_Class *power;
TFT_eSPI *tft = nullptr;

QueueHandle_t g_event_queue_handle = NULL;
EventGroupHandle_t g_event_group = NULL;
EventGroupHandle_t isr_group = NULL;
bool lenergy = false;

/*******************************************/
/*               PROTOTYPES                */
/*******************************************/
void print_wakeup_reason();
void buttonClicked();

void setupNetwork()
{
    WiFi.mode(WIFI_STA);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        xEventGroupClearBits(g_event_group, G_EVENT_WIFI_CONNECTED);
    }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        uint8_t data = Q_EVENT_WIFI_SCAN_DONE;
        xQueueSend(g_event_queue_handle, &data, portMAX_DELAY);
    }, WiFiEvent_t::SYSTEM_EVENT_SCAN_DONE);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        xEventGroupSetBits(g_event_group, G_EVENT_WIFI_CONNECTED);
    }, WiFiEvent_t::SYSTEM_EVENT_STA_CONNECTED);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        //wifi_connect_status(true);
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
}
/*******************************************/
/*   Switch to low consumption mode        */
/*   Bascule en mode basse consommation    */
/*******************************************/
void low_energy()
{
    // Le rétro-éclairage est actif
    if (watch->bl->isOn()) {
        xEventGroupSetBits(isr_group, WATCH_FLAG_SLEEP_MODE);
        watch->closeBL();
        watch->bma->enableStepCountInterrupt(false);
        watch->displaySleep();
        watch->stopLvglTick();
        if (!WiFi.isConnected()) {
            lenergy = true;
            WiFi.mode(WIFI_OFF);
            // Decrease CPU frequency to 20MHz to reduce consumption
            // Diminue la fréquence du CPU à 20MHz pour réduire la consommation
            setCpuFrequencyMhz(20);

            Serial.println("ENTER IN LIGHT SLEEEP MODE");
            delay(50);

            // Alarm clock with the home button
            // Réveil avec le bouton principal
            gpio_wakeup_enable ((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);  
            gpio_wakeup_enable ((gpio_num_t)BMA423_INT1, GPIO_INTR_LOW_LEVEL);  
            esp_sleep_enable_gpio_wakeup();
        
            esp_light_sleep_start();
        }
    // The backlight is off
    // Le rétro-éclairage est éteint    
    } else {
        watch->displayWakeup();
        watch->openBL();
        watch->startLvglTick();  
        lv_disp_trig_activity(NULL);
        updateStepCounter(watch->bma->getCounter());
        updateBatteryLevel();
        updateBatteryIcon(LV_ICON_CALCULATION);
        watch->rtc->syncToSystem();
        watch->bma->enableStepCountInterrupt();        
    }
}

void setup()
{
    Serial.begin(115200);

    print_wakeup_reason();
    //Create a program that allows the required message objects and group flags
    g_event_queue_handle = xQueueCreate(20, sizeof(uint8_t));
    g_event_group = xEventGroupCreate();
    isr_group = xEventGroupCreate();

    watch = TTGOClass::getWatch();
    tft = watch->tft;
    watch->begin();

    // Records pointers
    // Enregistre les pointeurs
    power = watch->power;

    // User button handler
    // Superviseur bouton utilisateur
    watch->button->setClickHandler(buttonClicked);

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
    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(BMA423_INT1, [] {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        EventBits_t  bits = xEventGroupGetBitsFromISR(isr_group);
        if (bits & WATCH_FLAG_SLEEP_MODE)
        {
            //! For quick wake up, use the group flag
            xEventGroupSetBitsFromISR(isr_group, WATCH_FLAG_SLEEP_EXIT | WATCH_FLAG_BMA_IRQ, &xHigherPriorityTaskWoken);
        } else
        {
            uint8_t data = Q_EVENT_BMA_INT;
            xQueueSendFromISR(g_event_queue_handle, &data, &xHigherPriorityTaskWoken);
        }

        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR ();
        }
    }, RISING);

    sensor = watch->bma;
    // Optionnel
//    // Accel parameter structure
//    Acfg cfg;
//    /*!
//        Output data rate in Hz, Optional parameters:
//            - BMA4_OUTPUT_DATA_RATE_0_78HZ
//            - BMA4_OUTPUT_DATA_RATE_1_56HZ
//            - BMA4_OUTPUT_DATA_RATE_3_12HZ
//            - BMA4_OUTPUT_DATA_RATE_6_25HZ
//            - BMA4_OUTPUT_DATA_RATE_12_5HZ
//            - BMA4_OUTPUT_DATA_RATE_25HZ
//            - BMA4_OUTPUT_DATA_RATE_50HZ
//            - BMA4_OUTPUT_DATA_RATE_100HZ
//            - BMA4_OUTPUT_DATA_RATE_200HZ
//            - BMA4_OUTPUT_DATA_RATE_400HZ
//            - BMA4_OUTPUT_DATA_RATE_800HZ
//            - BMA4_OUTPUT_DATA_RATE_1600HZ
//    */
//    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
//    /*!
//        G-range, Optional parameters:
//            - BMA4_ACCEL_RANGE_2G
//            - BMA4_ACCEL_RANGE_4G
//            - BMA4_ACCEL_RANGE_8G
//            - BMA4_ACCEL_RANGE_16G
//    */
//    cfg.range = BMA4_ACCEL_RANGE_4G;
//    /*!
//        Bandwidth parameter, determines filter configuration, Optional parameters:
//            - BMA4_ACCEL_OSR4_AVG1
//            - BMA4_ACCEL_OSR2_AVG2
//            - BMA4_ACCEL_NORMAL_AVG4
//            - BMA4_ACCEL_CIC_AVG8
//            - BMA4_ACCEL_RES_AVG16
//            - BMA4_ACCEL_RES_AVG32
//            - BMA4_ACCEL_RES_AVG64
//            - BMA4_ACCEL_RES_AVG128
//    */
//    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
//
//    /*! Filter performance mode , Optional parameters:
//        - BMA4_CIC_AVG_MODE
//        - BMA4_CONTINUOUS_MODE
//    */
//    cfg.perf_mode = BMA4_CONTINUOUS_MODE;
//
//    // Configure the BMA423 accelerometer
//    sensor->accelConfig(cfg);
//
    // Enable BMA423 accelerometer
    // Active le BMA 423
    sensor->enableAccel();

    // Enable BMA423 isDoubleClick feature
    sensor->enableFeature(BMA423_WAKEUP, true);

    // Envoi un signal sur la broche 39 de l'ESP32 dès qu'un mouvement est détecté
    sensor->enableWakeupInterrupt();

    // R&cupère les interruptions du contrôleur d'alimentation AXP202
    pinMode(AXP202_INT, INPUT);
    attachInterrupt(AXP202_INT, [] {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        EventBits_t  bits = xEventGroupGetBitsFromISR(isr_group);
        if (bits & WATCH_FLAG_SLEEP_MODE)
        {
            //! For quick wake up, use the group flag
            xEventGroupSetBitsFromISR(isr_group, WATCH_FLAG_SLEEP_EXIT | WATCH_FLAG_AXP_IRQ, &xHigherPriorityTaskWoken);
        } else
        {
            uint8_t data = Q_EVENT_AXP_INT;
            xQueueSendFromISR(g_event_queue_handle, &data, &xHigherPriorityTaskWoken);
        }
        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR ();
        }
    }, FALLING);

    //Check if the RTC clock matches, if not, use compile time
    watch->rtc->check();

    //Synchronize time to system time
    watch->rtc->syncToSystem();

#ifdef LILYGO_WATCH_HAS_BUTTON
    //Set the user button long press to restart
    watch->button->setLongClickHandler([]() {
        Serial.println("Pressed Restart Button,Restart now ...");
        delay(1000);
        esp_restart();
    });
#endif
    //Setting up the network
    setupNetwork();
    //Initialize lvgl

    watch->lvgl_begin();
    //Execute your own GUI interface
    setupGui();

    //Clear lvgl counter
    lv_disp_trig_activity(NULL); 

#ifdef LILYGO_WATCH_HAS_BUTTON
    //In lvgl we call the button processing regularly
    lv_task_create([](lv_task_t *args) {
        watch->button->loop();
    }, 30, 1, nullptr);   
#endif
    //When the initialization is complete, turn on the backlight
    watch->openBL();
}

void loop()
{
    bool  rlst;
    uint8_t data;
    //! Fast response wake-up interrupt
    EventBits_t  bits = xEventGroupGetBits(isr_group);
    if (bits & WATCH_FLAG_SLEEP_EXIT) {
        Serial.println("EXIT SLEEP MODE");
        if (lenergy) {
            lenergy = false;
            setCpuFrequencyMhz(160);
        }

        low_energy();
        
        // Destroys the WATCH_FLAG_BMA_IRQ message (wake up with the BMA 423)
        // Détruit le message WATCH_FLAG_BMA_IRQ (réveil avec le BMA 423)
        if (bits & WATCH_FLAG_BMA_IRQ) {
            Serial.printf("WATCH_FLAG_BMA_IRQ bits=%u\n", bits);
            do {
                rlst =  watch->bma->readInterrupt();
            } while (!rlst);
            xEventGroupClearBits(isr_group, WATCH_FLAG_BMA_IRQ);
        }
        if (bits & WATCH_FLAG_AXP_IRQ) {
            Serial.printf("WATCH_FLAG_AXP_IRQ bits=%u\n", bits);
            watch->power->readIRQ();
            watch->power->clearIRQ();
            //TODO: Only accept axp power pek key short press
            xEventGroupClearBits(isr_group, WATCH_FLAG_AXP_IRQ);
        }
        xEventGroupClearBits(isr_group, WATCH_FLAG_SLEEP_EXIT);
        xEventGroupClearBits(isr_group, WATCH_FLAG_SLEEP_MODE);
    }
  
    if ((bits & WATCH_FLAG_SLEEP_MODE)) {
        //! No event processing after entering the information screen
        return;
    }

    //! Normal polling
    if (xQueueReceive(g_event_queue_handle, &data, 5 / portTICK_RATE_MS) == pdPASS) {
        switch (data) {
        case Q_EVENT_BMA_INT:
            do {
                rlst = watch->bma->readInterrupt();
            } while (!rlst);

            break;
        case Q_EVENT_AXP_INT:
            watch->power->readIRQ();
            if (watch->power->isPEKShortPressIRQ()) {
                // Switch to low consumption mode when the main button is pressed
                // Bascule en mode basse consommation lorsqu'on appuie sur le bouton principal
                watch->power->clearIRQ();
                low_energy();
                return;
            }
            watch->power->clearIRQ();
            break;
        case Q_EVENT_WIFI_SCAN_DONE: {
            int16_t len =  WiFi.scanComplete();
            for (int i = 0; i < len; ++i) {
                wifi_list_add(WiFi.SSID(i).c_str());
            }   
            break;
        }
        default:
            break;
        }

    }  
    if (lv_disp_get_inactive_time(NULL) < DEFAULT_SCREEN_TIMEOUT) {
        lv_task_handler();
    } else {
        low_energy();
    }   
}


void buttonClicked(){
    Serial.println("User button clicked, enter in deep sleep mode");
    
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

//Function that prints the reason by which ESP32 has been awaken from sleep
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by touchpad"); break;
    case 4  : Serial.println("Wakeup caused by timer"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}