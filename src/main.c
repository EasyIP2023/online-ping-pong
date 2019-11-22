#include <common.h>
#include <ppg.h>

static void game_reset(ppg *game) {
  ppg_player_init(game, 0, SCREEN_HEIGHT/2 - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT, 6);
  // Player 2
  // ppg_player_init(&game, SCREEN_WIDTH - PLAYER_WIDTH, HEIGHT/2, PLAYER_WIDTH, PLAYER_HEIGHT, 3);
  ppg_ball_init(game, SCREEN_WIDTH/2 - BALL_WIDTH, SCREEN_HEIGHT/2 - BALL_HEIGHT, BALL_WIDTH, BALL_HEIGHT, 12, 12);
}

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

int main(void) {
  int err = 0;
  ppg game;
  game.player.points = 0;
  ppg_reset_values(&game);

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

  err = ppg_otba(&game, 4, PPG_TEXTURE);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  err = ppg_otba(&game, 3, PPG_AUDIO);
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

  if (load_display_items(&game)) {
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  time_t t;
  srand((unsigned) time(&t));
  uint8_t ball_dir = rand() % 4;
  game_reset(&game);

  err = ppg_load_audio(&game, 0, "music/evolution.mp3", PPG_MUSIC);
  if (!err) {
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  err = ppg_load_audio(&game, 1, "music/dreams.mp3", PPG_MUSIC);
  if (!err) {
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  err = ppg_load_audio(&game, 2, "music/mario_jump.wav", PPG_EFFECT);
  if (!err) {
    ppg_freeup_game(&game);
    return EXIT_FAILURE;
  }

  /* read user input and handle it */
  int key = 0;
  SDL_Event e;
  bool game_menu = false, game_over = true, music_playing = false;
  while (!ppg_poll_ev(&e, &key)) {
    if (key == EXIT_GAME) goto exit_game;

    /* Game Menu Crazy If Statement Checks */
    if (!game_menu && game_over) {
      if (!music_playing) {
        if (!ppg_play_music(&game, 1, -1)) {
          ppg_freeup_game(&game);
          return EXIT_FAILURE;
        }
        music_playing = true;
      }
      game_menu = ppg_show_menu(&game, &e, &key);
      if (key == PLAY_GAME) {
        game_over = false;
        music_playing = false;
      }
    }

    /* Actual game */
    if (!game_over) {
      if (!music_playing) {
        if (!ppg_play_music(&game, 0, -1)) {
          ppg_freeup_game(&game);
          return EXIT_FAILURE;
        }
        music_playing = true;
      }
      if (ppg_screen_refresh(&game)) goto exit_game;
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
      if (game.player.points == 10) {
        game.player.points = 0;
        game_over = true;
        music_playing = false;
      }
    }
    ppg_reg_fps();
  }

exit_game:
  ppg_freeup_game(&game);
  ppg_log_me(PPG_SUCCESS, "SDL Shutdown");
  return EXIT_SUCCESS;
}
