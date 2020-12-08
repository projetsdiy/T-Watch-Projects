#include <Arduino.h>
#include <LilyGoWatch.h>
//#include "SIMPLE_BUTTON.c"
TTGOClass *ttgo;

// Pointeurs vers les ressources LVGL : images et fontes
LV_IMG_DECLARE(bg1);
LV_IMG_DECLARE(menu);
LV_IMG_DECLARE(simple_button);
LV_IMG_DECLARE(list_ico);
LV_IMG_DECLARE(switch_ico);
LV_IMG_DECLARE(spinbox_ico);
LV_IMG_DECLARE(gauge_ico);
LV_IMG_DECLARE(slider_ico);


// Pointeurs partagés
static lv_obj_t * slider_label = nullptr;
static lv_obj_t * scr = nullptr;
static lv_obj_t * spinbox = nullptr;
//static lv_obj_t * cpicker = nullptr;
static lv_obj_t * gauge = nullptr;
static lv_obj_t * arc = nullptr;
//static lv_obj_t * list = nullptr;

/*  CONTAINERS */
static lv_obj_t * cont_demo = nullptr;
static lv_obj_t * cont_scroll_menu = nullptr;
static lv_obj_t * scrollmenu = nullptr;
static lv_obj_t * exit_button = nullptr;

/* PROTOTYPES */
static void create_wallpaper();
static void create_menu(bool vertical);
static void button_simple();
static void switch_demo();
static void slider_demo();
static void spinbox_demo();
static void list_demo();
static void gauge_demo();
lv_style_t get_transparent_style();
static void event_handler(lv_obj_t *obj, lv_event_t event);
static void button_event_handler(lv_obj_t * obj, lv_event_t event);
static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
static void lv_spinbox_increment_event_cb(lv_obj_t * btn, lv_event_t e);
static void lv_spinbox_decrement_event_cb(lv_obj_t * btn, lv_event_t e);
static void list_event_cb(lv_obj_t * obj, lv_event_t event);
static void display_exit_button();
static void display_help(String _view);

static bool irq_axp202 = false;
static bool animate_gauge = false;

// Number of options in the vertical menu
// Nombre d'options dans le menu vertical
#define nboptions 6
typedef struct {
  const char *name;
  void *img;
  void (*event_cb)();
} lv_menu_config_t;

lv_menu_config_t _cfg[nboptions] = {
  {.name = "Simple Button",  .img = (void *) &simple_button },
  {.name = "Slider",  .img = (void *) &slider_ico },
  {.name = "Switch",  .img = (void *) &switch_ico }, 
  {.name = "Spinbox",  .img = (void *) &spinbox_ico }, 
  {.name = "List",  .img = (void *) &list_ico },
  {.name = "Gauge",  .img = (void *) &gauge_ico },
};

lv_obj_t * _options[nboptions];

void setup() {
  Serial.begin(115200);
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  ttgo->lvgl_begin();

  pinMode(AXP202_INT, INPUT);
  attachInterrupt(AXP202_INT, [] {
      irq_axp202 = true;
  }, FALLING);
  ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_IRQ, true);
  ttgo->power->clearIRQ();

  // Create wallpaper
  // Image de fond d'écran centrée
  create_wallpaper();
  
  // Create vertical (True) or Horizontal (false) menu
  // Créé le menu vertical (true) ou horizontal (false)
  create_menu(true);
}

void loop() {
  // put your main code here, to run repeatedly:
  lv_task_handler();
  if (irq_axp202) {
    //Serial.println("irq detected");
    irq_axp202 = false;
    ttgo->power->readIRQ();
    if ( ttgo->power->isPEKShortPressIRQ() ) {
      Serial.println("Power button pressed >> return to main menu");
      lv_obj_set_hidden(cont_demo, true);
      lv_obj_set_hidden(exit_button, true);
      lv_obj_set_hidden(cont_scroll_menu, false);
    }     
    ttgo->power->clearIRQ();
  } 
  if ( animate_gauge ) {
    animate_gauge = false;
    printf("Animate Gauge \n");
    for (size_t i = 0; i < 10; i++)
    {
      lv_gauge_set_value(gauge, 0, i * 10);
      delay(150);
    }
  }
  ttgo->button->loop();  
}

void create_wallpaper()
{
  scr = lv_scr_act();                               // get current scene
  static lv_obj_t * img_bg = lv_img_create(scr, NULL);    // Create image object
  lv_img_set_src(img_bg, &bg1);                      // Push and display image in the scene
  lv_obj_align(img_bg, NULL, LV_ALIGN_CENTER, 0, 0);// Align object on the screen
}

