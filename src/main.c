#include <pebble.h>
#include "main.h"

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_ERROR 2
#define KEY_MILITARY_TIME 3
#define KEY_TEMPC 4
#define KEY_DATEDDMM 5

// Pebble Screen is 144 x 168

GFont s_orbitron_font_36;
GFont s_orbitron_font_20;
GFont s_orbitron_font_15;
static Window *window;
// Time text layers
static TextLayer *hours_1st_layer, *hours_2nd_layer, *minutes_1st_layer, *minutes_2nd_layer, 
  *seconds_1st_layer, *seconds_2nd_layer, *date_1st_layer, *date_2nd_layer, *date_delimiter_layer, *date_3rd_layer, *date_4th_layer,
  *day_of_week_layer, *s_weather_layer, *s_battery_info_layer;
// Battery ico
static BitmapLayer *s_battery_lightning_layer;
static GBitmap *s_battery_lightning_bmp;
static int s_battery_level;

// Time angles decorations
static BitmapLayer *s_time_angles_layer;
static GBitmap *s_time_angles_bmp;
// Seconds arows
static BitmapLayer *s_seconds_arows_layer;
static GBitmap *s_seconds_arows_bmp;

// Time format
static bool militaryTime = true;
// Temperature format
static bool tempC = true;
// Date format
static bool dateDDMM = true;

// Weather countdown
static const int weatherCountdownInit = 1800; // Initial value
static int weatherCountdown; // Time to re-read weather values

// Redraw time when it's changed (every second)
void handle_timechanges(struct tm *tick_time, TimeUnits units_changed){
  // TimeUnits to redraw just part of screen
  // Buffer where we write time
  static char time_buffer[17];
  
  if (militaryTime) {
    strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S %d.%m %w", tick_time);
  } else {
    strftime(time_buffer, sizeof(time_buffer), "%I:%M:%S %d.%m %w", tick_time);
  }
  
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
  //day_1st_digit[0] = '0';
  day_1st_digit[1] = '\0';
  
  static char day_2nd_digit[2];
  day_2nd_digit[0] = time_buffer[10];
  //day_2nd_digit[0] = '0';
  day_2nd_digit[1] = '\0';
  
  static char date_delimiter[2];
  date_delimiter[0] = '/';
  date_delimiter[1] = '\0';
  
  static char month_1st_digit[2];
  month_1st_digit[0] = time_buffer[12];
  //month_1st_digit[0] = '0';
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
  
  // Drawing date according format.
  if (dateDDMM){
    text_layer_set_text(date_1st_layer, day_1st_digit);
    text_layer_set_text(date_2nd_layer, day_2nd_digit);
    text_layer_set_text(date_3rd_layer, month_1st_digit);
    text_layer_set_text(date_4th_layer, month_2nd_digit);
  } else {
    text_layer_set_text(date_1st_layer, month_1st_digit);
    text_layer_set_text(date_2nd_layer, month_2nd_digit);
    text_layer_set_text(date_3rd_layer, day_1st_digit);
    text_layer_set_text(date_4th_layer, day_2nd_digit);
  }

  text_layer_set_text(date_delimiter_layer, date_delimiter);
  text_layer_set_text(day_of_week_layer, day_of_week);
  
  // Get weather update
  if (weatherCountdown > 0) {
    weatherCountdown -= 1;
  } else {
    get_weather();
    weatherCountdown = weatherCountdownInit;
  }
  
  // Draw battery state
  static char battery_buffer[5];
  snprintf(battery_buffer, sizeof(battery_buffer), "%02d%%", s_battery_level);
  text_layer_set_text(s_battery_info_layer, battery_buffer);
}

// Send weather request
static void get_weather() {
  // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
    
    // Send the message
    app_message_outbox_send();
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
        if (tempC){
          snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);
        } else {
          int tempF = (int)t->value->int32 * 1.8 + 32;
          snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", tempF);
        }
        
        break;
      case KEY_CONDITIONS:
        snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
        break;
      case KEY_ERROR:
        snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
        break;
      case KEY_MILITARY_TIME:
        if (t->value->int8 == 102 || t->value->int8 == 0){
          militaryTime = false;
          
        } else {
          militaryTime = true;
        }
      
        persist_write_bool(KEY_MILITARY_TIME, militaryTime);
        break;
      case KEY_TEMPC:
        if (t->value->int8 == 102 || t->value->int8 == 0){
          tempC = false;
          
        } else {
          tempC = true;
        }

        get_weather(); // Reread weather
        persist_write_bool(KEY_TEMPC, tempC);
        break;
      case KEY_DATEDDMM:
        if (t->value->int8 == 102 || t->value->int8 == 0){
          dateDDMM = false;
          
        } else {
          dateDDMM = true;
        }
        APP_LOG(APP_LOG_LEVEL_ERROR, "Date format %d", (int)t->key);
      
        persist_write_bool(KEY_DATEDDMM, dateDDMM);
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
  // Print unsuccess message
  static char message_buffer[32];
  snprintf(message_buffer, sizeof(message_buffer), "%s", "--//--");
  text_layer_set_text(s_weather_layer, message_buffer);
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

