// utility functions such as image processing, collision detection should be put here.

#ifndef SCENE_UTILITY_H
#define SCENE_UTILITY_H
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

/* Utility functions */

ALLEGRO_SAMPLE* load_audio(const char* filename);						// 仔入影音(.wav, .flac, .ogg, .it, .mod, .s3m, .xm)並確認是否成功
ALLEGRO_SAMPLE_ID play_audio(ALLEGRO_SAMPLE* sample, float volume);		// 播放一次
ALLEGRO_SAMPLE_ID play_bgm(ALLEGRO_SAMPLE* sample, float volume);		// 重複播放
void stop_bgm(ALLEGRO_SAMPLE_ID sample);								// 停止播放
ALLEGRO_FONT* load_font(const char* filename, int size);				// 載入字體(.ttf)並確認是否成功
ALLEGRO_BITMAP* load_bitmap(const char* filename);						// 載入bitmap(.jpg, .png, .bmp, .pcx, .tga)並確認是否成功
ALLEGRO_BITMAP* load_bitmap_resized(const char* filename, int w, int h);// 同上但自訂大小
typedef struct RecArea{
	float x, y, w, h;
} RecArea;
typedef struct Pair_IntInt {
	int x;
	int y;
} Pair_IntInt;
typedef enum Directions{
	NONE = 0,
	UP = 1,
	LEFT = 2,
	RIGHT = 3,
	DOWN = 4,
	UP_DOWN = 5,
	LEFT_RIGHT = 6,
	UP_LEFT = 7,
	DOWN_LEFT = 8,
	DOWN_RIGHT = 9,
	UP_RIGHT = 10
} Directions;
typedef struct object {
	Pair_IntInt Coord;
	Pair_IntInt Size; 				// x: width, y: height
	Directions facing;
	Directions preMove;
	Directions nextTryMove;
	uint32_t moveCD;				// movement CountDown
} object;

typedef struct bitmapdata{
	int bitmap_x;
	int bitmap_y;
	int bitmap_w;
	int bitmap_h;
} bitmapdata;

//extern const int GridSize;

void setRecArea(RecArea *RA, float x, float y, float w, float h);
RecArea getDrawArea(object *obj, uint32_t TICK);
bool RecAreaOverlap(const RecArea *const RA, const RecArea *const RB);
void printRecAreaInfo(const RecArea *RA);
void printDirection(const Directions a);
bool moveTime(int speed);
uint32_t generateRandomNumber(uint32_t a, uint32_t b);
double generateRandomFloat(void);
bool bernoulliTrail(double p); // Given a Possibility and decided if occur.
bool pnt_in_rect(int px, int py, RecArea field);			// (px, py) 是否在 rect(x,y,w,h) 中
Directions oppositeDirec(Directions direc);					// *
bool is_collision(object, object);

#endif
