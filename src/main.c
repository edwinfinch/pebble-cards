#include <pebble.h>
#include "content_layer.h"
#include "notification.h"
#include "extras.h"
#include "main.h"

void bt_handler(bool connected);
void battery_handler(BatteryChargeState state);

ContentLayer *card_weather, *card_bt, *card_battery, *card_note1, *card_note2;

void chat_bubble_proc(Layer *l, GContext *ctx){
	graphics_fill_rect(ctx, GRect(16, 52, 113, 72), 3, GCornersAll);
}

void battery_layer_proc(Layer *l, GContext *ctx){
	//34, 154
	for(int i = charge_percent/10; i > 0; i--){
		int x_fix = 26+(i*8);
		graphics_fill_rect(ctx, GRect(x_fix, 154, 5, 5), 0, GCornerNone);
	}
}

void card_time_root_proc(Layer *l, GContext *ctx){
	graphics_fill_rect(ctx, GRect(80, 105, 40, 18), 2, GCornersAll);
}

void circles_proc(Layer *l, GContext *ctx){
	if(amount_of_cards == 1){
		layer_set_hidden(l, 1);
	}
	else{
		layer_set_hidden(l, 0);
	}
	int i = 1;
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_fill_color(ctx, GColorWhite);
	for(i = amount_of_cards; i > 0; i--){
		int fix = 15+(i*10);
		if(i > 3){ //99v
			fix += 43;
		}
		graphics_draw_circle(ctx, GPoint(fix, 36), 3);
		if(card_f == i){
			graphics_fill_circle(ctx, GPoint(fix, 36), 3);
		}
	}
}

void tick_handler(struct tm *t, TimeUnits units){
	static char buffer[] = "00 00";
	if(clock_is_24h_style()){
		strftime(buffer, sizeof(buffer), "%H:%M", t);
   	}
   	else{
		strftime(buffer,sizeof(buffer), "%I:%M", t);
   	}	
	text_layer_set_text(time_layer, buffer);
	text_layer_set_text(card_time_layer, buffer);
	
	static char y_buffer[] = "on September 30th, 2014.";
	strftime(y_buffer, sizeof(y_buffer), "'%y", t);

	static char d_buffer[] = "Oct...";
	strftime(d_buffer, sizeof(d_buffer), "%d", t);

	static char final_buffer[] = "am September 30th, 2014..........";
	if(settings.language == 0){
		snprintf(final_buffer, sizeof(final_buffer), "%s %s. %s, %s", language_data[settings.language][8], months[settings.language][t->tm_mon], d_buffer, y_buffer);
	}
	else{
		snprintf(final_buffer, sizeof(final_buffer), "%s %s. %s. %s", language_data[settings.language][8], d_buffer, months[settings.language][t->tm_mon], y_buffer);
	}

	text_layer_set_text(date_layer, final_buffer);

	if(t->tm_hour > 9 && t->tm_hour < 7){
		isNight = true;
	}
	else{
		isNight = false;
	}
}

void tick_handler_fire(){
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	tick_handler(t, MINUTE_UNIT);
}

void refresh_settings(){
	layer_set_hidden(inverter_layer_get_layer(theme), settings.theme);
	layer_set_hidden(text_layer_get_layer(its_layer), settings.its_layer);
	layer_set_hidden(text_layer_get_layer(date_layer), settings.bottom_slot);
	layer_set_hidden(battery_bar_layer, settings.battery_bar);

	static char its_buffer[] = "It's..........";
	snprintf(its_buffer, sizeof(its_buffer), "%s", language_data[settings.language][5]);
	text_layer_set_text(its_layer, its_buffer);

	content_layer_set_text(card_note1, note_1.content[0], note_1.content[1]);
	content_layer_set_text(card_note2, note_2.content[0], note_2.content[1]);

	static char weather_buffer[] = "It's -1337 degrees and partially cloudy..............................";
	snprintf(weather_buffer, sizeof(weather_buffer), "%s %d %s %s %s.", language_data[settings.language][5], settings.temp, language_data[settings.language][9], language_data[settings.language][10], w_l_d[settings.language][settings.icon]);
	content_layer_set_text(card_weather, language_data[settings.language][11], weather_buffer);

	amount_of_cards = settings.bt_card + settings.battery_card + settings.note_1 + settings.note_2 + settings.weather_card + 1; //+1 is the time card
	layer_mark_dirty(circles_layer);

	booted = false;

	bool bt = bluetooth_connection_service_peek();
	bt_handler(bt);

	BatteryChargeState state = battery_state_service_peek();
	battery_handler(state);

	tick_handler_fire();

	if((previousFonts[0] != settings.fonts[0]) || (previousFonts[1] != settings.fonts[1])){
		window_stack_pop(false);
		window_stack_push(window, true);
	}
}

