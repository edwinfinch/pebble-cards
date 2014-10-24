#include <pebble.h>
#pragma once
	
typedef struct ContentLayer {
	Layer *root;
	TextLayer *title;
	TextLayer *content;
	int ID;
	int R_ID;
}ContentLayer;

void reset_stack_top();
int get_next_content_layer_id();
void set_id_stack_value(int placement, int value);
void set_stack_placement_by_id(int id);
ContentLayer content_layer_create(Window *window, int R_ID, bool customFont);
void content_layer_set_text(ContentLayer layer, char *title, char *content);
void content_layer_display(ContentLayer layer);
void content_layer_hide(ContentLayer layer);
void content_layer_destroy(ContentLayer content);