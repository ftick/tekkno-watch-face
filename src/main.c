#include <pebble.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_batt_layer;
static GFont s_time_font;
static GFont s_date_font;
static GFont s_batt_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static int count = 0;
static int flash = 0;

/***************************************************************************/

char * string_repeat( int n, const char * s )
{
	size_t slen = strlen(s);
	char * dest = malloc(n*slen+1);
	
	int i; char * p;
	for ( i=0, p = dest; i < n; ++i, p += slen )
    memcpy(p, s, slen);
  
	*p = '\0';
	return dest;
}

/***************************************************************************/

static void update_time()
{
  	// Get a tm structure
  	time_t temp = time(NULL); 
  	struct tm *tick_time = localtime(&temp);

  	// Create a long-lived buffer
  	static char buffer[] = "00:00";
  	static char buffer2[] = "Feb 01";

    // Write the current hours and minutes into the buffer
    if(clock_is_24h_style() == true)
	{
      	// Use 2h hour format
		if(count == 0 || flash != 1)
      		strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
		else
			strftime(buffer, sizeof("00:00"), "%H|%M", tick_time);
	}
    else
	{
     	 // Use 12 hour format
		if(count == 0 || flash != 1)
      		strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
		else
			strftime(buffer, sizeof("00:00"), "%I|%M", tick_time);
	}
    
	count = (count + 1) % 2;
	
  	strftime(buffer2, sizeof("Jan 01"), "%b %d", tick_time);

  	// Display this time on the TextLayer
  	text_layer_set_text(s_time_layer, buffer);
  	text_layer_set_text(s_date_layer, buffer2);
}

/***************************************************************************/

char* concat(char *s1, char *s2)
{
	// Simple concatenation function
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = malloc(len1+len2+1);
	
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);
    return result;
}

/***************************************************************************/

static void tap_handler(AccelAxisType axis, int32_t direction) {
	switch (axis) {
		case ACCEL_AXIS_X:
		if (direction > 0)
		{
			APP_LOG(APP_LOG_LEVEL_INFO, "X axis positive.");
			flash = (flash + 1) % 2;
		}
		else
		{
			APP_LOG(APP_LOG_LEVEL_INFO, "X axis negative.");
			flash = (flash + 1) % 2;
		}
		
		break;
		
		case ACCEL_AXIS_Y:
		if (direction > 0)
		{
			APP_LOG(APP_LOG_LEVEL_INFO, "Y axis positive.");
			flash = (flash + 1) % 2;
		}
		else
		{
			APP_LOG(APP_LOG_LEVEL_INFO, "Y axis negative.");
			flash = (flash + 1) % 2;
		}
		
		break;
		
		case ACCEL_AXIS_Z:
		if (direction > 0)
		{
			APP_LOG(APP_LOG_LEVEL_INFO, "Z axis positive.");
			flash = (flash + 1) % 2;
		}
		else
		{
			APP_LOG(APP_LOG_LEVEL_INFO, "Z axis negative.");
			flash = (flash + 1) % 2;
		}
		
		break;
	}
}

/***************************************************************************/

static void battery_handler(BatteryChargeState new_state)
{
  	// Write to buffer and display
  	static char s_battery_buffer[] = "**********";
  	
	int mult;
  	char* left;
  	char* done;
  	char* str = "";
		
  	mult = new_state.charge_percent / 10;
  	left = string_repeat(mult, "@");
  	done = string_repeat(10-mult, " ");
  	
  	str = concat(left, done);
	
	snprintf(s_battery_buffer, sizeof(s_battery_buffer), str);
  	text_layer_set_text(s_batt_layer, s_battery_buffer);
  
  	free(str);
}

/***************************************************************************/

static void main_window_load(Window *window) 
{
	// Create background GBitmap, then set to created BitmapLayer
  	s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  	s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  
  	// Create time TextLayer
  	s_time_layer = text_layer_create(GRect(0, 65, 144, 50));
  	text_layer_set_background_color(s_time_layer, GColorClear);
  	text_layer_set_text_color(s_time_layer, GColorBlack);
	
  	// Create date TextLayer
  	s_date_layer = text_layer_create(GRect(0, 15, 144, 50));
  	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_text_color(s_date_layer, GColorWhite);
	
  	// Create battery TextLayer
  	s_batt_layer = text_layer_create(GRect(7, 125, 144, 168));
  	text_layer_set_background_color(s_batt_layer, GColorClear);
	text_layer_set_text_color(s_batt_layer, GColorWhite);
  
  	// Create GFonts
  	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CON_32));
  	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CON_22));
  	s_batt_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CON_14));
  
  	// Apply to Layer
  	bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  	text_layer_set_font(s_time_layer, s_time_font);
  	text_layer_set_font(s_date_layer, s_date_font);
  	text_layer_set_font(s_batt_layer, s_batt_font);
  
  	// Improve the layout to be more like a watchface
  	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  	text_layer_set_text_alignment(s_batt_layer, GTextAlignmentLeft);

  	// Add it as a child layer to the Window's root layer
  	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_batt_layer));
  
  	// Make sure the time is displayed from the start
  	update_time();
  	battery_handler(battery_state_service_peek());
	accel_tap_service_subscribe(tap_handler);
}

/***************************************************************************/

static void main_window_unload(Window *window)
{
  	// Destroy TextLayers
  	text_layer_destroy(s_time_layer);
  	text_layer_destroy(s_date_layer);
  	text_layer_destroy(s_batt_layer);
  
	// Unload GFonts
  	fonts_unload_custom_font(s_time_font);
  	fonts_unload_custom_font(s_date_font);
  	fonts_unload_custom_font(s_batt_font);
  
  	// Destroy GBitmap
  	gbitmap_destroy(s_background_bitmap);
  	// Destroy BitmapLayer
  	bitmap_layer_destroy(s_background_layer);
}

/***************************************************************************/

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  	update_time();
}

/***************************************************************************/

static void init()
{
  	// Create main Window element and assign to pointer
  	s_main_window = window_create();

  	// Set handlers to manage the elements inside the Window
  	window_set_window_handlers(s_main_window, (WindowHandlers)
	  {
    	.load = main_window_load,
    	.unload = main_window_unload
	  });

  	// Show the Window on the watch, with animated=true
  	window_stack_push(s_main_window, true);
  
  	// Register with TickTimerService
  	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  	// Subscribe to the Battery State Service
  	battery_state_service_subscribe(battery_handler);
}

/***************************************************************************/

static void deinit()
{
  	// Destroy Window
  	window_destroy(s_main_window);
}

/***************************************************************************/

int main(void)
{
  	init();
  	app_event_loop();
  	deinit();
}
