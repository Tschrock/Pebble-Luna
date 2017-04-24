#include <pebble.h>
#include "indicators.h"

static Window *s_main_window;

static GRect bgPos;
static GRect timePos;
static GRect timePos2;

// Text (Clock)
static TextLayer *s_time_layer;
static TextLayer *s_time_layer2;

// Background
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

#ifdef PBL_BW
  static BitmapLayer *s_background_layer2;
  static GBitmap *s_background_bitmap2;
#endif


//
// Time Update functions
//

static void update_time() {

  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  
  #ifdef PBL_COLOR
    static char buffer[] = "00:00";
  #else
    static char buffer[] = "00";
  #endif
  
  static char buffer2[] = "00";

  if(clock_is_24h_style() == true) {
    #ifdef PBL_COLOR
      strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    #else
      strftime(buffer, sizeof("00"), "%H", tick_time);
    #endif
  } else {
    #ifdef PBL_COLOR
      strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    #else
      strftime(buffer, sizeof("00"), "%I", tick_time);
    #endif
  }

  strftime(buffer2, sizeof("00"), "%M", tick_time);
  

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_time_layer2, buffer2);
  
  //static char buffer3[] = "Thu Aug 23 2001";
  //strftime(buffer3, sizeof("Thu Aug 23 2001"), "%a %b %e %Y", tick_time);
  
  //text_layer_set_text(s_date_layer, buffer3);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


//
// Window functions
//

static void main_window_load(Window *window) {
  bgPos = GRect(0, 0, 144, 168);
  
  //
  // Background Setup
  //
  
  s_background_layer = bitmap_layer_create(bgPos);

  #ifdef PBL_COLOR
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG);
    bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  #else
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BG_BLACK);
    bitmap_layer_set_compositing_mode(s_background_layer, GCompOpClear);
  #endif
    
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    
  #ifdef PBL_BW
    s_background_layer2 = bitmap_layer_create(bgPos);
    s_background_bitmap2 = gbitmap_create_with_resource(RESOURCE_ID_BG_WHITE);
    
    bitmap_layer_set_compositing_mode(s_background_layer2, GCompOpOr);
    bitmap_layer_set_bitmap(s_background_layer2, s_background_bitmap2);
  
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer2));
  #endif
  
  //
  // Time Setup
  //
  
  
  #ifdef PBL_COLOR
    timePos = GRect(0, 110, 144, 50);
    timePos2 = GRect(144, 110, 60, 50);
  #else
    timePos = GRect(-3, 110, 62, 50);
    timePos2 = GRect(82, 110, 60, 50);
  #endif
  
  
  ResHandle font_handle = resource_get_handle(RESOURCE_ID_ROBOTO_BOLD_45);
  
  // Create time TextLayer
  s_time_layer = text_layer_create(timePos);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_load_custom_font(font_handle));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Create time TextLayer2
  s_time_layer2 = text_layer_create(timePos2);
  text_layer_set_background_color(s_time_layer2, GColorClear);
  text_layer_set_text_color(s_time_layer2, GColorWhite);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer2, fonts_load_custom_font(font_handle));
  text_layer_set_text_alignment(s_time_layer2, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer2));

  
  
  
  initIndicatorSettings();
  initBatteryIcon(window);
  initBluetoothIcon(window);
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  
  #ifdef PBL_BW
    gbitmap_destroy(s_background_bitmap2);
    bitmap_layer_destroy(s_background_layer2);
  #endif
  
  deinitBatteryIcon();
  deinitBluetoothIcon();
}



//
// Main App Setup/Loop
//
  
static void init() {
  s_main_window = window_create();
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
}

static void deinit() {
  window_destroy(s_main_window);
}
  
int main(void) {
  init();
  app_event_loop();
  deinit();
}