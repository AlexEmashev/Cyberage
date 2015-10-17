#include <pebble.h>
#include "main.h"
// Define "magic numbers" for temperature and weather conditions
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
// Pebble Screen is 144 x 168

GFont s_orbitron_font_36;
GFont s_orbitron_font_20;
GFont s_orbitron_font_15;
static Window *window;
// Time text layers
static TextLayer *hours_1st_layer, *hours_2nd_layer, *minutes_1st_layer, *minutes_2nd_layer, 
  *seconds_1st_layer, *seconds_2nd_layer, *day_1st_layer, *day_2nd_layer, *month_1st_layer, *month_2nd_layer,
  *day_of_week_layer, *s_weather_layer, *s_battery_info_layer;
// Battery ico
static BitmapLayer *s_battery_lightning_layer;
static GBitmap *s_battery_lightning_bmp;
// Time angles decorations
static BitmapLayer *s_time_angles_layer;
static GBitmap *s_time_angles_bmp;
// Seconds arows
static BitmapLayer *s_seconds_arows_layer;
static GBitmap *s_seconds_arows_bmp;

// Redraw time when it's changed (every second)
void handle_timechanges(struct tm *tick_time, TimeUnits units_changed){
  // TimeUnits to redraw just part of screen
  // Buffer where we write time
  static char time_buffer[17];
  
  // Get time in format "hh:mm:ss dd.mm w"
  strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S %d.%m %w", tick_time);
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
  //seconds_1st_digit[0] = '8'; // debug line
  seconds_1st_digit[1] = '\0';
  
  static char seconds_2nd_digit[2];
  seconds_2nd_digit[0] = time_buffer[7];
  //seconds_2nd_digit[0] = '8';
  seconds_2nd_digit[1] = '\0';
  
  static char day_1st_digit[2];
  day_1st_digit[0] = time_buffer[9];
  //day_1st_digit[0] = '8';
  day_1st_digit[1] = '\0';
  
  static char day_2nd_digit[3];
  day_2nd_digit[0] = time_buffer[10];
  //day_2nd_digit[0] = '8';
  day_2nd_digit[1] = '/';
  day_2nd_digit[2] = '\0';
  
  static char month_1st_digit[2];
  month_1st_digit[0] = time_buffer[12];
  //month_1st_digit[0] = '8';
  month_1st_digit[1] = '\0';
  
  static char month_2nd_digit[2];
  month_2nd_digit[0] = time_buffer[13];
  //month_2nd_digit[0] = '8';
  month_2nd_digit[1] = '\0';
  
  static char day_of_week_id[2];
  day_of_week_id[0] = time_buffer[15];
  static char day_of_week[3];
    
  switch (day_of_week_id[0]){
    case '0':
    {
      strcpy(day_of_week, "SU");
      break;
    }
    case '1':
    {
      strcpy(day_of_week, "MO");
      break;
    }
    case '2':
    {
      strcpy(day_of_week, "TU");
      break;
    }
    case '3':
    {
      strcpy(day_of_week, "WE");
      break;
    }
    case '4':
    {
      strcpy(day_of_week, "TH");
      break;
    }
    case '5':
    {
      strcpy(day_of_week, "FR");
      break;
    }
    case '6':
    {
      strcpy(day_of_week, "SA");
      break;
    }
    default:
     strcpy(day_of_week, "NA");    
  }
  
  // Draw time
  text_layer_set_text(hours_1st_layer, hours_1st_digit);
  text_layer_set_text(hours_2nd_layer, hours_2nd_digit);
  text_layer_set_text(minutes_1st_layer, minutes_1st_digit);
  text_layer_set_text(minutes_2nd_layer, minutes_2nd_digit);
  text_layer_set_text(seconds_1st_layer, seconds_1st_digit);
  text_layer_set_text(seconds_2nd_layer, seconds_2nd_digit);
  
  // Drawing date.
  text_layer_set_text(day_1st_layer, day_1st_digit);
  text_layer_set_text(day_2nd_layer, day_2nd_digit);
  text_layer_set_text(month_1st_layer, month_1st_digit);
  text_layer_set_text(month_2nd_layer, month_2nd_digit);
  text_layer_set_text(day_of_week_layer, day_of_week);
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0 && tick_time->tm_sec == 0){
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
    
    // Send the message
    app_message_outbox_send();
  }
}


///// AppMessage callbacks to talk to phone ///// 

// Callback for receiving messages from AppMessage API
static void inbox_received_callback(DictionaryIterator *iterator, void *context){
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);
  
  // For all items
  while(t != NULL){
    // Define which key was received
    switch(t->key){
      case KEY_TEMPERATURE:
        snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);
        break;
      case KEY_CONDITIONS:
        snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
    }

    t = dict_read_next(iterator);
  }
  
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), 
           "%s %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

