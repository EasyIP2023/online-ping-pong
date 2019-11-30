#include <common.h>
#include <ppg.h>
#include <net.h>

#include <netpong.h>

static bool load_display_items(ppg *game) {
  bool err = false;
  uint32_t cur_di = 0;
  err = ppg_load_display_item(game, "i:p:t", cur_di, "imgs/paddle.png", PPG_IMG_TEX);
  if (err) return err;
  cur_di++;

  err = ppg_load_display_item(game,"i:p:t", cur_di, "imgs/ball.png", PPG_IMG_TEX);
  if (err) return err;
  cur_di++;

  /* Load score font */
  SDL_Color color = {26, 255, 26, 0};
  ppg_copy_sdl_color(game, cur_di, color);
  err = ppg_load_display_item(game, "i:p:t:f", cur_di, "fonts/bankruptcy/Bankruptcy.otf", PPG_FONT_TEX, FONT_SIZE);
  if (err) return err;
  cur_di++;

  /* Load menu font */
  err = ppg_load_display_item(game, "i:p:t:f", cur_di, "fonts/bankruptcy/Bankruptcy.otf", PPG_FONT_TEX, FONT_SIZE);
  if (err) return err;

  return err;
}

bool start_client(const char *ip_addr, uint16_t port) {
  bool err = false;
  ppg game;
  ppg_reset_values(&game);

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    ppg_log_me(PPG_DANGER, "Could not initialize SDL: %s", SDL_GetError());
    return false;
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    ppg_log_me(PPG_DANGER, "IMG_Init failed: %s", IMG_GetError());
    ppg_freeup_game(&game);
    return false;
  }

  if (TTF_Init() != 0) {
    ppg_log_me(PPG_DANGER, "TTF_Init failed: %s", TTF_GetError());
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_otba(&game, 4, PPG_TEXTURE);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_otba(&game, 3, PPG_AUDIO);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ppg_freeup_game(&game);
    return false;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Initialized");

  game.win = SDL_CreateWindow("Ping Pong Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
  if (!game.win) {
    ppg_log_me(PPG_DANGER, "SDL_CreateWindow Error: %s", SDL_GetError());
    ppg_freeup_game(&game);
    return false;
  }
  ppg_log_me(PPG_SUCCESS, "SDL Created Window");

  game.ren = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!game.ren){
    ppg_log_me(PPG_DANGER, "SDL_CreateRenderer Error: %s", SDL_GetError());
    ppg_freeup_game(&game);
    return false;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Created Renderer");

  if (load_display_items(&game)) {
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_load_audio(&game, 0, "music/evolution.mp3", PPG_MUSIC);
  if (!err) {
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_load_audio(&game, 1, "music/dreams.mp3", PPG_MUSIC);
  if (!err) {
    ppg_freeup_game(&game);
    return false;
  }

  err = ppg_load_audio(&game, 2, "music/mario_jump.wav", PPG_EFFECT);
  if (!err) {
    ppg_freeup_game(&game);
    return false;
  }

  ppg_player_init(&game, 0, 0, SCREEN_HEIGHT/2 - PLAYER_HEIGHT);

  /* read user input and handle it */
  uint32_t key = 0, sock_fd = 0;
  SDL_Event e;
  bool game_menu = false, game_over = true, music_playing = false;
  while (!ppg_poll_ev(&e, &key)) {
    if (key == EXIT_GAME) goto exit_game;
    if (key == RET_TO_MENU) { game_over = true; music_playing = false; }

    /* Game Menu Crazy If Statement Checks */
    if (!game_menu && game_over) {
      if (!music_playing) {
        if (!ppg_play_music(&game, 1, -1)) {
          ppg_freeup_game(&game);
          return false;
        }
        music_playing = true;
      }
      game_menu = ppg_show_menu(&game, &e, &key);
      if (key == PLAY_GAME) {
        game_over = false;
        music_playing = false;
        sock_fd = ppg_connect_client(ip_addr, port);
        if (sock_fd == UINT32_MAX) return false;
      }
    }

    /* Actual game */
    if (!game_over) {
      if (!music_playing) {
        if (!ppg_play_music(&game, 0, -1)) {
          ppg_freeup_game(&game);
          return false;
        }
        music_playing = true;
      }
      if (ppg_screen_refresh(&game)) goto exit_game;
      switch (key) {
        case 4:
          if (key != KEY_RELEASED) ppg_player_move_down(&game, 0);
          break;
        case 5:
          if (key != KEY_RELEASED) ppg_player_move_up(&game, 0);
          break;
        default: break;
      }
      if (game.player[0].points == MAX_POINTS || game.player[1].points == MAX_POINTS) {
        game.player[0].points = game.player[1].points = 0;
        game_over = true;
        music_playing = false;
        close(sock_fd);
      }
    }
    ppg_reg_fps();
  }

exit_game:
  close(sock_fd);
  ppg_freeup_game(&game);
  ppg_log_me(PPG_SUCCESS, "SDL Shutdown");
  return true;
}
