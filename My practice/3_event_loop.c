#include <stdio.h>
#include <allegro5/allegro.h>

#define LOG_ENABLED

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;

const int SCREEN_W = 800;
const int SCREEN_H = 600;

void game_start_event_loop(void);
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
        
    game_event_queue = al_create_event_queue();
    if (!game_event_queue)
        game_abort("failed to create event queue");
    
    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));

    game_log("Allegro5 initialized");
    game_log("Game begin");

    game_log("Game start event loop");
    game_start_event_loop();

    game_log("Game end");
    game_destroy();
    return 0;
}

void game_start_event_loop(void) {
    bool done = false;
    ALLEGRO_EVENT event;
    while (!done) {
        al_wait_for_event(game_event_queue, &event);
        
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            game_log("Window close button clicked");
            done = true;
        }
    }
}

void game_destroy(void) {
    al_destroy_event_queue(game_event_queue);
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