// Callback called when watch dropped message
static void inbox_dropped_calback(AppMessageResult reason, void *context){
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

// Callback for failing to send a message from outox
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context){
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

// Callback called when message sent successfully
static void outbox_sent_callback(DictionaryIterator *iterator, void *context){
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


///// Program initializers /////

// Program initializer
void init(void){
  // Create a window and text layer
  window = window_create();
  window_set_background_color(window, GColorBlack);
  
  // Initialize font for time
  s_orbitron_font_36 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_LIGHT_36));
  s_orbitron_font_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_LIGHT_20));
  s_orbitron_font_15 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_LIGHT_15));
  
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

  // Initialize battery lightning
  s_battery_lightning_bmp = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_LIGHTNING);
  s_battery_lightning_layer = bitmap_layer_create(GRect(43, 23, 17, 14));
  bitmap_layer_set_bitmap(s_battery_lightning_layer, s_battery_lightning_bmp);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_battery_lightning_layer));
  
  // Initialize hours layers
  init_text_layer(&hours_1st_layer, GRect(4, 50, 32, 36), s_orbitron_font_36);
  init_text_layer(&hours_2nd_layer, GRect(35, 50, 32, 36), s_orbitron_font_36);
  
  init_text_layer(&minutes_1st_layer, GRect(75, 50, 32, 36), s_orbitron_font_36);
  init_text_layer(&minutes_2nd_layer, GRect(105, 50, 32, 36), s_orbitron_font_36);
  
  init_text_layer(&seconds_1st_layer, GRect(53, 93, 18, 20), s_orbitron_font_20);
  init_text_layer(&seconds_2nd_layer, GRect(71, 93, 18, 20), s_orbitron_font_20);
  
  init_text_layer(&day_1st_layer, GRect(6, 140, 18, 20), s_orbitron_font_20);
  init_text_layer(&day_2nd_layer, GRect(24, 140, 28, 20), s_orbitron_font_20);
  
  init_text_layer(&month_1st_layer, GRect(52, 140, 18, 20), s_orbitron_font_20);
  init_text_layer(&month_2nd_layer, GRect(70, 140, 18, 20), s_orbitron_font_20);
  
  init_text_layer(&day_of_week_layer, GRect(95, 140, 40, 20), s_orbitron_font_20);

  // Initialize weather layer
  s_weather_layer = text_layer_create(GRect(0, 2, 144, 25));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weather_layer, s_orbitron_font_20);
  text_layer_set_text(s_weather_layer, "Loading...");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));

  // Initialize battery layer
  s_battery_info_layer = text_layer_create(GRect(0, 2, 144, 25));
  text_layer_set_background_color(s_battery_info_layer, GColorClear);
  text_layer_set_text_color(s_battery_info_layer, GColorWhite);
  text_layer_set_text_alignment(s_battery_info_layer, GTextAlignmentLeft);
  text_layer_set_font(s_battery_info_layer, s_orbitron_font_15);
  text_layer_set_text(s_battery_info_layer, "100%");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_info_layer));
  
  // To launch time changing handler
  time_t now = time(NULL);
  handle_timechanges(localtime(&now), SECOND_UNIT);
  
  // Subscribe to time changing events
  tick_timer_service_subscribe(SECOND_UNIT, handle_timechanges);
  
  // Push the window
  window_stack_push(window, true);

  // Register callbacks for messages system AppMessage
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_calback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

// Initialize text layer shorthand
void init_text_layer(TextLayer **txt_layer, struct GRect grect, GFont font){
  *txt_layer = text_layer_create(grect);
  text_layer_set_font(*txt_layer, font);
  text_layer_set_background_color(*txt_layer, GColorClear);
  text_layer_set_text_color(*txt_layer, GColorWhite);
  text_layer_set_text_alignment(*txt_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(*txt_layer));
}

// Program deinitializer
void deinit(void){
  // Destroy the text layer
  text_layer_destroy(hours_1st_layer);
  text_layer_destroy(hours_2nd_layer);
  text_layer_destroy(minutes_1st_layer);
  text_layer_destroy(minutes_2nd_layer);
  text_layer_destroy(seconds_1st_layer);
  text_layer_destroy(seconds_2nd_layer);
  text_layer_destroy(day_1st_layer);
  text_layer_destroy(day_2nd_layer);
  text_layer_destroy(month_1st_layer);
  text_layer_destroy(month_2nd_layer);
  text_layer_destroy(day_of_week_layer);
  text_layer_destroy(s_weather_layer);
  
  // Destroy graphics
  gbitmap_destroy(s_time_angles_bmp);
  gbitmap_destroy(s_seconds_arows_bmp);
  bitmap_layer_destroy(s_time_angles_layer);
  bitmap_layer_destroy(s_seconds_arows_layer);
  gbitmap_destroy(s_battery_lightning_bmp);
  bitmap_layer_destroy(s_battery_lightning_layer);
  
  // Unload fonts
  fonts_unload_custom_font(s_orbitron_font_36);
  fonts_unload_custom_font(s_orbitron_font_20);
  fonts_unload_custom_font(s_orbitron_font_15);
  
  // Destroy the window
  window_destroy(window);
}

// Program entry point
int main(void){
  init();
  app_event_loop();
  deinit();
}