#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#define LOG_ENABLED

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

const int FPS = 30;
const int SCREEN_W = 800;
const int SCREEN_H = 600;
bool key_state[ALLEGRO_KEY_MAX];

float x, y;

void game_init(void);
void allegro5_init(void);
void game_start_event_loop(void);
void game_update(void);
void game_draw(void);
void game_destroy(void);

void game_abort(const char* format, ...);
void game_log(const char* format, ...);
void game_vlog(const char* format, va_list arg);

int main(void) {
    allegro5_init();
    game_log("Allegro5 initialized");
    game_log("Game begin");

    game_init();
    game_log("Game initialized");

    game_draw();
    game_log("Game start event loop");

    game_start_event_loop();
    game_log("Game end");
    game_destroy();
    return 0;
}

void allegro5_init(void) {
    if (!al_init())
        game_abort("failed to initialize allegro");

    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");

    game_update_timer = al_create_timer(1.0f / FPS);
    if (!game_update_timer)
        game_abort("failed to create timer");

    game_event_queue = al_create_event_queue();
    if (!game_event_queue)
        game_abort("failed to create event queue");

    if (!al_init_primitives_addon())
        game_abort("failed to initialize primitives add-on");

    if (!al_install_keyboard())
        game_abort("failed to install keyboard");

    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    al_register_event_source(game_event_queue, al_get_keyboard_event_source());
    al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));

    al_start_timer(game_update_timer);
}

void game_init(void) {
    x = y = 400;
}

void game_start_event_loop(void) {
    bool done = false;
    ALLEGRO_EVENT event;
    while (!done) {
        al_wait_for_event(game_event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            done = true;

        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            game_log("Key with keycode %d down", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = true;

        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            game_log("Key with keycode %d up", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = false;

        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            if (event.timer.source == game_update_timer) {
                game_update();
                game_draw();
            }
        }
    }
}

void game_update(void) {
    int vx, vy;
    vx = vy = 0;
    if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
        vy -= 1;
    if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
        vy += 1;
    if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
        vx -= 1;
    if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
        vx += 1;
    
    // 0.71 is (1/sqrt(2)).
    y += vy * (vx ? 0.71f : 1);
    x += vx * (vy ? 0.71f : 1);
}

void game_draw(void) {
    al_clear_to_color(al_map_rgb(100, 100, 100));
    al_draw_filled_rectangle(x, y, x + 25, y + 25, al_map_rgb(255, 255, 255));
    al_flip_display();
}

void game_destroy(void) {
    al_destroy_timer(game_update_timer);
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