void battery_handler(BatteryChargeState state){
	bool markDirty = false;
	if(state.charge_percent != charge_percent){
		markDirty = true;
	}
	charge_percent = state.charge_percent;
	if(markDirty){ layer_mark_dirty(battery_bar_layer); }
	
	static char battery_buffer[] = "Holy crap, some German words are so long we have to make the buffer long.asfjnafnsdfjnsdkj";
	snprintf(battery_buffer, sizeof(battery_buffer), "%d%% %s %s", state.charge_percent, language_data[settings.language][6], charge_states[state.is_charging]);

	content_layer_set_text(card_battery, language_data[settings.language][7], battery_buffer);
}

void bt_handler(bool connected){
	if(booted){
		if(settings.btdisalert && !connected){
			notif_layer_push_notif(language_data[settings.language][0], language_data[settings.language][1], 3);
		}
		if(settings.btrealert && connected){
			notif_layer_push_notif(language_data[settings.language][2], language_data[settings.language][3], 2);
		}
	}
	else{
		booted = true;
	}
	
	if(connected){
		content_layer_set_text(card_bt, language_data[settings.language][4], language_data[settings.language][2]);
	}
	else{
		content_layer_set_text(card_bt, language_data[settings.language][4], language_data[settings.language][0]);
	}
}

void animate_buildup(bool invert){
	if(!invert){
		saved_animation = animate_layer_return(text_layer_get_layer(buildup_layer), &GRect(0, 164, 0, 9), &GRect(0, 164, 144, 9), 5000, 0);
	}
	else{
		GRect temp = layer_get_frame(text_layer_get_layer(buildup_layer));
		animate_layer(text_layer_get_layer(buildup_layer), &temp, &GRect(144, 164, 0, 9), 500, 0);
	}
}

void icon_layer_hide(){
	animate_layer(bitmap_layer_get_layer(icon_layer_bm), &GRect(0, -55, 144, 168), &GRect(0, 0, 144, 168), 700, 0);
}

void icon_switch(){
	bitmap_layer_set_bitmap(icon_layer_bm, temp_icon);
}

void icon_layer_switch(GBitmap *icon){
	if(icon == NULL){
		gbitmap_destroy(temp_temp_icon);
		if(!isNight){
			temp_temp_icon = gbitmap_create_with_resource(weather_ids[settings.icon]);
		}
		else{
			temp_temp_icon = gbitmap_create_with_resource(weather_ids[settings.icon+1]);
		}
	}
	animate_layer(bitmap_layer_get_layer(icon_layer_bm), &GRect(0, -55, 144, 168), &GRect(0, 0, 144, 168), 350, 0);
	animate_layer(bitmap_layer_get_layer(icon_layer_bm), &GRect(0, 00, 144, 168), &GRect(0, -55, 144, 168), 350, 720);
	icon_timer = app_timer_register(700, icon_switch, NULL);
	if(icon != NULL){
		temp_icon = icon;
	}
	else{
		temp_icon = temp_temp_icon;
	}
}

void icon_layer_show(){
	animate_layer(bitmap_layer_get_layer(icon_layer_bm), &GRect(0, 0, 144, 168), &GRect(0, -55, 144, 168), 700, 0);
}

void time_elements_show(){
	animate_layer(time_content_root, &GRect(0, 200, 144, 168), &GRect(0, 0, 144, 168), 700, 0);
	animate_layer(text_layer_get_layer(card_time_layer), &GRect(80, 120, 40, 18), &GRect(80, 100, 40, 18), 700, 0);
	animate_layer(card_time_root, &GRect(0, 15, 144, 168), &GRect(0, 0, 144, 168), 700, 0);
}

