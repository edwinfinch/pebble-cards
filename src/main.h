#include <pebble.h>
#pragma once
#define TIME_CARD 0
#define OTHER_CARD 1
#define CARD_WEATHER 1
#define CARD_BLUETOOTH 2
#define CARD_BATTERY 3
#define CARD_NOTE_1 4
#define CARD_NOTE_2 5

Window *window;
	
BitmapLayer *background_layer;
GBitmap *background_bitmap, *bt_icon, *battery_icon, *temp_icon, *note_icon, *clock_icon, *temp_temp_icon;
Layer *chat_bubble_layer, *battery_bar_layer, *time_content_root, *card_time_root, *icon_layer_fix, *circles_layer;
TextLayer *time_layer, *its_layer, *date_layer, *buildup_layer, *card_time_layer;

BitmapLayer *icon_layer_bm;

InverterLayer *theme;

AppTimer *return_timer, *icon_timer, *refresh_timer;

Animation *saved_animation;

int currentCard = 0, charge_percent = 0, amount_of_cards = 1, card_f = 1;
bool booted = false, isNight = false;
bool previousFonts[2] = {
	false, false
};

typedef struct Settings {
	bool btdisalert;
	bool btrealert;
	bool theme;
	bool bt_card;
	bool battery_card;
	bool weather_card;
	bool temp_pref;
	bool note_1;
	bool note_2;
	bool its_layer;
	bool bottom_slot;
	bool battery_bar;
	bool fonts[2]; //for those users (cough, philipp, cough) that want differnet fonts.
	int temp;
	int icon;
	int placements[5];
	uint8_t update_freq;
	uint8_t language;
}Settings;

typedef struct Note {
	char content[2][70];
}Note;

Settings settings;

Note note_1, note_2;

char *charge_states[2] = {
	"", "(charging)"
};

/*
Language array:
0. English
1. German
2. Spanish
*/

char *language_data[3][12] = {
	{
		"Disconnected", "Bluetooth disconnected from phone",
		"Connected", "Bluetooth connection reestablished with phone",
		"Bluetooth", "It's", "of battery charge left", "Battery", "on",
		"degrees", "and", "Weather"
	},
	{
		"Getrennt", "Bluetooth-Verbindung getrennt",
		"Verbunden", "Bluetooth-Verbindung hergestellt",
		"Bluetooth", "Es ist", "Restbatterie", "Batterie", "am",
		"Grad", "und", "Wetter"
	},
	{
		"Desconectado", "Bluetooth desconectado del telefono",
		"Conectado", "Conexion Bluetooth reestablecida con el telefono",
    	"Bluetooth", "Son las", "carga de bateria restante", "Bateria", "activado"
    	"grados", "y", "Tiempo"
	}
};

char *w_l_d[3][10] = {
	{
		"thunderstorms", "some drizzle", "raining", "snowing", "foggy", "clear", "clear", "partially cloudy", "partially cloudy", "cloudy"
	},
	{
		"Gewitter", "etwas Nieselregen", "regnet", "schneit", "neblig", "klar", "klar", "teilweise bewoelkt", "teilweise bewoelkt", "wolkig"
	},
	{
		"tormentas eléctricas", "llovizna", "lluvia", "nieve", "niebla", "despejado", "despejado", "parcialmente nublado", "parcialmente nublado", "nublado"
	}
};

char *months[3][12] = {
	{
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	},
	{
		"Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"
	},
	{
		"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"
	}
};

uint32_t weather_ids[10] = {
	RESOURCE_ID_WEATHER_THUNDER,
	RESOURCE_ID_WEATHER_DRIZZLE,
	RESOURCE_ID_WEATHER_RAIN,
	RESOURCE_ID_WEATHER_SNOW,
	RESOURCE_ID_WEATHER_FOG,
	RESOURCE_ID_WEATHER_CLEAR_DAY,
	RESOURCE_ID_WEATHER_CLEAR_NIGHT,
	RESOURCE_ID_WEATHER_PARTIALLY_CLOUDY_DAY,
	RESOURCE_ID_WEATHER_PARTIALLY_CLOUDY_NIGHT,
	RESOURCE_ID_WEATHER_CLOUDY,
};