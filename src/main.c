#include <common.h>
#include <ppg.h>

static void game_reset(ppg *game) {
  ppg_player_init(game, 0, SCREEN_HEIGHT/2 - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT, 6);
  // Player 2
  // ppg_player_init(&game, SCREEN_WIDTH - PLAYER_WIDTH, HEIGHT/2, PLAYER_WIDTH, PLAYER_HEIGHT, 3);
  ppg_ball_init(game, SCREEN_WIDTH/2 - BALL_WIDTH, SCREEN_HEIGHT/2 - BALL_HEIGHT, BALL_WIDTH, BALL_HEIGHT, 12, 12);
}

int main(void) {
  int err = 0;
  ppg game;
  game.player.points = 0;
  ppg_reset_values(&game);

  /* Each init called here adds an extra boost in performance */
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    ppg_log_me(PPG_DANGER, "Could not initialize SDL: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    ppg_log_me(PPG_DANGER, "IMG_Init failed: %s", IMG_GetError());
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  if (TTF_Init() != 0) {
    ppg_log_me(PPG_DANGER, "TTF_Init failed: %s", TTF_GetError());
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  err = ppg_otba(&game, 3, PPG_TEXTURE);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  err = ppg_otba(&game, 2, PPG_AUDIO);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Initialized");

  game.win = SDL_CreateWindow("Ping Pong Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
  if (!game.win) {
    ppg_log_me(PPG_DANGER, "SDL_CreateWindow Error: %s", SDL_GetError());
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }
  ppg_log_me(PPG_SUCCESS, "SDL Created Window");

  game.ren = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!game.ren){
    ppg_log_me(PPG_DANGER, "SDL_CreateRenderer Error: %s", SDL_GetError());
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Created Renderer");

  uint32_t cur_tex = 0;
  err = ppg_load_texture(&game, "i:p:t", cur_tex, "imgs/paddle.png", PPG_IMG_TEX);
  if (err) { ppg_freeup_game(&game); return EXIT_FAILURE; }
  cur_tex++;

  err = ppg_load_texture(&game,"i:p:t", cur_tex, "imgs/ball.png", PPG_IMG_TEX);
  if (err) { ppg_freeup_game(&game); return EXIT_FAILURE; }
  cur_tex++;

  SDL_Color temp = {26, 255, 26, 0};
  memmove(&game.texture[cur_tex].color, &temp, sizeof(SDL_Color));
  err = ppg_load_texture(&game, "i:p:t:f", cur_tex, "fonts/bankruptcy/Bankruptcy.otf", PPG_FONT_TEX, FONT_SIZE);
  if (err) { ppg_freeup_game(&game); return EXIT_FAILURE; }

  time_t t;
  srand((unsigned) time(&t));
  uint8_t ball_dir = rand() % 4;
  game_reset(&game);

  ppg_load_audio(&game, 0, "music/evolution.mp3", PPG_MUSIC);
  ppg_load_audio(&game, 1, "music/mario_jump.wav", PPG_EFFECT);
  ppg_play_music(&game, 0, -1);

  /* read user input and handle it */
  int key = 0;
  SDL_Event e;
  while (!ppg_poll_ev(&e, &key)) {
    ppg_screen_refresh(&game);
    ppg_ball_move(&game, ball_dir);
    switch (key) {
      case 4:
        if (key != KEY_RELEASED) ppg_player_move_down(&game);
        break;
      case 5:
        if (key != KEY_RELEASED) ppg_player_move_up(&game);
        break;
      default: break;
    }
    switch (ppg_is_out(&game)) {
      case 1:
        game.player.points++;
        ball_dir = rand() % 4;
        game_reset(&game);
        break;
      case 2:
        game.player.points++;
        ball_dir = rand() % 4;
        game_reset(&game);
        break;
      default: break;
    }
    // ppg_reg_fps();
  }

  ppg_freeup_game(&game);
  ppg_log_me(PPG_SUCCESS, "SDL Shutdown");
  return EXIT_SUCCESS;
}
