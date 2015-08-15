#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
//static TextLayer *s_battery_layer; //text layer for battery percentage
static TextLayer *s_date_layer; //Layer layer for current date 
static TextLayer *s_dow_layer; //Layer layer for current date
static TextLayer *s_disconnect_layer;
static GFont *s_time_font,*s_date_font,*s_battery_font;
static GBitmap *s_frame_1,*s_frame_2,*s_frame_3;
static BitmapLayer *s_bg_layer;
int fade_sec =0; //set time since last fade to 0
int nextFrame=2;
int f_delay =3000;
int g_interval =18; //glitces every 'g_interval' seconds
bool vib_hour =false;
int last_hour =24; //that's not possible!
bool batflash =true;
bool wasDisconnected =false; //previouly disconnected? 


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00 : 00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  
  // push the string to the layer
  //text_layer_set_text(s_time_layer, time);
  
  //--- DATE CODE---
  // Need to be static because they're used by the system later.
  static char s_date_text[] = "Xxxx 00";
  
  strftime(s_date_text, sizeof(s_date_text), "%b %e", tick_time);
 
  text_layer_set_text(s_date_layer, s_date_text);  
  
  //update day of week!
  static char week_day[] = "Xxx,";
  strftime(week_day,sizeof(week_day),"%a,",tick_time);
  text_layer_set_text(s_dow_layer,week_day);
}




  static void battery_handler(BatteryChargeState new_state) {
  // Write to buffer and display
  static char s_battery_buffer[32];  //char for the battery percent
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", new_state.charge_percent); //get he battery info, and add the approprate text
  //text_layer_set_text(s_battery_layer, s_battery_buffer); //set the layer battery text (percent + text) to the battery layer
  }


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
    //Create GFont for clock| Font: Control Freak [by Apostrophic Labs ]
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_time_48));
  
  //create font for date/dow  |Font: NovaSquare [by wmk69 ]
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_date_22));
  
  
  //Create background
  s_frame_1 = gbitmap_create_with_resource(RESOURCE_ID_mm_frame_1);
  s_frame_2 = gbitmap_create_with_resource(RESOURCE_ID_mm_frame_2);
  s_frame_3 = gbitmap_create_with_resource(RESOURCE_ID_mm_frame_3);
  s_bg_layer = bitmap_layer_create(window_bounds);
  bitmap_layer_set_bitmap(s_bg_layer,  s_frame_2 );
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bg_layer));
  
  // Create Time 
  s_time_layer = text_layer_create(GRect(1, 12, 148, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer,GColorWhite); /// black text
  text_layer_set_text(s_time_layer, "00:00");
  

  
  /**
  //battery setup
  s_battery_layer = text_layer_create(GRect(20, 150, window_bounds.size.w, window_bounds.size.h));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentLeft); //left align the text in this layer
  text_layer_set_background_color(s_battery_layer, GColorClear); //clear background
  #ifdef PBL_PLATFORM_APLITE //if running on older pebble
    text_layer_set_text_color(s_battery_layer, GColorWhite); /// black text
   #elif PBL_PLATFORM_BASALT  //if running on pebble time
    text_layer_set_text_color(s_battery_layer,GColorBulgarianRose); //red text
  #endif
  text_layer_set_font(s_battery_layer, s_battery_font); //change the font
  layer_add_child(window_layer, text_layer_get_layer(s_battery_layer));//Add battery layer to our window
 
  // Get the current battery level
  battery_handler(battery_state_service_peek());
  **/
  
  
  //Create Date layer:
  //s_date_layer = text_layer_create(GRect(20,49, 136, 100)); old
  s_date_layer = text_layer_create(GRect(69,0,130,100));
  text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer,GColorWhite); /// black text
  
  text_layer_set_font(s_date_layer,  s_date_font);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  
  //Create 'DAY OF WEEK layer:
 // s_dow_layer = text_layer_create(GRect(83,100, 136, 100)); old
  s_dow_layer = text_layer_create(GRect(5,0, 136, 100));
  text_layer_set_background_color(s_dow_layer, GColorClear);
  text_layer_set_text_color(s_dow_layer,  GColorWhite); /// black text

  
  text_layer_set_font(s_dow_layer, s_date_font);
  layer_add_child(window_layer, text_layer_get_layer(s_dow_layer));
 




  //time
  //Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  

  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  //Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  fonts_unload_custom_font(s_battery_font);
  
  //Destroy GBitmap
  gbitmap_destroy(s_frame_1);
  gbitmap_destroy(s_frame_2);
  gbitmap_destroy(s_frame_3);




  //Destroy BitmapLayer
  bitmap_layer_destroy(s_bg_layer);

  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  //text_layer_destroy(s_battery_layer);
  text_layer_destroy(s_dow_layer);
  
}


