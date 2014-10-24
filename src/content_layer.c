#include <pebble.h>
#include "content_layer.h"
#include "extras.h"

int ID_stack[5] = {
	0, 1, 2, 3, 4
};

int ID_stack_top = -1;

void reset_stack_top(){
	ID_stack_top = -1;
}

int get_next_content_layer_id(){
	ID_stack_top++;
	if(ID_stack_top > 4){
		reset_stack_top();
		return -1;	
	}
	return ID_stack[ID_stack_top];
}

void set_id_stack_value(int placement, int value){
	ID_stack[placement] = value;
}

void set_stack_placement_by_id(int id){
	int value = 0;
	while(value != id){
		value = ID_stack[ID_stack_top];
		ID_stack_top++;
	}
	//APP_LOG(APP_LOG_LEVEL_INFO, "value %d, id %d", value, id);
}

ContentLayer content_layer_create(Window *window, int R_ID, bool customFont){
	ContentLayer returnlayer;

	returnlayer.root = layer_create(GRect(0, 168, 144, 168));
	returnlayer.title = text_layer_init(GRect(22, 48, 102, 20), GTextAlignmentCenter, 0);
	text_layer_set_font(returnlayer.title, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_18)));
	returnlayer.content = text_layer_init(GRect(22, 66, 102, 80), GTextAlignmentCenter, 0);
	if(!customFont){
		text_layer_set_font(returnlayer.content, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	}
	else{
		text_layer_set_font(returnlayer.content, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	}
	
	layer_add_child(window_get_root_layer(window), returnlayer.root);
	layer_add_child(returnlayer.root, text_layer_get_layer(returnlayer.title));
	layer_add_child(returnlayer.root, text_layer_get_layer(returnlayer.content));

	returnlayer.ID = rand();
	ID_stack_top++;
	ID_stack[ID_stack_top] = returnlayer.ID;

	returnlayer.R_ID = R_ID;

	if(ID_stack_top > 3){
		reset_stack_top();
	}
	
	return returnlayer;
}

void content_layer_set_text(ContentLayer layer, char *title, char *content){
	text_layer_set_text(layer.title, title);
	text_layer_set_text(layer.content, content);
}

void content_layer_display(ContentLayer layer){
	animate_layer(layer.root, &GRect(0, 168, 144, 168), &GRect(0, 0, 144, 168), 600, 0);
}

void content_layer_hide(ContentLayer layer){
	animate_layer(layer.root, &GRect(0, 0, 144, 168), &GRect(0, -168, 144, 168), 600, 0);
}

void content_layer_destroy(ContentLayer content){
	layer_destroy(content.root);
	text_layer_destroy(content.title);
	text_layer_destroy(content.content);
}