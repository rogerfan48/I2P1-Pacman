#include "scene_settings.h"
#include "utility.h"
#include "shared.h"
#include "scene_game.h"
#include <allegro5/allegro_primitives.h>

// Variables and functions with 'static' prefix at the top level of a
// source file is only accessible in that file ("file scope", also
// known as "internal linkage"). If other files has the same variable
// name, they'll be different variables.

/* Define your static vars / function prototypes below. */

// TODO-IF: More variables and functions that will only be accessed
// inside this scene. They should all have the 'static' prefix.
int blank_state[4] = {0};

extern float music_volume;  // Initial value between 0 and 1
const float sliderWidth1 = 400;
const float sliderHeight1 = 30;
const float sliderX1 = 300;
const float sliderY1 = 360;
RecArea sliderCoord1 = {.x=sliderX1, .y=sliderY1, .w=sliderWidth1, .h=sliderHeight1};
extern float effect_volume;  // Initial value between 0 and 1
const float sliderWidth2 = 400;
const float sliderHeight2 = 30;
const float sliderX2 = 300;
const float sliderY2 = 420;
RecArea sliderCoord2 = {.x=sliderX2, .y=sliderY2, .w=sliderWidth2, .h=sliderHeight2};

extern bool debug_mode;
extern bool cheat_mode;
const float checkWidth = 40;
const float checkX1 = 370;
const float checkY1 = 500-3;
const float checkX2 = 370;
const float checkY2 = 560-3;
RecArea checkCoord1 = {.x=checkX1, .y=checkY1, .w=checkWidth, .h=checkWidth};
RecArea checkCoord2 = {.x=checkX2, .y=checkY2, .w=checkWidth, .h=checkWidth};

	// 	al_flip_display();
	// }
	// else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
	// 	if (ev.mouse.x >= sliderX && ev.mouse.x <= sliderX + sliderWidth &&
	// 		ev.mouse.y >= sliderY && ev.mouse.y <= sliderY + sliderHeight) {
	// 		// Update slider value based on mouse click position
	// 		sliderValue = (ev.mouse.x - sliderX) / sliderWidth;
	// 	}
	// }

static void blank_state_clean() {
	for (int i=0; i<4; i++) {
		blank_state[i] = 0;
	}
}

static void draw(void) {
	al_clear_to_color(al_map_rgb(10, 30, 40));
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"<ENTER> Back to menu"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		100,
		100,
		ALLEGRO_ALIGN_LEFT,
		"Move Up:"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		100,
		160,
		ALLEGRO_ALIGN_LEFT,
		"Move Left:"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		100,
		220,
		ALLEGRO_ALIGN_LEFT,
		"Move Down:"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		100,
		280,
		ALLEGRO_ALIGN_LEFT,
		"Move Right:"
	);
	al_draw_filled_rectangle(380, 90, 700, 140, blank_state[0] ? al_map_rgb(20, 170, 170) : al_map_rgb(20, 60, 60));
	al_draw_filled_rectangle(380, 150, 700, 200, blank_state[1] ? al_map_rgb(20, 170, 170) : al_map_rgb(20, 60, 60));
	al_draw_filled_rectangle(380, 210, 700, 260, blank_state[2] ? al_map_rgb(20, 170, 170) : al_map_rgb(20, 60, 60));
	al_draw_filled_rectangle(380, 270, 700, 320, blank_state[3] ? al_map_rgb(20, 170, 170) : al_map_rgb(20, 60, 60));
	al_draw_text(
		menuFont,
		blank_state[0] ? al_map_rgb(20, 170, 170) : al_map_rgb(255, 255, 255),
		540,
		100,
		ALLEGRO_ALIGN_CENTER,
		al_keycode_to_name(keyboard_w)
	);
	al_draw_text(
		menuFont,
		blank_state[1] ? al_map_rgb(20, 170, 170) : al_map_rgb(255, 255, 255),
		540,
		160,
		ALLEGRO_ALIGN_CENTER,
		al_keycode_to_name(keyboard_a)
	);
	al_draw_text(
		menuFont,
		blank_state[2] ? al_map_rgb(20, 170, 170) : al_map_rgb(255, 255, 255),
		540,
		220,
		ALLEGRO_ALIGN_CENTER,
		al_keycode_to_name(keyboard_s)
	);
	al_draw_text(
		menuFont,
		blank_state[3] ? al_map_rgb(20, 170, 170) : al_map_rgb(255, 255, 255),
		540,
		280,
		ALLEGRO_ALIGN_CENTER,
		al_keycode_to_name(keyboard_d)
	);

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		100,
		360,
		ALLEGRO_ALIGN_LEFT,
		"Sound:"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		100,
		420,
		ALLEGRO_ALIGN_LEFT,
		"Effect:"
	);
	al_draw_filled_rectangle(sliderX1, sliderY1, sliderX1 + sliderWidth1, sliderY1 + sliderHeight1, al_map_rgb(20, 60, 60));
	al_draw_filled_rectangle(sliderX1, sliderY1, sliderX1 + music_volume * sliderWidth1, sliderY1 + sliderHeight1, al_map_rgb(20, 170, 170));
	al_draw_filled_rectangle(sliderX2, sliderY2, sliderX2 + sliderWidth2, sliderY2 + sliderHeight2, al_map_rgb(20, 60, 60));
	al_draw_filled_rectangle(sliderX2, sliderY2, sliderX2 + effect_volume * sliderWidth2, sliderY2 + sliderHeight2, al_map_rgb(20, 170, 170));

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		100,
		500,
		ALLEGRO_ALIGN_LEFT,
		"Debug Mode:"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		100,
		560,
		ALLEGRO_ALIGN_LEFT,
		"Cheat Mode:"
	);
	al_draw_rectangle(checkX1, checkY1, checkX1+checkWidth, checkY1+checkWidth, al_map_rgb(20, 170, 170), 5);
	if (debug_mode) {
		al_draw_filled_rectangle(checkX1, checkY1, checkX1+checkWidth, checkY1+checkWidth, al_map_rgb(20, 170, 170));
	}
	al_draw_rectangle(checkX2, checkY2, checkX2+checkWidth, checkY2+checkWidth, al_map_rgb(20, 170, 170), 5);
	if (cheat_mode) {
		al_draw_filled_rectangle(checkX2, checkY2, checkX2+checkWidth, checkY2+checkWidth, al_map_rgb(20, 170, 170));
	}
}

