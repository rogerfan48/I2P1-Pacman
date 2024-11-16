# main.c
- game_create()

# game.c
- game_create
  - allegro5_init()
  - game_init()
  - game_draw()
  - game_start_event_loop()
  - (*active_scene.destroy)();
  - game_destroy()

- static allegro5_init
  - 初始化各個 add-on
  - 設定
    - game_display
    - game_update_timer
    - game_tick_timer
    - game_event_queue
    - mouse_state
  - 註冊 event source
  - 開始 game_update_timer

- static game_init
  - shared_init()
  - game_change_scene -> menu

- static game_start_event_loop
  - 發生各種event時，要做的相應反應

- static game_draw
  - (*active_scene.draw)();
  - flip

- static game_update
  - (*active_scene.update)();

- static game_destroy
  - shared_destroy()
  - 釋放所有資源

- game_change_scene
- game_abort
- game_log
- game_vlog

# shared.c
- 各種資源(字體、音檔...)
- shared_init
  - 載入資源
- shared_destroy
  - 釋放資源