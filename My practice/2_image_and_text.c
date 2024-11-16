#include <stdio.h>
#include <allegro5/allegro.h>

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define LOG_ENABLED

ALLEGRO_DISPLAY* game_display;
ALLEGRO_FONT* font;
ALLEGRO_BITMAP* img;

const int SCREEN_W = 800;
const int SCREEN_H = 600;

const int IMG_W = 640;
const int IMG_H = 479;

void game_init(void);
void game_draw(void);
void game_destroy(void);
void game_abort(const char* format, ...);
void game_log(const char* format, ...);
void game_vlog(const char* format, va_list arg);

int main(void) {
    if (!al_init())
        game_abort("failed to initialize allegro");
    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");

    if (!al_init_primitives_addon())
        game_abort("failed to initialize primitives add-on");
    if (!al_init_font_addon())
        game_abort("failed to initialize font add-on");
    if (!al_init_ttf_addon())
        game_abort("failed to initialize ttf add-on");
    if (!al_init_image_addon())
        game_abort("failed to initialize image add-on");

    game_log("Allegro5 initialized");
    game_log("Game begin");
    game_init();
    game_log("Game initialized");
    game_draw();
    al_rest(20);

    game_log("Game end");
    game_destroy();
    return 0;
}

void game_init(void) {
    font = al_load_ttf_font("pirulen.ttf", 12, 0);
    if (!font)
        game_abort("failed to load font: pirulen.ttf");

    img = al_load_bitmap("32largebugs.jpg");
    if (!img)
        game_abort("failed to load image: 32largebugs.jpg");
}

void game_draw(void) {
    al_clear_to_color(al_map_rgb(100, 100, 100));
    
    al_draw_rectangle((SCREEN_W - 437) / 2, 24,
                      (SCREEN_W + 437) / 2, 50,
                      al_map_rgb(255, 255, 255), 0);

    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30,
                 ALLEGRO_ALIGN_CENTER, "How to deal with bugs in your final project");

    al_draw_bitmap(img, (SCREEN_W - IMG_W) / 2, (SCREEN_H - IMG_H) / 2, 0);

    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_W / 2, 550,
                 ALLEGRO_ALIGN_CENTER, "Source: http://cartoontester.blogspot.com/2010/01/big-bugs.html");

    al_flip_display();
}

void game_destroy(void) {
    al_destroy_bitmap(img);
    al_destroy_font(font);
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