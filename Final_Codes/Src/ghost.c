#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include "ghost.h"
#include "map.h"
#include "pacman_obj.h"
#include "scene_game.h"

/* global variables*/
// [ NOTE ]
// if you change the map .txt to your own design.
// You have to modify room_grid_{x,y} to corressponding value also.
// Or you can do some change while loading map (reading .txt file)
// Make the start position metadata stored with map.txt.
const int room_grid_x=22, room_grid_y=11;

/* shared variables. */
extern uint32_t GAME_TICK;
extern uint32_t GAME_TICK_CD;
extern const int block_width,  block_height;
/* Internal variables */
static const int fix_draw_pixel_offset_x = -3;
static const int fix_draw_pixel_offset_y = -3;
static const int draw_region = 30;
// [ NOTE - speed again ]
// Again, you see this notaficationd. If you still want to implement something 
// fancy with speed, objData->moveCD and GAME_TICK, you can first start on 
// working on animation of ghosts and pacman. // Once you finished the animation 
// part, you will have more understanding on whole mechanism.
static const int basic_speed = 2;

Ghost* ghost_create(int flag) {
	Ghost* ghost = (Ghost*)malloc(sizeof(Ghost));
	if (!ghost)
		return NULL;

	ghost->go_in_time = GAME_TICK; 
	ghost->typeFlag = flag;
	ghost->objData.Size.x = block_width;
	ghost->objData.Size.y = block_height;

	ghost->objData.nextTryMove = NONE;
	ghost->speed = basic_speed;
	ghost->status = BLOCKED;
	ghost->objData.facing = RIGHT;	// *
	ghost->img_state = false;		// *
	ghost->flee_near_end = false;	// *
	ghost->is_direct = false;		// *
	ghost->last_time_change = 0; 	// *


	ghost->flee_sprite = load_bitmap("Assets/ghost_flee.png");
	ghost->dead_sprite = load_bitmap("Assets/ghost_dead.png");

	// TODO-IF: You may design your own special tracking rules.
	switch (ghost->typeFlag) {
		case Blinky:
			ghost->objData.Coord.x = room_grid_x;
			ghost->objData.Coord.y = room_grid_y;
			ghost->move_sprite = load_bitmap("Assets/ghost_move_red.png");
			ghost->move_script = &ghost_move_script_random;
			break;
		case Pinky:
			// *load move script of shortest_path
			ghost->objData.Coord.x = room_grid_x;
			ghost->objData.Coord.y = room_grid_y+1;
			ghost->move_sprite = load_bitmap("Assets/ghost_move_pink.png");
			ghost->move_script = &ghost_move_script_random;

			break;
		case Inky:
			ghost->objData.Coord.x = room_grid_x-1;
			ghost->objData.Coord.y = room_grid_y+1;
			ghost->move_sprite = load_bitmap("Assets/ghost_move_blue.png");
			ghost->move_script = &ghost_move_script_shortest_path;

			break;
		case Clyde:
			ghost->objData.Coord.x = room_grid_x+1;
			ghost->objData.Coord.y = room_grid_y+1;
			ghost->move_sprite = load_bitmap("Assets/ghost_move_orange.png");
			ghost->move_script = &ghost_move_script_shortest_path;
			break;
		default:
			ghost->objData.Coord.x = room_grid_x;
			ghost->objData.Coord.y = room_grid_y;
			ghost->move_sprite = load_bitmap("Assets/ghost_move_red.png");
			ghost->move_script = &ghost_move_script_random;
			break;
	}
	return ghost;
}
void ghost_destroy(Ghost* ghost) {
	al_destroy_bitmap(ghost->dead_sprite);
	al_destroy_bitmap(ghost->flee_sprite);
	al_destroy_bitmap(ghost->move_sprite);
	free(ghost);
}

void ghost_draw(Ghost* ghost) {
	RecArea drawArea = getDrawArea((object*)ghost, GAME_TICK_CD);

	int bitmap_x_offset = 0;
	if (ghost->status == FLEE) {
		if (ghost->flee_near_end == false) {
			al_draw_scaled_bitmap(ghost->flee_sprite, (ghost->img_state) ? 0 : 16, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				);
		} else {
			al_draw_scaled_bitmap(ghost->flee_sprite, (ghost->objData.moveCD > GAME_TICK_CD/2) ? 0 : 48, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				);
		}
	} else if (ghost->status == GO_IN) {
		switch (ghost->objData.facing) {
			case RIGHT:
				al_draw_scaled_bitmap(ghost->dead_sprite, 0, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			case LEFT:
				al_draw_scaled_bitmap(ghost->dead_sprite, 16, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			case UP:
				al_draw_scaled_bitmap(ghost->dead_sprite, 32, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			case DOWN:
				al_draw_scaled_bitmap(ghost->dead_sprite, 48, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			default: break;
		}
	} else { 											//ghost->move_sprite
		switch (ghost->objData.facing) {
			case RIGHT:
				al_draw_scaled_bitmap(ghost->move_sprite, (ghost->img_state) ? 0 : 16, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			case LEFT:
				al_draw_scaled_bitmap(ghost->move_sprite, (ghost->img_state) ? 32 : 48, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			case UP:
				al_draw_scaled_bitmap(ghost->move_sprite, (ghost->img_state) ? 64 : 80, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			case DOWN:
				al_draw_scaled_bitmap(ghost->move_sprite, (ghost->img_state) ? 96 : 112, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			default: break;
		}
	}
	if (ghost->objData.moveCD == GAME_TICK_CD/2) {
		ghost->img_state = !ghost->img_state;
	}
}
void ghost_NextMove(Ghost* ghost, Directions next) {
	ghost->objData.nextTryMove = next;
}
void printGhostStatus(GhostStatus S) {
	switch(S){
	
	case BLOCKED: // stay inside the ghost room
		game_log("BLOCKED");
		break;
	case GO_OUT: // going out the ghost room
		game_log("GO_OUT");
		break;
	case FREEDOM: // free at the map
		game_log("FREEDOM");
		break;
	case GO_IN:
		game_log("GO_IN");
		break;
	case FLEE:
		game_log("FLEE");
		break;
	default:
		game_log("status error");
		break;
	}
}
bool ghost_movable(const Ghost* ghost, const Map* M, Directions targetDirec, bool disallow_room) {
	if (ghostStop) return false;
	int x = ghost->objData.Coord.x;
	int y = ghost->objData.Coord.y;
	
	switch (targetDirec) {
		case UP:
			y -= 1;
			break;
		case DOWN:
			y += 1;
			break;
		case LEFT:
			x -= 1;
			break;
		case RIGHT:
			x += 1;
			break;
		default:
			return false;
	}
	if (is_wall_block(M, x, y) || (disallow_room && is_room_block(M, x, y)))
		return false;

	return true;

}

void ghost_toggle_FLEE(Ghost* ghost, bool setFLEE) {
	if (setFLEE) {
		ghost->status = FLEE;
		ghost->speed = 1;
		ghost->flee_near_end = false;
	} else if (ghost->status == FLEE) {
		ghost->status = FREEDOM;
		ghost->speed = 2;
		ghost->flee_near_end = false;
	}
}

void ghost_collided(Ghost* ghost) {
	if (ghost->status == FLEE) {
		ghost->status = GO_IN;
		ghost->speed = 4; // Go back to room faster
	}
}