void time_elements_hide(){
	animate_layer(time_content_root, &GRect(0, 0, 144, 168), &GRect(0, -200, 144, 168), 700, 0);
	animate_layer(text_layer_get_layer(card_time_layer), &GRect(80, 100, 40, 18), &GRect(80, 120, 40, 18), 700, 0);
	animate_layer(card_time_root, &GRect(0, 0, 144, 168), &GRect(0, 15, 144, 168), 700, 0);
}

void hide_current_card(){
	switch(currentCard){
		case CARD_WEATHER:
			content_layer_hide(card_weather);
			break;
		case CARD_BLUETOOTH:
			content_layer_hide(card_bt);
			break;
		case CARD_BATTERY:
			content_layer_hide(card_battery);
			break;
		case CARD_NOTE_1:
			content_layer_hide(card_note1);
			break;
		case CARD_NOTE_2:
			content_layer_hide(card_note2);
			break;
	}
}

void cards_deinit(){
	app_timer_cancel(return_timer);
	animation_unschedule_all();
	hide_current_card();
	card_f = 1;
	layer_mark_dirty(circles_layer);
	currentCard = TIME_CARD;
	animate_buildup(true);
	icon_layer_switch(clock_icon);
	time_elements_show();
	reset_stack_top();
}

void cards_init(){
	return_timer = app_timer_register(5000, cards_deinit, NULL);
	animate_buildup(false);
	time_elements_hide();
}
/*
 * Pre-card reordering framework
 * Kept here for historical purposes :) 
 *
void tap_handler(AccelAxisType axis, int32_t direction){
	switch(currentCard){
		case TIME_CARD:
			if(settings.weather_card){
				content_layer_display(card_weather);
				icon_layer_switch(NULL);
				currentCard = CARD_WEATHER;
			}
			else if(settings.bt_card){
				content_layer_display(card_bt);
				icon_layer_switch(bt_icon);
				currentCard = CARD_BLUETOOTH;
		    }
		    else if(settings.battery_card){
		    	content_layer_display(card_battery);
		    	icon_layer_switch(battery_icon);
				currentCard = CARD_BATTERY;
		    }
		    else if(settings.note_1){
		    	content_layer_display(card_note1);
		    	icon_layer_switch(note_icon);
				currentCard = CARD_NOTE_1;
		    }
		    else if(settings.note_2){
		    	content_layer_display(card_note2);
		    	icon_layer_switch(note_icon);
				currentCard = CARD_NOTE_2;
		    }
		    else{
		    	cards_deinit();
		    }
			cards_init();
			break;
		case CARD_WEATHER:
			content_layer_hide(card_weather);
			if(settings.bt_card){
				content_layer_display(card_bt);
				currentCard = CARD_BLUETOOTH;
			}
			else{
				goto F_CARD_BLUETOOTH;
			}
			app_timer_reschedule(return_timer, 5000);
			animate_buildup(false);
			icon_layer_switch(bt_icon);
			break;	
		case CARD_BLUETOOTH:
			F_CARD_BLUETOOTH:;
			content_layer_hide(card_bt);
			if(settings.battery_card){
				content_layer_display(card_battery);
				currentCard = CARD_BATTERY;
			}
			else{
				goto F_CARD_BATTERY;
			}
			app_timer_reschedule(return_timer, 5000);
			animate_buildup(false);
			icon_layer_switch(battery_icon);
			break;
		case CARD_BATTERY:
			F_CARD_BATTERY:;
			content_layer_hide(card_battery);
			if(settings.note_2){
				content_layer_display(card_note1);
				currentCard = CARD_NOTE_1;
			}
			else{
				goto F_CARD_NOTE_1;
			}
			app_timer_reschedule(return_timer, 5000);
			animate_buildup(false);
			icon_layer_switch(note_icon);
			break;
		case CARD_NOTE_1:
			F_CARD_NOTE_1:;
			content_layer_hide(card_note1);
			if(settings.note_2){
				content_layer_display(card_note2);
				currentCard = CARD_NOTE_2;
			}
			else{
				goto F_CARD_NOTE_2;
			}
			app_timer_reschedule(return_timer, 5000);
			animate_buildup(false);
			break;
		case CARD_NOTE_2:
			F_CARD_NOTE_2:;
			content_layer_hide(card_note2);
			cards_deinit();
			break;
	}
	card_f++;
	if(card_f > amount_of_cards){
		card_f = 1;
	}
	layer_mark_dirty(circles_layer);
}
*/
void tap_handler(AccelAxisType axis, int32_t direction){
	switch(currentCard){
		case TIME_CARD:
			currentCard = OTHER_CARD;
			cards_init();
			goto DEFAULT_T;
		case -1:
			currentCard = TIME_CARD;
			cards_deinit();
			break;
		default:
			DEFAULT_T:;
			int id = get_next_content_layer_id();
			//APP_LOG(APP_LOG_LEVEL_INFO, "return id %d", id);

			hide_current_card();
			app_timer_reschedule(return_timer, 5000);
			animate_buildup(false);

			if(id == card_weather->ID){
				if(settings.weather_card){
					content_layer_display(card_weather);
					currentCard = card_weather->R_ID;
					icon_layer_switch(NULL);
			    }
			    else{
			    	goto DEFAULT_T;
			    }
			}
			else if(id == card_bt->ID){
				if(settings.bt_card){
					content_layer_display(card_bt);
					currentCard = card_bt->R_ID;
					icon_layer_switch(bt_icon);
				}
				else{
					goto DEFAULT_T;
				}
			}
			else if(id == card_battery->ID){
				if(settings.battery_card){
					content_layer_display(card_battery);
					currentCard = card_battery->R_ID;
					icon_layer_switch(battery_icon);
				}
				else{
					goto DEFAULT_T;
				}
			}
			else if(id == card_note1->ID){
				if(settings.note_1){
					content_layer_display(card_note1);
					currentCard = card_note1->R_ID;
					icon_layer_switch(note_icon);
				}
				else{
					goto DEFAULT_T;
				}
			}
			else if(id == card_note2->ID){
				if(settings.note_2){
					content_layer_display(card_note2);
					currentCard = card_note2->R_ID;
					if(currentCard != card_note1->R_ID){
						icon_layer_switch(note_icon);
					}
				}
				else{
					goto DEFAULT_T;
				}
			}
			else{
				cards_deinit();
				return;
			}
			break;
	}
	card_f++;
	if(card_f > amount_of_cards){
		card_f = 1;
	}
	layer_mark_dirty(circles_layer);
}