static void on_key_down(int keycode) {
	if (keycode == ALLEGRO_KEY_ENTER) {
		game_change_scene(scene_menu_create());
	} else if (keycode == ALLEGRO_KEY_G) {
		debug_mode = !debug_mode;
	} else if (keycode == ALLEGRO_KEY_C) {
		cheat_mode = !cheat_mode;
	} else if (keycode == ALLEGRO_KEY_K) {
	} else if (keycode == keyboard_w) {
		blank_state_clean();
		blank_state[0] = 1;
	} else if (keycode == keyboard_a) {
		blank_state_clean();
		blank_state[1] = 1;
	} else if (keycode == keyboard_s) {
		blank_state_clean();
		blank_state[2] = 1;
	} else if (keycode == keyboard_d) {
		blank_state_clean();
		blank_state[3] = 1;
	} else if (blank_state[0]) {
		keyboard_w = keycode;
		blank_state_clean();
	} else if (blank_state[1]) {
		keyboard_a = keycode;
		blank_state_clean();
	} else if (blank_state[2]) {
		keyboard_s = keycode;
		blank_state_clean();
	} else if (blank_state[3]) {
		keyboard_d = keycode;
		blank_state_clean();
	}
}

static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	if (pnt_in_rect(mouse_x, mouse_y, sliderCoord1)) {
		music_volume = (mouse_x - sliderX1) / sliderWidth1;
	} else if (pnt_in_rect(mouse_x, mouse_y, sliderCoord2)) {
		effect_volume = (mouse_x - sliderX2) / sliderWidth2;
	}
}

static void on_mouse_down(int btn, int x, int y, int dz) {
	if (pnt_in_rect(x, y, checkCoord1)) {
		debug_mode = !debug_mode;
	} else if (pnt_in_rect(x, y, checkCoord2)) {
		cheat_mode = !cheat_mode;
	}
}

// The only function that is shared across files.
Scene scene_settings_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.draw = &draw;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings scene created");
	return scene;
}

// TODO-Graphical_Widget:
// Just suggestions, you can create your own usage.
	// Selecting BGM:
	// 1. Declare global variables for storing the BGM. (see `shared.h`, `shared.c`)
	// 2. Load the BGM in `shared.c`.
	// 3. Create dropdown menu for selecting BGM in setting scene, involving `scene_settings.c` and `scene_setting.h.
	// 4. Switch and play the BGM if the corresponding option is selected.

	// Adjusting Volume:
	// 1. Declare global variables for storing the volume. (see `shared.h`, `shared.c`)
	// 2. Create a slider for adjusting volume in setting scene, involving `scene_settings.c` and `scene_setting.h.
		// 2.1. You may use checkbox to switch between mute and unmute.
	// 3. Adjust the volume and play when the button is pressed.

	// Selecting Map:
	// 1. Preload the map to `shared.c`.
	// 2. Create buttons(or dropdown menu) for selecting map in setting scene, involving `scene_settings.c` and `scene_setting.h.
		// 2.1. For player experience, you may also create another scene between menu scene and game scene for selecting map.
	// 3. Create buttons(or dropdown menu) for selecting map in setting scene, involving `scene_settings.c` and `scene_setting.h.
	// 4. Switch and draw the map if the corresponding option is selected.
		// 4.1. Suggestions: You may use `al_draw_bitmap` to draw the map for previewing. 
							// But the map is too large to be drawn in original size. 
							// You might want to modify the function to allow scaling.