#include "scene_scoreboard.h"
#include <allegro5/allegro_primitives.h>
#include <string.h>

#define SCOREBOARD_MAX 8

bool justFinished;
int justScore;
int scoreboard_num;
Score* scoreboard;
static char temp[13];
static int idx = 0;

static FILE* pFile;

static void loading_new_score() {
    pFile = fopen("Src/scoreboard.txt", "w+");
    if (scoreboard_num == 0) {
        fprintf(pFile, "1\n%s %d\n", temp, justScore);
        
    } else {
        int pos=8;
        for (int i=0; i<scoreboard_num; i++) {
            if (scoreboard[i].point < justScore) {
                pos = i;
                break;
            }
        }
        fprintf(pFile, "%d\n", (scoreboard_num+1 >= 8) ? 8 : scoreboard_num+1);
        for (int i=0; i<scoreboard_num; i++) {
            if (i==pos) {
                fprintf(pFile, "%s %d\n", temp, justScore);
            }
            fprintf(pFile, "%s %d\n", scoreboard[i].name, scoreboard[i].point);
        }
    }
    fclose(pFile);
    game_change_scene(scene_menu_create());
    game_change_scene(scene_scoreboard_create());
}

static void init(void) {
    scoreboard = malloc(SCOREBOARD_MAX * sizeof(Score));

    pFile = fopen("Src/scoreboard.txt", "r");
    if (pFile == NULL) {
        scoreboard_num = 0;
    } else {
        fscanf(pFile, "%d", &scoreboard_num);
        for (int i=0; i<scoreboard_num && i<8; i++) {
            fscanf(pFile, " %s%d", scoreboard[i].name, &scoreboard[i].point);
        }
    }
    fclose(pFile);

    if (justFinished) {
        for (int i=0; i<13; i++) {
            temp[i] = '\0';
        }
        idx=0;
    }
}

static void draw(void) {
    char str[30];
	al_clear_to_color(al_map_rgb(10, 30, 40));
    al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"<ENTER> Back to menu"
	);
    if (!justFinished) {
        for (int i=0; i<scoreboard_num; i++) {
            snprintf(str, 30, "%d. %s", i+1, scoreboard[i].name);
            al_draw_text(
                menuFont,
                al_map_rgb(255, 255, 255),
                160,
                100 + i * 60,
                ALLEGRO_ALIGN_LEFT,
                str
            );
            snprintf(str, 30, "--- %3d", scoreboard[i].point);
            al_draw_text(
                menuFont,
                al_map_rgb(255, 255, 255),
                640,
                100 + i * 60,
                ALLEGRO_ALIGN_RIGHT,
                str
            );
        }
    } else {
        al_draw_text(
            menuFont,
            al_map_rgb(255, 255, 255),
            400,
            280,
            ALLEGRO_ALIGN_CENTER,
            "Type Your Name"
        );
        al_draw_filled_rectangle(220, 330, 580, 400, al_map_rgb(20, 60, 60));
        al_draw_text(
            menuFont,
            al_map_rgb(255, 255, 255),
            400,
            355-3,
            ALLEGRO_ALIGN_CENTER,
            temp
        );
        snprintf(str, 30, "Score: %3d", justScore);
        al_draw_text(
            menuFont,
            al_map_rgb(255, 255, 255),
            400,
            425-3,
            ALLEGRO_ALIGN_CENTER,
            str
        );
    }
}

static void on_key_down(int keycode) {
    if (justFinished) {
        if (((keycode >= ALLEGRO_KEY_A && keycode <= ALLEGRO_KEY_9) || keycode==ALLEGRO_KEY_SPACE) && strlen(temp)<12) {
            temp[idx] = (al_keycode_to_name(keycode))[0];
            idx++;
        } else if (keycode == ALLEGRO_KEY_BACKSPACE) {
            idx--;
            temp[idx] = '\0';
        } else if (keycode == ALLEGRO_KEY_ENTER) {
            justFinished = false;
            loading_new_score();
        }
    } else {
        if (keycode == ALLEGRO_KEY_ENTER) {
            game_change_scene(scene_menu_create());
        }
    }
}

static void destroy(void) {
    free(scoreboard);
}

Scene scene_scoreboard_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Scoreboard";
    scene.initialize = &init;
	scene.draw = &draw;
	scene.on_key_down = &on_key_down;
    scene.destroy = &destroy;
	// scene.on_mouse_move = &on_mouse_move;
	// scene.on_mouse_down = &on_mouse_down;
	// TODO-IF: Register more event callback functions such as keyboard, mouse, ...
	game_log("Settings scene created");
	return scene;
}