void process_tuple(Tuple *t){
	int key = t->key;
	int value = t->value->int32;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded key num: %d with value %d", key, value);
	switch (key) {
		case 0:
	  		settings.btdisalert = value;
			break;
		case 1:
			settings.btrealert = value;
			break;
		case 2:
			settings.theme = value;
			break;
		case 3:
			settings.language = value;
			break;
		case 4:
			settings.bt_card = value;
			break;
		case 5:
			settings.battery_card = value;
			break;
		case 6:
			settings.note_1 = value;
			break;
		case 7:
			strcpy(note_1.content[0], t->value->cstring);
			break;
		case 8:
			strcpy(note_1.content[1], t->value->cstring);
			break;
		case 9:
			settings.note_2 = value;
			break;
		case 10:
			strcpy(note_2.content[0], t->value->cstring);
			break;
		case 11:
			strcpy(note_2.content[1], t->value->cstring);
			break;
		case 12:
			settings.its_layer = value;
			break;
		case 13:
			settings.bottom_slot = value;
			break;
		case 14:
			settings.battery_bar = value;
			break;
		case 15:
			settings.weather_card = value;
			break;
		case 16:
			settings.temp_pref = value;
			break;
		case 17:
			settings.update_freq = value;
			break;
		case 18:
			if(settings.temp_pref){
				settings.temp = (1.8*(value-273)+32);
			}
			else{
				settings.temp = (value - 273);
			}
			break;
		case 19:
			settings.icon = value;
			if(settings.icon > 5){
				settings.icon++;
			}
			break;
		case 20:
			settings.placements[0] = value;
			set_id_stack_value(value, card_weather->ID);
			break;
		case 21:
			settings.placements[1] = value;
			set_id_stack_value(value, card_bt->ID);
			break;
		case 22:
			settings.placements[2] = value;
			set_id_stack_value(value, card_battery->ID);
			break;
		case 23:
			settings.placements[3] = value;
			set_id_stack_value(value, card_note1->ID);
			break;
		case 24:
			settings.placements[4] = value;
			set_id_stack_value(value, card_note2->ID);
			break;
		case 25:
			previousFonts[0] = settings.fonts[0];
			settings.fonts[0] = value;
			break;
		case 26:
			previousFonts[1] = settings.fonts[1];
			settings.fonts[1] = value;
			break;
	}
}