void create_menu(bool vertical = true)
{
  // Get general stylesheet
  // Récupère le style général
  static lv_style_t _style = get_transparent_style();
  lv_style_set_text_color(&_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
  lv_style_set_image_recolor(&_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);

  lv_point_t * _vp = new lv_point_t [nboptions];

  cont_scroll_menu = lv_cont_create(lv_scr_act(), NULL);

  // Créé autant de tuiles qu'il y a d'options dans le menu - verticalement ou horizontalement
  for (int i = 0; i < nboptions; i++) {
      if (vertical) {
          _vp[i].x = 0;
          _vp[i].y = i;
      } else {
          _vp[i].x = i;
          _vp[i].y = 0;
      }
  }

  lv_obj_set_size(cont_scroll_menu, 240, 240);
  //lv_obj_align(_cont, NULL, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  lv_obj_align(cont_scroll_menu, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(cont_scroll_menu, LV_OBJ_PART_MAIN, &_style);

  scrollmenu = lv_tileview_create(cont_scroll_menu, NULL);
  lv_tileview_set_valid_positions(scrollmenu, _vp, nboptions );
  lv_tileview_set_edge_flash(scrollmenu, false);
  lv_obj_align(scrollmenu, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_page_set_scrlbar_mode(scrollmenu, LV_SCRLBAR_MODE_OFF);
  lv_obj_add_style(scrollmenu, LV_OBJ_PART_MAIN, &_style);

  lv_coord_t _w = lv_obj_get_width(scrollmenu) ;
  lv_coord_t _h = lv_obj_get_height(scrollmenu);

  for (int i = 0; i < nboptions; i++) {
    Serial.printf("Add %s tile to menu %ux%u \n", _cfg[i].name, _w, _h);
    _options[i] = lv_cont_create(scrollmenu, scrollmenu);
    lv_obj_set_size(_options[i], _w, _h);

    lv_obj_t *img = lv_img_create(_options[i], NULL);
    lv_img_set_src(img, _cfg[i].img);
    lv_obj_align(img, _options[i], LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(_options[i], NULL);
    lv_label_set_text(label, _cfg[i].name);
    lv_obj_align(label, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    
    i == 0 ? lv_obj_align(_options[i], NULL, LV_ALIGN_CENTER, 0, 0) : lv_obj_align(_options[i], _options[i - 1], direction ? LV_ALIGN_OUT_BOTTOM_MID : LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    
    lv_tileview_add_element(scrollmenu, _options[i]);
    lv_obj_set_click(_options[i], true);
    lv_obj_set_event_cb(_options[i], event_handler);
  }
}  
/******************************/
/*     LVGL WIDGET DEMO       */
/*   DEMO DES WIDGET LVGL     */
/******************************/
void button_simple()
{ 
  static lv_style_t _style = get_transparent_style();
  cont_demo = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_size(cont_demo, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(cont_demo, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(cont_demo, LV_OBJ_PART_MAIN, &_style);

  lv_obj_t * label;

  lv_obj_t * btn1 = lv_btn_create(cont_demo, NULL);
  lv_obj_set_event_cb(btn1, button_event_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);

  label = lv_label_create(btn1, NULL);
  lv_label_set_text(label, "Button");

  lv_obj_t * btn2 = lv_btn_create(cont_demo, NULL);
  lv_obj_set_event_cb(btn2, button_event_handler);
  lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 0, 40);
  lv_btn_set_checkable(btn2, true);
  lv_btn_toggle(btn2);
  lv_btn_set_fit2(btn2, LV_FIT_NONE, LV_FIT_TIGHT);

  label = lv_label_create(btn2, NULL);
  lv_label_set_text(label, "Toggled");
  
  display_exit_button();  
}
void slider_demo(){
  // Documentation https://docs.lvgl.io/latest/en/html/widgets/slider.html
  static lv_style_t _style = get_transparent_style();
  cont_demo = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_size(cont_demo, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(cont_demo, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(cont_demo, LV_OBJ_PART_MAIN, &_style);

  // Text in white
  // Texte en blanc
  lv_style_set_text_color(&_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    
  lv_obj_t * slider = lv_slider_create(cont_demo, NULL);
  lv_obj_set_size(slider, LV_HOR_RES - 40, 20);
  lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, -30);
  // Min / max
  lv_slider_set_range(slider, 0, 100);
  lv_obj_set_event_cb(slider, slider_event_cb);

  /* Create a label below the slider */
  slider_label = lv_label_create(cont_demo, NULL);
  lv_label_set_text(slider_label, "0");
  lv_obj_set_auto_realign(slider_label, true);
  lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);


  arc = lv_arc_create(cont_demo, NULL);
  lv_obj_add_style(arc, LV_OBJ_PART_MAIN, &_style);
  lv_arc_set_bg_angles(arc, 0, 360);
  lv_arc_set_angles(arc, 0, 360);
  lv_arc_set_end_angle(arc, 0);
  lv_obj_set_size(arc, 120, 120);
  lv_obj_align(arc, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

  // Ajoute un texte d'aide dans le haut de l'écran
  display_help("slider");
  // Add the back button to the main menu
  // Ajoute le bouton de retour au menu principal
  display_exit_button(); 
}
void switch_demo()
{
  // Documentation
  static lv_style_t _style = get_transparent_style();
  cont_demo = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_size(cont_demo, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(cont_demo, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(cont_demo, LV_OBJ_PART_MAIN, &_style);

  /*Create a switch and apply the styles*/
  lv_obj_t *sw1 = lv_switch_create(cont_demo, NULL);
  lv_obj_align(sw1, NULL, LV_ALIGN_CENTER, 0, -50);
  lv_obj_set_event_cb(sw1, event_handler);

  /*Copy the first switch and turn it ON*/
  lv_obj_t *sw2 = lv_switch_create(cont_demo, sw1);
  lv_switch_on(sw2, LV_ANIM_ON);
  lv_obj_align(sw2, NULL, LV_ALIGN_CENTER, 0, 50);

  // Add the back button to the main menu
  // Ajoute le bouton de retour au menu principal
  display_exit_button(); 
}
void list_demo(){
  // Documentation
  static lv_style_t _style = get_transparent_style();
  cont_demo = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_size(cont_demo, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(cont_demo, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(cont_demo, LV_OBJ_PART_MAIN, &_style);

  lv_obj_t * list = lv_list_create(cont_demo, NULL);

  //lv_obj_set_width(obj, 150);
  //lv_obj_set_pos(obj, 20, 20);        
  //lv_cont_set_fit2(obj, LV_FIT_NONE, LV_FIT_TIGHT);
  //lv_list_add_btn(obj, NULL, "Item 1");
  //lv_list_add_btn(obj, NULL, "Item 2");
  //lv_list_add_btn(obj, NULL, "Item 3");
  //lv_list_add_btn(obj, NULL, "Item 4");

  lv_obj_set_size(list, 160, LV_VER_RES);
  lv_obj_align(list, NULL, LV_ALIGN_CENTER, 0, 0);

  /*Add buttons to the list*/
  lv_obj_t * list_btn;
  list_btn = lv_list_add_btn(list, LV_SYMBOL_FILE, "New"); 
  lv_obj_set_event_cb(list_btn, list_event_cb);

  list_btn = lv_list_add_btn(list, LV_SYMBOL_DIRECTORY, "Open");
  lv_obj_set_event_cb(list_btn, list_event_cb);

  list_btn = lv_list_add_btn(list, LV_SYMBOL_CLOSE, "Delete");
  lv_obj_set_event_cb(list_btn, list_event_cb);

  list_btn = lv_list_add_btn(list, LV_SYMBOL_EDIT, "Edit");
  lv_obj_set_event_cb(list_btn, list_event_cb);

  list_btn = lv_list_add_btn(list, LV_SYMBOL_SAVE, "Save");
  lv_obj_set_event_cb(list_btn, list_event_cb);

  list_btn = lv_list_add_btn(list, LV_SYMBOL_BELL, "Notify");
  lv_obj_set_event_cb(list_btn, list_event_cb);

  list_btn = lv_list_add_btn(list, LV_SYMBOL_BATTERY_FULL, "Battery");
  lv_obj_set_event_cb(list_btn, list_event_cb);

  // Add the back button to the main menu
  // Ajoute le bouton de retour au menu principal
  display_exit_button(); 
}

void spinbox_demo()
{
  static lv_style_t _style = get_transparent_style();
  cont_demo = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_size(cont_demo, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(cont_demo, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(cont_demo, LV_OBJ_PART_MAIN, &_style);

  spinbox = lv_spinbox_create(cont_demo, NULL);
  lv_spinbox_set_range(spinbox, -1000, 90000);
  lv_spinbox_set_digit_format(spinbox, 5, 2);
  lv_spinbox_step_prev(spinbox);
  lv_obj_set_width(spinbox, 100);
  lv_obj_align(spinbox, NULL, LV_ALIGN_CENTER, 0, 0);

  lv_coord_t h = lv_obj_get_height(spinbox);
  lv_obj_t * btn = lv_btn_create(cont_demo, NULL);

  lv_obj_set_size(btn, h, h);
  lv_obj_align(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
  lv_theme_apply(btn, LV_THEME_SPINBOX_BTN);
  lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_PLUS);
  lv_obj_set_event_cb(btn, lv_spinbox_increment_event_cb);

  btn = lv_btn_create(cont_demo, btn);
  lv_obj_align(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
  lv_obj_set_event_cb(btn, lv_spinbox_decrement_event_cb);
  lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_MINUS);

  display_exit_button(); 
}

void gauge_demo(void)
{
  static lv_style_t _style = get_transparent_style();
  cont_demo = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_size(cont_demo, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(cont_demo, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(cont_demo, LV_OBJ_PART_MAIN, &_style);
  
  /*Describe the color for the needles*/
  static lv_color_t needle_colors[1];
  needle_colors[0] = LV_COLOR_BLUE;
  //needle_colors[1] = LV_COLOR_ORANGE;
  //needle_colors[2] = LV_COLOR_PURPLE;

  /*Create a gauge*/
  gauge = lv_gauge_create(cont_demo, NULL);
  lv_gauge_set_needle_count(gauge, 1, needle_colors);
  lv_obj_set_size(gauge, 200, 200);
  lv_obj_align(gauge, NULL, LV_ALIGN_CENTER, 0, 0);

  // Place the gauge needle at random
  // Place l'aiguille de la gauge au hasard
  lv_gauge_set_value(gauge, 0, random(0,100));

  //lv_gauge_set_value(gauge, 1, 20);
  //lv_gauge_set_value(gauge, 2, 30);
  display_exit_button(); 

  // Screen is ready, we launch a mini animation of the needle
  // Ecran est prêt, on lance une mini animation de l'aiguille
  delay(100);
  animate_gauge = true;
}
/***************************/
/* DISPLAY INFO AND BUTTON */
/* AFFICHE INFO ET BOUTON  */
/***************************/
void display_exit_button()
{
  exit_button = lv_imgbtn_create(lv_scr_act(), NULL);
  lv_imgbtn_set_src(exit_button, LV_BTN_STATE_RELEASED, &menu);
  lv_imgbtn_set_src(exit_button, LV_BTN_STATE_PRESSED, &menu);
  lv_imgbtn_set_src(exit_button, LV_BTN_STATE_CHECKED_PRESSED, &menu);
  lv_imgbtn_set_src(exit_button, LV_BTN_STATE_CHECKED_RELEASED, &menu);
  lv_obj_align(exit_button, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -20, -20);
  lv_obj_set_event_cb(exit_button, event_handler);
  lv_obj_set_top(exit_button, true);
}
void display_help(String _view)
{
  lv_obj_t * info = lv_label_create(cont_demo, NULL);

  if ( _view == "slider") {
    lv_label_set_text(info, "Move the slider and see that the label\n"
                            "updates to match it.");
  } else {
    lv_label_set_text(info, "");
  }
  lv_obj_align(info, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);
  lv_obj_set_top(info, true);
}
/************************************************/
/*  EVENTS HANDLER (actions on objects)         */
/*  SUPERVISEUR EVENEMENTS (actions sur objets) */
/************************************************/
static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    //LV_EVENT_PRESSED,             /**< The object has been pressed*/
    //LV_EVENT_PRESSING,            /**< The object is being pressed (called continuously while pressing)*/
    //LV_EVENT_PRESS_LOST,          /**< User is still pressing but slid cursor/finger off of the object */
    //LV_EVENT_SHORT_CLICKED,       /**< User pressed object for a short period of time, then released it. Not called if dragged. */
    //LV_EVENT_LONG_PRESSED,        /**< Object has been pressed for at least `LV_INDEV_LONG_PRESS_TIME`.  Not called if dragged.*/
    //LV_EVENT_LONG_PRESSED_REPEAT, /**< Called after `LV_INDEV_LONG_PRESS_TIME` in every
    //                                   `LV_INDEV_LONG_PRESS_REP_TIME` ms.  Not called if dragged.*/
    //LV_EVENT_CLICKED,             /**< Called on release if not dragged (regardless to long press)*/
    //LV_EVENT_RELEASED,            /**< Called in every cases when the object has been released*/
    //LV_EVENT_DRAG_BEGIN,
    //LV_EVENT_DRAG_END,
    //LV_EVENT_DRAG_THROW_BEGIN,
    //LV_EVENT_GESTURE,           /**< The object has been gesture*/
    //LV_EVENT_KEY,
    //LV_EVENT_FOCUSED,
    //LV_EVENT_DEFOCUSED,
    //LV_EVENT_LEAVE,
    //LV_EVENT_VALUE_CHANGED,      /**< The object's value has changed (i.e. slider moved) */
    //LV_EVENT_INSERT,
    //LV_EVENT_REFRESH,
    //LV_EVENT_APPLY,  /**< "Ok", "Apply" or similar specific button has clicked*/
    //LV_EVENT_CANCEL, /**< "Close", "Cancel" or similar specific button has clicked*/
    //LV_EVENT_DELETE, /**< Object is being deleted */
    if (event == LV_EVENT_SHORT_CLICKED) {  //!  Event callback Is in here
      //Serial.printf("Object clicked %s\n", obj);
          for (size_t i = 0; i < nboptions; i++)
          {
            if ( obj == _options[i]) {
              Serial.printf("Open Demo: %s\n", _cfg[i].name);
              if ( _cfg[i].name == "Simple Button" ) {
                lv_obj_set_hidden(cont_scroll_menu, true);
                button_simple();
              } else if ( _cfg[i].name == "Slider" ) {
                lv_obj_set_hidden(cont_scroll_menu, true);
                slider_demo();
              } else if ( _cfg[i].name == "Switch" ) {
                lv_obj_set_hidden(cont_scroll_menu, true);
                switch_demo();
              } else if ( _cfg[i].name == "Spinbox" ) {
                lv_obj_set_hidden(cont_scroll_menu, true);
                spinbox_demo();
              } else if ( _cfg[i].name == "List" ) {
                lv_obj_set_hidden(cont_scroll_menu, true);
                list_demo();
              } else if ( _cfg[i].name == "Gauge" ) {
                lv_obj_set_hidden(cont_scroll_menu, true);
                gauge_demo();
              } 
            } 
            // Return to the main menu
            // Retourne au menu principal
            else if ( obj == exit_button ) {
              lv_obj_set_hidden(cont_demo, true);
              lv_obj_set_hidden(exit_button, true);
              lv_obj_set_hidden(cont_scroll_menu, false);
            }
          }
   } 
}
// SLIDER
static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        static char buf[20]; /* max 3 bytes for number plus 1 null terminating byte */
        snprintf(buf, 20, "Current value: %u", lv_slider_get_value(slider));
        // Update slider label
        // Met à jour le libellé
        lv_label_set_text(slider_label, buf);
        // Update arc value
        // Actualise la valeur de l'arc
        printf("update arc to angle %u \n", map(lv_slider_get_value(slider), 0, 100, 0, 360));
        lv_arc_set_end_angle(arc, map(lv_slider_get_value(slider), 0, 100, 0, 360));
    }
}
// BUTTON
static void button_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
      printf("Button Clicked \n");
    }
    else if(event == LV_EVENT_VALUE_CHANGED) {
      printf("Toggled\n");
    }
}
// Spinbox
static void lv_spinbox_increment_event_cb(lv_obj_t * btn, lv_event_t e)
{
    if(e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox);
    }
}
static void lv_spinbox_decrement_event_cb(lv_obj_t * btn, lv_event_t e)
{
    if(e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_decrement(spinbox);
    }
}
// List clicked
static void list_event_cb(lv_obj_t * obj, lv_event_t event)
{
    if ( event == LV_EVENT_CLICKED ) {
        printf("Element clicked: %s \n", lv_list_get_btn_text(obj));
    }
}
/************************/
/*     STYPE SHEET      */
/*   FEUILLE DE STYPE   */
/************************/
lv_style_t get_transparent_style() 
{
  static lv_style_t mainStyle;
  // Container radius (pixels)
  // Rayon du conteneur (pixels)
  lv_style_set_radius(&mainStyle, LV_OBJ_PART_MAIN, 0);         
  // Background color
  // Couleur de fond 
  lv_style_set_bg_color(&mainStyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
  // Opacity of the background, here transparent. From 0 (transparent) to 255 (100%) or LV_OPA_0 to LV_OPA_100 (with a step of 10)
  // Opacité du fond, ici transparent. De 0 (transparent) à 255 (100%) ou LV_OPA_0 à LV_OPA_100 (avec un pas de 10)
  lv_style_set_bg_opa(&mainStyle, LV_OBJ_PART_MAIN, LV_OPA_0);  
  // Container border thickness in pixels
  // Epaisseur du bord en pixel
  lv_style_set_border_width(&mainStyle, LV_OBJ_PART_MAIN, 0);
  return mainStyle;
}