// Callback called when message sent successfully
static void outbox_sent_callback(DictionaryIterator *iterator, void *context){
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


///// Battery info /////
// Battery state callback
static void battery_callback(BatteryChargeState state) {
  // Get battery level
  s_battery_level = state.charge_percent;
  
  if(state.is_charging) {
    // Draw charging ico
  }
}

///// Program initializers /////

// Program initializer
void init(void){
  // Create a window and text layer
  window = window_create();
  window_set_background_color(window, GColorBlack);
  
  // Read settings
  if (persist_exists(KEY_MILITARY_TIME)) {
    militaryTime = persist_read_bool(KEY_MILITARY_TIME);
  }
  
  if (persist_exists(KEY_TEMPC)) {
    tempC = persist_read_bool(KEY_TEMPC);
  }
  
  if (persist_exists(KEY_DATEDDMM)) {
    dateDDMM = persist_read_bool(KEY_DATEDDMM);
  }
  
  // Initialize font for time
  s_orbitron_font_36 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_LIGHT_36));
  s_orbitron_font_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_LIGHT_20));
  s_orbitron_font_15 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ORBITRON_LIGHT_15));
  
  // Initialize time angles decorations
  s_time_angles_bmp = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIME_ANGLES);
  s_seconds_arows_layer = bitmap_layer_create(GRect(0, 56, 144, 38));
  bitmap_layer_set_bitmap(s_seconds_arows_layer, s_time_angles_bmp);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_seconds_arows_layer));
  
  // Initialize secnods arows decorations
  s_time_angles_bmp = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SECOND_AROWS1);
  s_time_angles_layer = bitmap_layer_create(GRect(43, 107, 55, 5));
  bitmap_layer_set_bitmap(s_time_angles_layer, s_time_angles_bmp);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_time_angles_layer));
  
  // Initialize hours layers
  init_text_layer(&hours_1st_layer, GRect(4, 54, 32, 36), s_orbitron_font_36);
  init_text_layer(&hours_2nd_layer, GRect(35, 54, 32, 36), s_orbitron_font_36);
  
  init_text_layer(&minutes_1st_layer, GRect(75, 54, 32, 36), s_orbitron_font_36);
  init_text_layer(&minutes_2nd_layer, GRect(105, 54, 32, 36), s_orbitron_font_36);
  
  init_text_layer(&seconds_1st_layer, GRect(53, 95, 18, 20), s_orbitron_font_20);
  init_text_layer(&seconds_2nd_layer, GRect(71, 95, 18, 20), s_orbitron_font_20);
  
  init_text_layer(&date_1st_layer, GRect(6, 140, 18, 20), s_orbitron_font_20);
  init_text_layer(&date_2nd_layer, GRect(19, 140, 28, 20), s_orbitron_font_20);
  
  init_text_layer(&date_delimiter_layer, GRect(35, 140, 28, 20), s_orbitron_font_20);
      
  init_text_layer(&date_3rd_layer, GRect(57, 140, 18, 20), s_orbitron_font_20);
  init_text_layer(&date_4th_layer, GRect(75, 140, 18, 20), s_orbitron_font_20);
  
  init_text_layer(&day_of_week_layer, GRect(98, 140, 40, 20), s_orbitron_font_20);

  // Initialize weather layer
  s_weather_layer = text_layer_create(GRect(0, -2, 144, 25));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weather_layer, s_orbitron_font_20);
  text_layer_set_text(s_weather_layer, "Loading...");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
   weatherCountdown = weatherCountdownInit; // Time to re-read weather values
  
  // Initialize battery lightning
  s_battery_lightning_bmp = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_LIGHTNING);
  s_battery_lightning_layer = bitmap_layer_create(GRect(40, 39, 17, 14));
  bitmap_layer_set_bitmap(s_battery_lightning_layer, s_battery_lightning_bmp);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_battery_lightning_layer));

  // Initialize battery layer
  s_battery_info_layer = text_layer_create(GRect(60, 36, 47, 25));
  text_layer_set_background_color(s_battery_info_layer, GColorClear);
  text_layer_set_text_color(s_battery_info_layer, GColorWhite);
  text_layer_set_text_alignment(s_battery_info_layer, GTextAlignmentLeft);
  text_layer_set_font(s_battery_info_layer, s_orbitron_font_15);
  text_layer_set_text(s_battery_info_layer, "--%");
  
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
  
  // Register battery state callback
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek()); // get initial value
  
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
  text_layer_destroy(date_1st_layer);
  text_layer_destroy(date_2nd_layer);
  text_layer_destroy(date_3rd_layer);
  text_layer_destroy(date_4th_layer);
  text_layer_destroy(day_of_week_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_battery_info_layer);
  text_layer_destroy(date_delimiter_layer);
  
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