void app_message_handler(DictionaryIterator *iter, void *context) {
	refresh_timer = app_timer_register(600, refresh_settings, NULL);
	Tuple *t = dict_read_first(iter);
	if(t)
	{
		process_tuple(t);
	}
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)
		{
			process_tuple(t);
		}
	}
}
	
void window_load_main(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	
	background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
	bt_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH_ICON);
	battery_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_ICON);
	note_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NOTES_ICON);
	clock_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CLOCK_ICON);
	temp_temp_icon = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_RAIN);
	
	background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	bitmap_layer_set_bitmap(background_layer, background_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

	card_time_root = layer_create(GRect(0, -200, 144, 168));
	layer_set_update_proc(card_time_root, card_time_root_proc);
	layer_add_child(window_layer, card_time_root);

	card_time_layer = text_layer_init(GRect(80, -205, 40, 18), GTextAlignmentCenter, 1);
	text_layer_set_text(card_time_layer, "00:00");
	layer_add_child(window_layer, text_layer_get_layer(card_time_layer));

	icon_layer_bm = bitmap_layer_create(GRect(0, -55, 144, 168));
	bitmap_layer_set_bitmap(icon_layer_bm, clock_icon);
	layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer_bm));

	//This is only still here to cover the elements that hide behind this layer.
	chat_bubble_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(chat_bubble_layer, chat_bubble_proc);
	layer_add_child(window_layer, chat_bubble_layer);

	battery_bar_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(battery_bar_layer, battery_layer_proc);
	layer_add_child(window_layer, battery_bar_layer);
	
	time_content_root = layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_layer, time_content_root);
	
	time_layer = text_layer_init(GRect(0, 57, 144, 50), GTextAlignmentCenter, 0);
	text_layer_set_text(time_layer, "00 00");
	layer_add_child(time_content_root, text_layer_get_layer(time_layer));
	
	its_layer = text_layer_init(GRect(36, 45, 80, 38), GTextAlignmentLeft, 1);
	text_layer_set_text(its_layer, "It's");
	layer_add_child(time_content_root, text_layer_get_layer(its_layer));
	
	date_layer = text_layer_init(GRect(0, 99, 144, 20), GTextAlignmentCenter, 1);
	text_layer_set_text(date_layer, "on Oct. 9th, '14");
	if(settings.fonts[0] == 1){
		text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	}
	layer_add_child(time_content_root, text_layer_get_layer(date_layer));
	
	card_weather = content_layer_create(window, 1, settings.fonts[1]);
	content_layer_set_text(card_weather, "Weather", "0 degrees, sunny");
	
	card_bt = content_layer_create(window, 2, settings.fonts[1]);
	content_layer_set_text(card_bt, "Bluetooth", "Connected");
	
	card_battery = content_layer_create(window, 3, settings.fonts[1]);
	content_layer_set_text(card_battery, "Battery", "Watch at 70%");

	card_note1 = content_layer_create(window, 4, settings.fonts[1]);
	content_layer_set_text(card_note1, "Groceries", "Remember to get eggs, milk, and your mom. Hello person reading this.");

	card_note2 = content_layer_create(window, 5, settings.fonts[1]);
	content_layer_set_text(card_note2, "Something", "Notes r fun 4 all!!!");

	set_id_stack_value(settings.placements[0], card_weather->ID);
	set_id_stack_value(settings.placements[1], card_bt->ID);
	set_id_stack_value(settings.placements[2], card_battery->ID);
	set_id_stack_value(settings.placements[3], card_note1->ID);
	set_id_stack_value(settings.placements[4], card_note2->ID);

	buildup_layer = text_layer_create(GRect(0, 164, 0, 9));
	text_layer_set_background_color(buildup_layer, GColorBlack);
	layer_add_child(window_layer, text_layer_get_layer(buildup_layer));

	circles_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(circles_layer, circles_proc);
	layer_add_child(window_layer, circles_layer);
	
	notif_layer_init(window);
	
	theme = inverter_layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_layer, inverter_layer_get_layer(theme));
	
	BatteryChargeState state = battery_state_service_peek();
	battery_handler(state);
	
	if(booted == false){
		previousFonts[0] = settings.fonts[0];
		previousFonts[1] = settings.fonts[1];
	}

	bool connected = bluetooth_connection_service_peek();
	bt_handler(connected);
	
	tick_handler_fire();
	refresh_settings();
}

