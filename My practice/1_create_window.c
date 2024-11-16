#include <stdio.h>
#include <allegro5/allegro5.h>

#define LOG_ENABLED                 //? 啟用「將log顯示於console」

ALLEGRO_DISPLAY* game_display;      //? 宣告 window display pointer
const int SCREEN_W = 800;           //? 宣告 window display width
const int SCREEN_H = 600;           //? 宣告 window display height

void game_draw();
void game_destroy();
void game_abort(const char* format, ...);
void game_log(const char* format, ...);
void game_vlog(const char* format, va_list arg);

int main() {
    if (!al_init())
        game_abort("failed to initialize allegro");     //? 執行 allegro 前須先初始化
    al_rest(5);                                         //? 暫停 5 秒
    
    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");

    game_log("Allegro5 initialized");
    game_log("Game begin");
    game_draw();
    al_rest(5);
    
    game_log("Game end");
    game_destroy();
    return 0;
}

void game_draw(void) {
    al_clear_to_color(al_map_rgb(100, 100, 100));
    al_flip_display();
}

void game_destroy(void) {
    al_destroy_display(game_display);
}

// -------
// -------
// -------

void game_abort(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
    fprintf(stderr, "error occurred, exiting after 2 secs");
    al_rest(2);
    exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
    static bool clear_file = true;
    vprintf(format, arg);
    printf("\n");
    FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
    if (pFile) {
        vfprintf(pFile, format, arg);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
    clear_file = false;
#endif
}