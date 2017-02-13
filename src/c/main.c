#include <pebble.h>

//APP_LOG(APP_LOG_LEVEL_DEBUG, "LOG MESSAGE");

// window layers
static Window *main_window;
static TextLayer *location_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;

// image layers
static BitmapLayer *splash_layer;
static GBitmap *splash_bitmap;

// saved settings
uint32_t hour_setting   = 0;
uint32_t date_setting   = 1;
uint32_t degree_setting = 2;
bool hour_bool;
bool date_bool;
bool degree_bool;

// load date/time
static char time12_buffer[6];
static char time24_buffer[6];
static char datedm_buffer[17];
static char datemd_buffer[17];

// load weather
static char location_cel_buffer[19];
static char location_fah_buffer[19];

// load options
static void load_options() {
  // load 24 or 12 hours
  if (persist_exists(hour_setting)) {
    char hour_buffer[5];
    persist_read_string(hour_setting, hour_buffer, sizeof(hour_buffer));
    if (strcmp(hour_buffer, "true") == 0) {
      hour_bool = true;
      text_layer_set_text(time_layer, time12_buffer);
    } else {
      hour_bool = false;
      text_layer_set_text(time_layer, time24_buffer);
    }
  } else {
    hour_bool = false;
  }

  // load date order
  if (persist_exists(date_setting)) {
    char date_buffer[5];
    persist_read_string(date_setting, date_buffer, sizeof(date_buffer));
    if (strcmp(date_buffer, "true") == 0) {
      date_bool = true;
      text_layer_set_text(date_layer,  datemd_buffer);
    } else {
      date_bool = false;
      text_layer_set_text(date_layer,  datedm_buffer);
    }
  } else {
    date_bool = false;
  }

  // load location
  if (persist_exists(degree_setting)) {
    char degree_buffer[5];
    persist_read_string(degree_setting, degree_buffer, sizeof(degree_buffer));
    if (strcmp(degree_buffer, "true") == 0) {
      degree_bool = true;
      if (strlen(location_cel_buffer) != 0) {
        text_layer_set_text(location_layer, location_cel_buffer);
      } else {
        text_layer_set_text(location_layer, "pebble world");
      }
    } else {
      degree_bool = false;
      if (strlen(location_fah_buffer) != 0) {
        text_layer_set_text(location_layer, location_fah_buffer);
      } else {
        text_layer_set_text(location_layer, "pebble world");
      }
    }
  } else {
    degree_bool = false;
  }
}

// update options/weather
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // collect options
  Tuple *hour_tuple   = dict_find(iterator, MESSAGE_KEY_HOUR);
  Tuple *date_tuple   = dict_find(iterator, MESSAGE_KEY_DATE);
  Tuple *degree_tuple = dict_find(iterator, MESSAGE_KEY_DEGREE);

  // save options
  if(hour_tuple && date_tuple && degree_tuple) {
    char *hour_string   = hour_tuple->value->cstring;
    char *date_string   = date_tuple->value->cstring;
    char *degree_string = degree_tuple->value->cstring;
    persist_write_string(hour_setting,   hour_string);
    persist_write_string(date_setting,   date_string);
    persist_write_string(degree_setting, degree_string);
    load_options();
  }

  // collect weather
  Tuple *location_tuple = dict_find(iterator, MESSAGE_KEY_LOCATION);
  Tuple *temp_cel_tuple = dict_find(iterator, MESSAGE_KEY_TEMP_CEL);
  Tuple *temp_fah_tuple = dict_find(iterator, MESSAGE_KEY_TEMP_FAH);

  // display location
  if (location_tuple && temp_cel_tuple && temp_fah_tuple) {
    static char location_buffer[12], temp_fah_buffer[7], temp_cel_buffer[6];
    snprintf(location_buffer, sizeof(location_buffer), "%s", location_tuple->value->cstring);
    snprintf(temp_fah_buffer, sizeof(temp_fah_buffer), "%d°F", (int)temp_fah_tuple->value->int32);
    snprintf(temp_cel_buffer, sizeof(temp_cel_buffer), "%d°C", (int)temp_cel_tuple->value->int32);
    snprintf(location_fah_buffer, sizeof(location_fah_buffer), "%s", location_buffer);
    snprintf(location_cel_buffer, sizeof(location_cel_buffer), "%s", location_buffer);
    int fah_length = strlen(location_fah_buffer);
    int cel_length = strlen(location_cel_buffer);
    snprintf(location_fah_buffer+fah_length, (sizeof location_fah_buffer) - fah_length, " %s", temp_fah_buffer);
    snprintf(location_cel_buffer+cel_length, (sizeof location_cel_buffer) - cel_length, " %s", temp_cel_buffer);
    if (!degree_bool) {
      text_layer_set_text(location_layer, location_fah_buffer);
    } else {
      text_layer_set_text(location_layer, location_cel_buffer);
    }
  }
}

