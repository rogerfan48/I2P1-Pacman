// declare shared variables and allegro5 related routines.

// extern 表「宣告但不定義」，若var被定義則被指定位置且可能被設初始值；宣告，則只是告知編譯器有這個變數/函數
// 函數宣告預設就是 extern
// static 當在 top level，表該變數/函數僅會在該檔案中被看見

#ifndef GAME_H
#define GAME_H
#define _CRT_SECURE_NO_DEPRECATE
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>	
typedef struct Ghost Ghost;
typedef struct Map Map;
typedef struct Pacman Pacman;

#define LOG_ENABLED					//  開啟 log 到 console

// Types of function pointers.
static ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;
typedef void(*func_ptr)(void);
typedef void(*script_func_ptr)(Ghost* ghost, Map*M, Pacman* pacman);
typedef void(*func_ptr_keyboard)(int keycode);
typedef void(*func_ptr_mouse)(int btn, int x, int y, int dz);
// TODO-IF: More function pointer typedefs for other events.

typedef struct Scene {				// 含有所有 scene func/even callbacks 的 struct
	char* name;
	func_ptr initialize;
	func_ptr update;
	func_ptr draw;
	func_ptr destroy;
	func_ptr_keyboard on_key_down;
	func_ptr_keyboard on_key_up;
	func_ptr_mouse on_mouse_down;
	func_ptr_mouse on_mouse_move;
	func_ptr_mouse on_mouse_up;
	func_ptr_mouse on_mouse_scroll;
	// TODO-IF: More event callbacks such as timer tick, video finished, ...
} Scene;

extern const int FPS;
extern const int SCREEN_W;
extern const int SCREEN_H;
extern const int RESERVE_SAMPLES;			// 至多 4 個音檔能被同時播放

/* Input states */
extern Scene active_scene;					// The active scene. Events will be triggered through function pointers.
extern bool key_state[ALLEGRO_KEY_MAX];		// 鍵盤按鍵被按下與否
extern bool* mouse_state;					// 滑鼠按鍵被按下與否 1:left 2:right 3:middle
extern int mouse_x, mouse_y;				// mouse pos
// TODO-IF: More variables to store input states such as joysticks, ...
extern int keyboard_w;
extern int keyboard_a;
extern int keyboard_s;
extern int keyboard_d;

/* Function prototypes */
void game_create(void);						// 創建並開始遊戲
void game_change_scene(Scene next_scene);	// 轉換 scene
void game_abort(const char* format, ...);
void game_log(const char* format, ...);

/* Functions and variables that should be implemented in other files. */

// Initialize shared variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
extern void shared_init(void);
// Free shared variables and resources.
extern void shared_destroy(void);
#endif
