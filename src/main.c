#include <pebble.h>
#include "main.h"
// Pebble Screen is 144 x 168

GFont s_orbitron_font_36;
GFont s_orbitron_font_24;
GFont s_orbitron_font_20;
static Window *window;
static TextLayer  *minutes_layer, *seconds_layer;
static TextLayer *date_layer;
// Time text layers
static TextLayer *hours_1st_layer, *hours_2nd_layer, *minutes_1st_layer, *minutes_2nd_layer, *seconds_1st_layer, *seconds_2nd_layer;
// Time angles decorations
static BitmapLayer *s_time_angles_layer;
static GBitmap *s_time_angles_bmp;
// Seconds arows
static BitmapLayer *s_seconds_arows_layer;
static GBitmap *s_seconds_arows_bmp;

void handle_timechanges(struct tm *tick_time, TimeUnits units_changed){
  // TimeUnits to redraw just part of screen
  // Buffer where we write time
  static char time_buffer[10];
  static char date_buffer[10];
  
  // Get time in format "hh:mm:ss"
  strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", tick_time);
  // Separate digits
  static char hours_1st_digit[2];
  hours_1st_digit[0] = time_buffer[0];
  hours_1st_digit[1] = '\0';
  
  static char hours_2nd_digit[2];
  hours_2nd_digit[0] = time_buffer[1];
  hours_2nd_digit[1] = '\0';
  
  static char minutes_1st_digit[2];
  minutes_1st_digit[0] = time_buffer[3];
  minutes_1st_digit[1] = '\0';
  
  static char minutes_2nd_digit[2];
  minutes_2nd_digit[0] = time_buffer[4];
  minutes_2nd_digit[1] = '\0';
  
  static char seconds_1st_digit[2];
  seconds_1st_digit[0] = time_buffer[6];
  //seconds_1st_digit[0] = '8';
  seconds_1st_digit[1] = '\0';
  
  static char seconds_2nd_digit[2];
  seconds_2nd_digit[0] = time_buffer[7];
  //seconds_2nd_digit[0] = '8';
  seconds_2nd_digit[1] = '\0';
  
  // Draw time
  text_layer_set_text(hours_1st_layer, hours_1st_digit);
  text_layer_set_text(hours_2nd_layer, hours_2nd_digit);
  text_layer_set_text(minutes_1st_layer, minutes_1st_digit);
  text_layer_set_text(minutes_2nd_layer, minutes_2nd_digit);
  text_layer_set_text(seconds_1st_layer, seconds_1st_digit);
  text_layer_set_text(seconds_2nd_layer, seconds_2nd_digit);

  
  // Drawing date.
  strftime(date_buffer, sizeof(date_buffer), "%D %e", tick_time);
  text_layer_set_text(date_layer, date_buffer);
}

void init(void){
  // Create a window and text layer
  window = window_create();
  window_set_background_color(window, GColorBlack);
  
  // Initialize font for time
  s_orbitron_font_36 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_LIGHT_36));
  s_orbitron_font_24 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_LIGHT_24));
  s_orbitron_font_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_LIGHT_20));
  
  // Initialize time angles decorations
  s_time_angles_bmp = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIME_ANGLES);
  s_seconds_arows_layer = bitmap_layer_create(GRect(0, 54, 144, 38));
  bitmap_layer_set_bitmap(s_seconds_arows_layer, s_time_angles_bmp);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_seconds_arows_layer));
  
  // Initialize secnods arows decorations
  s_time_angles_bmp = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SECOND_AROWS1);
  s_time_angles_layer = bitmap_layer_create(GRect(43, 103, 55, 5));
  bitmap_layer_set_bitmap(s_time_angles_layer, s_time_angles_bmp);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_angles_layer));
  
  // Setup hours layers
  hours_1st_layer = text_layer_create(GRect(4, 50, 32, 36));
  init_time_layer(hours_1st_layer);
  text_layer_set_font(hours_1st_layer, s_orbitron_font_36);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(hours_1st_layer));
  
  hours_2nd_layer = text_layer_create(GRect(35, 50, 32, 36));
  init_time_layer(hours_2nd_layer);
  text_layer_set_font(hours_2nd_layer, s_orbitron_font_36);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(hours_2nd_layer));
  
  // Setup minutes layers
  minutes_1st_layer = text_layer_create(GRect(75, 50, 32, 36));
  init_time_layer(minutes_1st_layer);
  text_layer_set_font(minutes_1st_layer, s_orbitron_font_36);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minutes_1st_layer));
  
  minutes_2nd_layer = text_layer_create(GRect(105, 50, 32, 36));
  init_time_layer(minutes_2nd_layer);
  text_layer_set_font(minutes_2nd_layer, s_orbitron_font_36);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(minutes_2nd_layer));
  
  // Setup seconds layers
  seconds_1st_layer = text_layer_create(GRect(51, 93, 18, 20));
  init_time_layer(seconds_1st_layer);
  text_layer_set_font(seconds_1st_layer, s_orbitron_font_20);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(seconds_1st_layer));
  
  seconds_2nd_layer = text_layer_create(GRect(69, 93, 18, 20));
  init_time_layer(seconds_2nd_layer);
  text_layer_set_font(seconds_2nd_layer, s_orbitron_font_20);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(seconds_2nd_layer));

  
  // Initialize date layer
  date_layer = text_layer_create(GRect(0, 142, 144, 56));
  // Adding layer to the window
  //layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_font(date_layer, s_orbitron_font_20);
  


  // To launch time changing handler
  time_t now = time(NULL);
  handle_timechanges(localtime(&now), SECOND_UNIT);
  
  // Subscribe to time changing events
  tick_timer_service_subscribe(SECOND_UNIT, handle_timechanges);
  
  // Push the window
  window_stack_push(window, true);
  
  // App Logging!
  APP_LOG(APP_LOG_LEVEL_DEBUG, "[handle_init] Hello World from the applogs!");
}

// Initialize text layer shorthand
void init_time_layer(TextLayer *txt_layer){
  text_layer_set_background_color(txt_layer, GColorClear);
  text_layer_set_text_color(txt_layer, GColorWhite);
  text_layer_set_text_alignment(txt_layer, GTextAlignmentRight);
}

void deinit(void){
  // Destroy the text layer
  text_layer_destroy(hours_1st_layer);
  
  // Destroy the window
  window_destroy(window);
  
  // Destroy date layer
  text_layer_destroy(date_layer);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}