// update date/time
static void update_datetime() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // define date/time
  strftime(time24_buffer,  sizeof(time24_buffer),  "%H:%M", tick_time);
  strftime(time12_buffer,  sizeof(time12_buffer),  "%I:%M", tick_time);
  strftime(datedm_buffer,  sizeof(datedm_buffer),  "%A %d %b", tick_time);
  strftime(datemd_buffer,  sizeof(datemd_buffer),  "%A %b %d", tick_time);

  // display time
  if (!hour_bool) {
    text_layer_set_text(time_layer, time24_buffer);
  } else {
    text_layer_set_text(time_layer, time12_buffer);
  }

  // display date
  if (!date_bool) {
    text_layer_set_text(date_layer, datedm_buffer);
  } else {
    text_layer_set_text(date_layer, datemd_buffer);
  }
}
static void mins_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_datetime();
}

// window load
static void main_window_load(Window *window) {
  // collect window size
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int mx = bounds.size.w;
  int my = bounds.size.h;
  int cy = bounds.size.h/2;

  // location layer
  location_layer = text_layer_create(GRect(0,cy-86,mx,my));
  text_layer_set_background_color(location_layer, GColorClear);
  text_layer_set_text_color(location_layer, GColorWhite);
  text_layer_set_text_alignment(location_layer, GTextAlignmentCenter);
  text_layer_set_font(location_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(location_layer, "pebble world");
  layer_add_child(window_layer, text_layer_get_layer(location_layer));
  
  // splash layer
  splash_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SPLASH);
  splash_layer = bitmap_layer_create(GRect(0,cy-111,mx,my));
  bitmap_layer_set_bitmap(splash_layer, splash_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(splash_layer));
  
  // time layer
  time_layer = text_layer_create(GRect(0,cy+11,mx,my));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  // date layer
  date_layer  = text_layer_create(GRect(0,cy+66,mx,my));
  text_layer_set_background_color(date_layer, GColorBlack);
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(date_layer));
}

// window unload
static void main_window_unload(Window *window) {
  // destroy image layers
  bitmap_layer_destroy(splash_layer);
  gbitmap_destroy(splash_bitmap);
  // destroy text layers
  text_layer_destroy(date_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(location_layer);
}

// init
static void init() {
  // create window
  main_window = window_create();
  window_set_background_color(main_window, GColorBlack);

  // load/unload window
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // send window to screen
  window_stack_push(main_window, true);
  
  // load options
  load_options();
  
  // update date/time
  update_datetime();
  tick_timer_service_subscribe(MINUTE_UNIT, mins_tick_handler);

  // update options/weather
  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(64,0);  
}

// deinit
static void deinit() {
  // unsubscribe from events
  tick_timer_service_unsubscribe();
  // destroy window
  window_destroy(main_window);
}

// main
int main(void) {
  init();
  app_event_loop();
  deinit();
}