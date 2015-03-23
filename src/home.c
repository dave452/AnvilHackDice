#include <pebble.h>
//GNU TERRY PRATCHETT
//Does gloabl variables
static Window *s_macro_window;
static Window *s_main_window;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static TextLayer *s_macro_layer;
static void tap_handler(AccelAxisType axis, int32_t direction);
int random(int max);
//static void send_roll();
int sharedDice = 20;
void config_providerv2(Window *window);
void main_rnd_handler(ClickRecognizerRef recognizer, void *context);

//Writes result of roll on screen
static void text(int Result){
  //Converts result of roll into string
  int result1 = Result % 10;
  int result10 = (Result - result1) / 10;
  char disp1 = (char)result1 + '0';
  char disp10 = (char)result10 + '0'; 
  char arr[2] = {disp10,disp1};
  char *str="   ";
  strcpy(str,arr);
  
  //Set up and write on text layer
  s_macro_layer = text_layer_create(GRect(0, 50,144, 144));
  text_layer_set_background_color(s_macro_layer, GColorClear);
  text_layer_set_text_color(s_macro_layer, GColorBlack);
  text_layer_set_font(s_macro_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_macro_layer, GTextAlignmentCenter);
  text_layer_set_text(s_macro_layer, str);
  layer_add_child(window_get_root_layer(s_macro_window), text_layer_get_layer(s_macro_layer));
}  

//Choose random number sends it to text
//Pretty crappy comment right there
static void send_roll(){
  text(random(sharedDice));
}
//Intialises the main window (sets layers, and images)
static void main_window_load(Window *window) {
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_HomeScr);
  s_background_layer = bitmap_layer_create(GRect(0,0,144,154));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(s_main_window), bitmap_layer_get_layer(s_background_layer));
}

//No idea why this is important but it was on the example so I kept it.
static void main_window_unload(Window *window) {
    // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
    // Destroy Window
  window_destroy(s_main_window);
}

//Loads macro (roll) window, should probably change the name of it
static void macro_window_load(Window *window) {
  send_roll();
  accel_tap_service_subscribe(tap_handler);
   
}

//Changes from main window to macro window 
void main_anything_handler(ClickRecognizerRef recognizer, void *context){
  s_macro_window = window_create();
  macro_window_load(s_macro_window);
  window_stack_push(s_macro_window, true);
  
  window_set_click_config_provider(s_macro_window, (ClickConfigProvider) config_providerv2);
  
}


//Selects random from 1 to max and passes it
int random(int max)
{
static long seed = 100;
seed = (((seed * 214013L + 2531011L) >> 16) & 32767);

return ((seed % max) + 1);
}

//Checks for sudden movement in watch
static void tap_handler(AccelAxisType axis, int32_t direction) {
  
  APP_LOG(APP_LOG_LEVEL_INFO, "ROLL");
  send_roll();
}

//Roll page Press up, Increase max
void main_up_handler(ClickRecognizerRef recognizer, void *context){
  sharedDice  = sharedDice +1;
  text(sharedDice);
}
//Roll page Press down, Decrease max, minimum of 0
void main_down_handler(ClickRecognizerRef recognizer, void *context){
  if(sharedDice<3){
    text(2);
  }else{
    sharedDice = sharedDice -1;
    text(sharedDice);
  }
}

//Runs send roll function
void main_rnd_handler(ClickRecognizerRef recognizer, void *context){
  send_roll();
}

//Main window button checks
void config_provider(Window *window){
  window_single_click_subscribe(BUTTON_ID_UP, main_anything_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, main_anything_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, main_anything_handler);
}

//Roll screen button checks
void config_providerv2(Window *window){
  window_single_click_subscribe(BUTTON_ID_UP, main_up_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, main_down_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, main_rnd_handler);
}

//Initalises app, Opens main window
static void init(){
  // Something Something Something Darkside  
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  window_set_click_config_provider(s_main_window, (ClickConfigProvider) config_provider);
  
}

//Deinitalsises i guess, this was also in example so you know
static void deinit(){
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
}


//This is god
int main(void){
  init();
  app_event_loop();
  deinit();
}