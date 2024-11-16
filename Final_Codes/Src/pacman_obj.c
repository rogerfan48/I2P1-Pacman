#include <allegro5/allegro_primitives.h>
#include "pacman_obj.h"
#include "map.h"
#include "scene_game.h"

/* Static variables */
static const int start_grid_x = 25, start_grid_y = 25;							// 初始位置
static const int fix_draw_pixel_offset_x = -3, fix_draw_pixel_offset_y = -3;  	// draw offset 
static const int draw_region = 30;												// pacman bitmap draw region
static ALLEGRO_SAMPLE_ID PACMAN_MOVESOUND_ID;
static const int basic_speed = 2;			// 更改前先了解 step(), speed, GAME_TICK, GAME_TICK_CD, objData->moveCD 關係
static int death_display_count = 0;
static int death_accumulate = 0;

/* Shared variables */
extern ALLEGRO_SAMPLE* PACMAN_MOVESOUND;
extern ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND;
extern ALLEGRO_SAMPLE* PACMAN_POWERED;
extern uint32_t GAME_TICK;
extern uint32_t GAME_TICK_CD;
extern bool game_over;
extern float effect_volume;

/* Declare static function */
static bool pacman_movable(const Pacman* pacman, const Map* M, Directions targetDirec) {
	if (wallWalk) return true;
	int x = pacman->objData.Coord.x;
	int y = pacman->objData.Coord.y;
	
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
	if (is_wall_block(M, x, y) || is_room_block(M, x, y))
		return false;
	
	return true;
}

Pacman* pacman_create() {
	Pacman* pman = (Pacman*)malloc(sizeof(Pacman));
	if (!pman)
		return NULL;
	pman->objData.Coord.x = 24;
	pman->objData.Coord.y = 24;
	pman->objData.Size.x = block_width;
	pman->objData.Size.y = block_height;

	pman->objData.preMove = NONE;
	pman->objData.nextTryMove = NONE;
	pman->speed = basic_speed;

	pman->death_anim_counter = al_create_timer(1.0f / 8.0f);
	pman->powerUp = false;
	/* load sprites */
	pman->move_sprite = load_bitmap("Assets/pacman_move.png");
	pman->die_sprite = load_bitmap("Assets/pacman_die.png");

	death_display_count = 0;
	death_accumulate = 0;
	return pman;
}

void pacman_destroy(Pacman* pman) {	
	al_destroy_bitmap(pman->move_sprite);
	al_destroy_bitmap(pman->die_sprite);
	al_destroy_timer(pman->death_anim_counter);
	free(pman);
}

void pacman_draw(Pacman* pman) {
	RecArea drawArea = getDrawArea((object *)pman, GAME_TICK_CD);
	if (!game_over) {
		switch (pman->objData.facing) {
			case RIGHT:
				al_draw_scaled_bitmap(pman->move_sprite, (pman->mouth_open) ? 0 : 16, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			case LEFT:
				al_draw_scaled_bitmap(pman->move_sprite, (pman->mouth_open) ? 32 : 48, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			case UP:
				al_draw_scaled_bitmap(pman->move_sprite, (pman->mouth_open) ? 64 : 80, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			case DOWN:
				al_draw_scaled_bitmap(pman->move_sprite, (pman->mouth_open) ? 96 : 112, 0,
					16, 16,
					drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
					draw_region, draw_region, 0
				); break;
			default: break;
		}
		if (pman->objData.moveCD == GAME_TICK_CD/2)
			pman->mouth_open = !pman->mouth_open;
	} else {
		al_draw_scaled_bitmap(pman->die_sprite, death_accumulate*16, 0,
			16, 16,
			drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
			draw_region, draw_region, 0
		);
		if (death_accumulate < 12) {
			if (death_display_count < 10) {
				death_display_count++;
			} else {
				death_display_count = 0;
				death_accumulate++;
			}
		}
	}
}
void pacman_move(Pacman* pacman, Map* M) {
	if (!moveTime(pacman->speed))
		return;
	if (game_over)
		return;

	// int probe_x = pacman->objData.Coord.x, probe_y = pacman->objData.Coord.y;
	if (pacman_movable(pacman, M, pacman->objData.nextTryMove)) 
		pacman->objData.preMove = pacman->objData.nextTryMove;
	else if (!pacman_movable(pacman, M, pacman->objData.preMove)) 
		return;

	switch (pacman->objData.preMove) {
	case UP:
		pacman->objData.Coord.y -= 1;
		pacman->objData.preMove = UP;
		break;
	case DOWN:
		pacman->objData.Coord.y += 1;
		pacman->objData.preMove = DOWN;
		break;
	case LEFT:
		pacman->objData.Coord.x -= 1;
		pacman->objData.preMove = LEFT;
		break;
	case RIGHT:
		pacman->objData.Coord.x += 1;
		pacman->objData.preMove = RIGHT;
		break;
	default:
		break;
	}
	pacman->objData.facing = pacman->objData.preMove;
	pacman->objData.moveCD = GAME_TICK_CD;
}

void pacman_eatItem(Pacman* pacman, const char Item) {
	switch (Item) {
		case '.':
			stop_bgm(PACMAN_MOVESOUND_ID);
			PACMAN_MOVESOUND_ID = play_audio(PACMAN_MOVESOUND, effect_volume);
			break;
		case 'P':
			stop_bgm(PACMAN_MOVESOUND_ID);
			PACMAN_MOVESOUND_ID = play_audio(PACMAN_POWERED, effect_volume);
			break;
		default:
			break;
	}
}

void pacman_NextMove(Pacman* pacman, Directions next) {
	pacman->objData.nextTryMove = next;
}

void pacman_die() {
	PACMAN_MOVESOUND_ID = play_audio(PACMAN_DEATH_SOUND, effect_volume);
}