void look(void * data){  
    
     
    int delay = f_delay;

    if (nextFrame == 1)
      {
        bitmap_layer_set_bitmap(s_bg_layer,s_frame_1);
        layer_mark_dirty(bitmap_layer_get_layer(s_bg_layer));
        nextFrame =2;
        app_timer_register(delay, look, NULL);
      }
    else if (nextFrame == 2 || nextFrame ==4)
      {
        bitmap_layer_set_bitmap(s_bg_layer,s_frame_2);
        layer_mark_dirty(bitmap_layer_get_layer(s_bg_layer));
        if (nextFrame == 2)
          nextFrame =3;
        if (nextFrame == 4)
          nextFrame =1;
        app_timer_register(delay, look, NULL);     
     }
    else if (nextFrame == 3)
      {
        bitmap_layer_set_bitmap(s_bg_layer,s_frame_3);
        layer_mark_dirty(bitmap_layer_get_layer(s_bg_layer));
        nextFrame =4;
        app_timer_register(delay, look, NULL);     
     }
}  


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    fade_sec ++;//time has passed since last blink
    int seconds = tick_time->tm_sec;
    int hours = tick_time ->tm_hour;

  /** TODO
    //Bluetooth Diconnect handler
    if (bluetooth_connection_service_peek() ==false && wasDisconnected==false){ //if we WERE connected, and now are not, then do the following
    layer_set_hidden( (Layer *) s_disconnect_layer,false);  //show it on the screen
    wasDisconnected = true; //at last check, we WERE disconnected, 
    vibes_double_pulse(); //double vibrate, so they will know it's not a normal notication.
    }
  
   if (bluetooth_connection_service_peek() ==true && wasDisconnected ==true){ //if we WERE DISconnected (last we checked), and now are connected again, then do the following
    layer_set_hidden( (Layer *) s_disconnect_layer,true);//hide the layer
    wasDisconnected = false;// at last check  (now) we WERE connected
    // layer_mark_dirty(bitmap_layer_get_layer( s_disconnected_layer)); 
   }
  **/
    if (last_hour == 24) //if at the last update the hour was 24, then we just started the watch
      last_hour=hours; //our new 'last hour' will be the current time
    

    if (hours != last_hour && vib_hour ==true) //if hour has changed
     {
        vibes_short_pulse(); //vibrate (short) 
        last_hour=hours;//our new 'last hour' will be the current time   
      }
   

    if (hours != last_hour && vib_hour ==false) //if hour has changed
     {
        last_hour=hours;//our new 'last hour' will be the current time   
      }
  
   // if (min == 59 && seconds == 59)
     // app_timer_register(500, glitch_hour_ani, NULL); //start the hour glitch

   // if (seconds ==59)
   //  app_timer_register(500, glitch_min_ani, NULL); //start the min glitch
  
    if (seconds == 0)
      {
      update_time();
    }

}
  

static void in_recv_handler(DictionaryIterator *iterator, void *context)
{
  //Get Tuple
  Tuple *t = dict_read_first(iterator);
  if(t)
  {
    switch(t->key)
    {
    case KEY_SPEED:
      //It's the KEY_INVERT key
      if(strcmp(t->value->cstring, "on") == 0)
      {
        //Set and save as inverted
        text_layer_set_text_color(text_layer, GColorWhite);
        text_layer_set_background_color(text_layer, GColorBlack);
        text_layer_set_text(text_layer, "Inverted!");
 
        persist_write_bool( KEY_SPEED, true);
      }
      else if(strcmp(t->value->cstring, "off") == 0)
      {
        //Set and save as not inverted
        text_layer_set_text_color(text_layer, GColorBlack);
        text_layer_set_background_color(text_layer, GColorWhite);
        text_layer_set_text(text_layer, "Not inverted!");
 
        persist_write_bool( KEY_SPEED, false);
      }
      break;
    }
  }
}

  
  static void init() {

  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, (TickHandler) tick_handler);
  app_timer_register(0, look, NULL);
    if (speed == 'Choice 1')
      f_delay =3000;
    if (speed == 'Choice 2')
      f_delay =300;
  
}


static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