void window_unload_main(Window *window){
	gbitmap_destroy(background_bitmap);
	gbitmap_destroy(bt_icon);
	gbitmap_destroy(battery_icon);
	gbitmap_destroy(note_icon);
	gbitmap_destroy(clock_icon);
	gbitmap_destroy(temp_temp_icon);
	bitmap_layer_destroy(background_layer);
	layer_destroy(card_time_root);
	text_layer_destroy(card_time_layer);
	bitmap_layer_destroy(icon_layer_bm);
	layer_destroy(chat_bubble_layer);
	layer_destroy(battery_bar_layer);
	layer_destroy(time_content_root);
	text_layer_destroy(time_layer);
	text_layer_destroy(its_layer);
	text_layer_destroy(date_layer);
	content_layer_destroy(card_weather);
	content_layer_destroy(card_bt);
	content_layer_destroy(card_battery);
	content_layer_destroy(card_note1);
	content_layer_destroy(card_note2);
	text_layer_destroy(buildup_layer);
	layer_destroy(circles_layer);
	notif_layer_deinit();
	inverter_layer_destroy(theme);
}
	
void init(){
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load_main,
		.unload = window_unload_main,
	});
	
	int value = 0, value1 = 0, value2 = 0;
	if(persist_exists(0)){
		value = persist_read_data(0, &settings, sizeof(settings));
	}	
	else{
		settings.btdisalert = true;
		settings.btrealert = false;
		settings.theme = 0;
		settings.bt_card = true;
		settings.battery_card = true;
		settings.language = 0;
		settings.note_1 = true;
		settings.note_2 = false;
		settings.its_layer = false;
		settings.bottom_slot = false;
		settings.battery_bar = false;
		for(int i = 0; i < 5; i++){
			settings.placements[i] = i;
		}
	}
	
	if(persist_exists(1)){
		value1 = persist_read_data(1, &note_1, sizeof(note_1));
	}
	else{
		strcpy(note_1.content[0], "Notes!");
		strcpy(note_1.content[1], "You can use notes to store quick data.");
	}

	if(persist_exists(2)){
		value1 = persist_read_data(2, &note_2, sizeof(note_2));
	}
	//don't need an else statement because if the user wants a second note, they enable it manually.

	int total = value+value1+value2;
	APP_LOG(APP_LOG_LEVEL_INFO, "HI: %d bytes read total.", total);

	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	battery_state_service_subscribe(battery_handler);
	bluetooth_connection_service_subscribe(bt_handler);
	accel_tap_service_subscribe(tap_handler);

	app_message_register_inbox_received(app_message_handler);
	app_message_open(512, 512);

	srand(time(NULL));
	
	bool this_doesnt_do_anything_so_why_does_it_matter_to_have_it_here_seriously = true;
	window_stack_push(window, this_doesnt_do_anything_so_why_does_it_matter_to_have_it_here_seriously);
}

void deinit(){
	window_destroy(window);
	int value = persist_write_data(0, &settings, sizeof(settings));
	int value1 = persist_write_data(1, &note_1, sizeof(note_1));
	int value2 = persist_write_data(2, &note_2, sizeof(note_2));

	int total = value+value1+value2;
	APP_LOG(APP_LOG_LEVEL_INFO, "HI: %d bytes written total", total);
}
	
int main(){
	init();
	app_event_loop();
	deinit();
	return 1337;
}