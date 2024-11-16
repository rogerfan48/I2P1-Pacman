
#include "ghost.h"
#include "pacman_obj.h"
#include "map.h"
#include "utility.h"
#include "scene_game.h"
/* Shared variables */
#define GO_OUT_TIME (5*GAME_TICK_CD)
#define GHOST_DIRECT (8*GAME_TICK_CD)
#define GHOST_RANDOM (8*GAME_TICK_CD)
extern uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
extern const int room_grid_x, room_grid_y;

/* Declare static function prototypes */
// static function reference: https://stackoverflow.com/questions/558122/what-is-a-static-function-in-c
static void ghost_move_script_FREEDOM_random(Ghost* ghost, Map* M);
static void ghost_move_script_FREEDOM_shortest_path(Ghost* ghost, Map* M, Pacman* pman);
static void ghost_move_script_BLOCKED(Ghost* ghost, Map* M);
static void ghost_move_script_GO_IN(Ghost* ghost, Map* M);
static void ghost_move_script_GO_OUT(Ghost* ghost, Map* M);
static void ghost_move_script_FLEE(Ghost* ghost, Map* M, const Pacman* const pacman);

static void ghost_move_script_FREEDOM_random(Ghost* ghost, Map* M) {
	static Directions maybeMove[4];
	int moveableNum = 0;
	for (Directions i = 1; i <= 4; i++)
		if (ghost_movable(ghost, M, i, true) && i!=oppositeDirec(ghost->objData.preMove)) maybeMove[moveableNum++] = i;
	if (moveableNum)
		ghost_NextMove(ghost, maybeMove[generateRandomNumber(0, moveableNum-1)]);
	else				// * 當進入死路，必須回頭
		ghost_NextMove(ghost, oppositeDirec(ghost->objData.preMove));
}

static void ghost_move_script_FREEDOM_shortest_path(Ghost* ghost, Map* M, Pacman* pman) {
	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, pman->objData.Coord.x, pman->objData.Coord.y);
}

static void ghost_move_script_BLOCKED(Ghost* ghost, Map* M) {
	switch (ghost->objData.preMove) {
		case UP:
			if (ghost->objData.Coord.y == 10)
				ghost_NextMove(ghost, DOWN);
			else
				ghost_NextMove(ghost, UP);
			break;
		case DOWN:
			if (ghost->objData.Coord.y == 12)
				ghost_NextMove(ghost, UP);
			else
				ghost_NextMove(ghost, DOWN);
			break;
		default:
			ghost_NextMove(ghost, UP);
			break;
	}
}

static void ghost_move_script_GO_IN(Ghost* ghost, Map* M) {
	// Description
	// `shortest_path_direc` is a function that returns the direction of shortest path.
	// Check `map.c` for its detail usage.
	// For GO_IN state.
	ghost->objData.nextTryMove = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, room_grid_x, room_grid_y);
}

