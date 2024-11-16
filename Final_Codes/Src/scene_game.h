// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

#ifndef SCENE_GAME_H
#define SCENE_GAME_H
#include "game.h"

#define GHOST_NUM 4
Ghost** ghosts;

bool debug_mode;
bool cheat_mode;
bool wallWalk;
bool ghostStop;

Scene scene_main_create(void);
/*
int64_t get_power_up_timer_tick();
int64_t get_power_up_duration();
*/

#endif
