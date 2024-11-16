#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "scene_scoreboard.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"

// #define GHOST_NUM 4  // * defined in 'scene_game.h'

/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
int game_main_Score = 0;
bool game_over = false;

bool wallWalk = false;
bool ghostStop = false;

extern ALLEGRO_TIMER* powered_timer;
extern ALLEGRO_TIMER* powered_timer_near;
extern const int powered_duration;
extern const int powered_duration_near;

extern Ghost** ghosts;

/* Internal variables*/
static Pacman* pman;
static Map* basic_map;
bool debug_mode = false;
bool cheat_mode = false;
ALLEGRO_TIMER* power_up_timer;
uint32_t game_over_time_count;
uint32_t game_over_time_length = 6 * 64;

/* Declare static function prototypes */
static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void printInfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(int btn, int x, int y, int dz);
static void render_init_screen(void);
static void draw_hitboxes(void);

static void init(void) {
	game_over = false;
	game_over_time_count = 0;
	game_main_Score = 0;
	
	basic_map = create_map("Assets/map_nthu.txt");
	if (!basic_map) {
		game_abort("error on creating map");
	}

	pman = pacman_create();
	pman->objData.facing = RIGHT;
	pman->mouth_open = true;
	if (!pman) {
		game_abort("error on creating pacman\n");
	}
	
	ghosts = malloc(sizeof(Ghost) * GHOST_NUM);
	if(!ghosts){
		game_log("We haven't create any ghosts!\n");
	} else {
		for (int i = 0; i < GHOST_NUM; i++) {
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);
			if (!ghosts[i])
				game_abort("error creating ghost\n");
		}
	}
	GAME_TICK = 0;

	render_init_screen();
	power_up_timer = al_create_timer(1.0f); // 1 tick per second
	if (!power_up_timer)
		game_abort("Error on create timer\n");
	return ;
}

static void step(void) {
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;
	switch (basic_map->map[Grid_y][Grid_x]) {
		case '.':
			pacman_eatItem(pman, '.');
			basic_map->map[Grid_y][Grid_x] = ' ';
			basic_map->beansCount -= 1;
			if (basic_map->beansCount == 0)
				game_over = true;
			break;
		case 'P':
			pacman_eatItem(pman, 'P');
			basic_map->map[Grid_y][Grid_x] = ' ';
			al_stop_timer(powered_timer);
			al_stop_timer(powered_timer_near);
			al_start_timer(powered_timer);
			al_start_timer(powered_timer_near);
			for (int i=0; i<GHOST_NUM; i++) {
				ghost_toggle_FLEE(ghosts[i], true);
			}
			break;
		default:
			break;
	}
}
static void status_update(void) {
	for (int i = 0; i < GHOST_NUM; i++) {
		if (ghosts[i]->status == GO_IN){
			continue;
		}
		else if (ghosts[i]->status == FREEDOM) {
			RecArea a = getDrawArea((object*)ghosts[i], GAME_TICK_CD);
			RecArea b = getDrawArea((object*)pman, GAME_TICK_CD);
			if (!cheat_mode && RecAreaOverlap(&a, &b)) {
				game_log("collide with ghost\n");
				pacman_die();
				game_over = true;
				break; // animation shouldn't be trigger twice.
			}
		}
		else if (ghosts[i]->status == FLEE) {
			RecArea a = getDrawArea((object*)ghosts[i], GAME_TICK_CD);
			RecArea b = getDrawArea((object*)pman, GAME_TICK_CD);
			if (!cheat_mode && RecAreaOverlap(&a, &b)) {
				ghost_collided(ghosts[i]);
				game_log("HERE");
			}
		}
	}
}

static void update(void) {
	if (game_over) {
		// TODO-GC-game_over: start pman->death_anim_counter and schedule a game-over event (e.g change scene to menu) after Pacman's death animation finished
		// hint: refer al_get_timer_started(...), al_get_timer_count(...), al_stop_timer(...), al_rest(...)
		// start the timer if it hasn't been started.
		// check timer counter.
		// stop the timer if counter reach desired time.
		if (game_over_time_count == 0) game_over_time_count = al_get_timer_count(game_tick_timer);
		if (al_get_timer_count(game_tick_timer) - game_over_time_count > game_over_time_length) {
			justFinished = true;
			justScore = basic_map->beansNum-basic_map->beansCount;
			game_change_scene(scene_scoreboard_create());
		}
		return;
	}

	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
	for (int i = 0; i < GHOST_NUM; i++) 
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
}

static void draw(void) {
	al_clear_to_color(al_map_rgb(16, 26, 36));

	char str[100];
	snprintf(str, 100, "%s: %d", "Points", basic_map->beansNum - basic_map->beansCount);
	al_draw_text(
		menuFont,
		al_map_rgb(166, 205, 236),
		80, 720,
		ALLEGRO_ALIGN_LEFT,
		str
	);

	draw_map(basic_map);

	pacman_draw(pman);
	if (game_over)
		return;
	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++)
		ghost_draw(ghosts[i]);
	
	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}

}

static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea((object *)pman, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea((object *)ghosts[i], GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printInfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}

static void destroy(void) {
	// for (int i=0; i<basic_map->row_num; i++) {
	// 	free(basic_map->map[i]);
	// }
	// free(basic_map->map);
	// pacman_destroy(pman);
	// for (int i=0; i<GHOST_NUM; i++) {
	// 	ghost_destroy(ghosts[i]);
	// }
}

static void on_key_down(int key_code) {
	if (key_code == keyboard_w) {
		pacman_NextMove(pman, UP);
	} else if (key_code == keyboard_a) {
		pacman_NextMove(pman, LEFT);
	} else if (key_code == keyboard_s) {
		pacman_NextMove(pman, DOWN);
	} else if (key_code == keyboard_d) {
		pacman_NextMove(pman, RIGHT);
	} else if (key_code == ALLEGRO_KEY_C) {
		cheat_mode = !cheat_mode;
		if (cheat_mode)
			printf("cheat mode on\n");
		else 
			printf("cheat mode off\n");
	} else if (key_code == ALLEGRO_KEY_G) {
		debug_mode = !debug_mode;
	} 
	if (key_code == ALLEGRO_KEY_L && key_state[217]) {
		wallWalk = !wallWalk;
	}
	if (key_code == ALLEGRO_KEY_S && key_state[217]) {
		ghostStop = !ghostStop;
	}
	if (key_code == ALLEGRO_KEY_K) {
		for (int i=0; i<GHOST_NUM; i++) {
			ghosts[i]->status = GO_IN;
			ghosts[i]->speed = 4;
		}
	}
}

static void on_mouse_down(int btn, int x, int y, int dz) {

}

static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(16, 26, 36));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		400, 400,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);

	al_flip_display();
	al_rest(2.0);
}

// The only function that is shared across files.
Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}