static void ghost_move_script_GO_OUT(Ghost* ghost, Map* M) {
	// Description
	// Here we always assume the room of ghosts opens upward.
	// And used a greedy method to drag ghosts out of room.
	// You should modify here if you have different implementation/design of room.
	if(M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B') 
		ghost_NextMove(ghost, UP);
	else
		ghost->status = FREEDOM;
}

static void ghost_move_script_FLEE(Ghost* ghost, Map* M, const Pacman * const pacman) {
	Directions shortestDirection = shortest_path_direc(M, ghost->objData.Coord.x, ghost->objData.Coord.y, pacman->objData.Coord.x, pacman->objData.Coord.y);
	if (ghost_movable(ghost, M, oppositeDirec(shortestDirection), true)) ghost_NextMove(ghost, oppositeDirec(shortestDirection));
	else {
		Directions maybeMove[4];
		int moveableNum = 0;
		for (Directions i = 1; i <= 4; i++)
			if (ghost_movable(ghost, M, i, true) && i!=oppositeDirec(ghost->objData.preMove)) maybeMove[moveableNum++] = i;
		if (moveableNum)
			ghost_NextMove(ghost, maybeMove[generateRandomNumber(0, moveableNum-1)]);
		else				// * 當進入死路，必須回頭
			ghost_NextMove(ghost, oppositeDirec(ghost->objData.preMove));
	} 
}

void ghost_move_script_random(Ghost* ghost, Map* M, Pacman* pacman) {
	if (!moveTime(ghost->speed))
		return;
		switch (ghost->status) {
			case BLOCKED:
				ghost_move_script_BLOCKED(ghost, M);
				if (al_get_timer_count(game_tick_timer) > GO_OUT_TIME)
					ghost->status = GO_OUT;
				break;
			case FREEDOM:
				ghost_move_script_FREEDOM_random(ghost, M);
				break;
			case GO_OUT:
				ghost_move_script_GO_OUT(ghost, M);
				break;
			case GO_IN:
				ghost_move_script_GO_IN(ghost, M);
				if (M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B') {
					ghost->status = GO_OUT;
					ghost->speed = 2; // reset the speed after back to the room.
				}
				break;
			case FLEE:
				ghost_move_script_FLEE(ghost, M, pacman);
				break;
			default:
				break;
		}

		if(ghost_movable(ghost, M, ghost->objData.nextTryMove, false)){
			ghost->objData.preMove = ghost->objData.nextTryMove;
			ghost->objData.nextTryMove = NONE;
		}
		else if (!ghost_movable(ghost, M, ghost->objData.preMove, false))
			return;

		switch (ghost->objData.preMove) {
		case RIGHT:
			ghost->objData.Coord.x += 1;
			break;
		case LEFT:
			ghost->objData.Coord.x -= 1;
			break;
		case UP:
			ghost->objData.Coord.y -= 1;
			break;
		case DOWN:
			ghost->objData.Coord.y += 1;
			break;
		default:
			break;
		}
		ghost->objData.facing = ghost->objData.preMove;
		ghost->objData.moveCD = GAME_TICK_CD;
}

void ghost_move_script_shortest_path(Ghost* ghost, Map* M, Pacman* pacman) {
	if (!moveTime(ghost->speed)) return;
			switch (ghost->status) {
			case BLOCKED:
				ghost_move_script_BLOCKED(ghost, M);
				if (al_get_timer_count(game_tick_timer) - ghost->go_in_time > GO_OUT_TIME)
					ghost->status = GO_OUT;
				break;
			case FREEDOM:
				if (ghost->is_direct) {
					ghost_move_script_FREEDOM_shortest_path(ghost, M, pacman);
					if (al_get_timer_count(game_tick_timer) - ghost->last_time_change > GHOST_DIRECT) {
						ghost->is_direct = false;
						ghost->last_time_change = al_get_timer_count(game_tick_timer);
					}
				} else {
					ghost_move_script_FREEDOM_random(ghost, M);
					if (ghost->last_time_change==0 || (al_get_timer_count(game_tick_timer) - ghost->last_time_change > GHOST_RANDOM)) {
						ghost->is_direct = true;
						ghost->last_time_change = al_get_timer_count(game_tick_timer);
					}
				}
				break;
			case GO_OUT:
				ghost_move_script_GO_OUT(ghost, M);
				break;
			case GO_IN:
				ghost_move_script_GO_IN(ghost, M);
				if (M->map[ghost->objData.Coord.y][ghost->objData.Coord.x] == 'B') {
					ghost->status = BLOCKED;
					ghost->speed = 2; // reset the speed after back to the room.
					ghost->go_in_time = al_get_timer_count(game_tick_timer); 
				}
				break;
			case FLEE:
				ghost_move_script_FLEE(ghost, M, pacman);
				break;
			default:
				break;
		}

		if(ghost_movable(ghost, M, ghost->objData.nextTryMove, false)){
			ghost->objData.preMove = ghost->objData.nextTryMove;
			ghost->objData.nextTryMove = NONE;
		}
		else if (!ghost_movable(ghost, M, ghost->objData.preMove, false))
			return;

		switch (ghost->objData.preMove) {
		case RIGHT:
			ghost->objData.Coord.x += 1;
			break;
		case LEFT:
			ghost->objData.Coord.x -= 1;
			break;
		case UP:
			ghost->objData.Coord.y -= 1;
			break;
		case DOWN:
			ghost->objData.Coord.y += 1;
			break;
		default:
			break;
		}
		ghost->objData.facing = ghost->objData.preMove;
		ghost->objData.moveCD = GAME_TICK_CD;
}