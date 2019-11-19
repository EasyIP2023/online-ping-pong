#include <common.h>
#include <ppg.h>

int main(void) {
  ppg game;
  int err = 0;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    ppg_log_me(PPG_DANGER, "Could not initialize SDL: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    ppg_log_me(PPG_DANGER, "IMG_Init failed: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  err = ppg_otba(&game, 2, PPG_TEXTURE);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    return EXIT_FAILURE;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Initialized");

  game.win = SDL_CreateWindow("Ping Pong Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
  if (!game.win) {
    ppg_log_me(PPG_DANGER, "SDL_CreateWindow Error: %s", SDL_GetError());
    freeup_ppg(&game);
    return EXIT_FAILURE;
  }
  ppg_log_me(PPG_SUCCESS, "SDL Created Window");

  game.ren = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!game.ren){
    ppg_log_me(PPG_DANGER, "SDL_CreateRenderer Error: %s", SDL_GetError());
    freeup_ppg(&game);
    return EXIT_FAILURE;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Created Renderer");
  ppg_player_init(&game, 0, SCREEN_HEIGHT/2 - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT, 3);
  // Player 2
  // ppg_player_init(&game, SCREEN_WIDTH - PLAYER_WIDTH, HEIGHT/2, PLAYER_WIDTH, PLAYER_HEIGHT, 3);
  ppg_ball_init(&game, SCREEN_WIDTH/2 - BALL_WIDTH, SCREEN_HEIGHT/2 - BALL_HEIGHT, BALL_WIDTH, BALL_HEIGHT, 5, 5);

  uint32_t cur_tex = 0;
  game.texture[cur_tex].name = "player";
  err = ppg_load_texture(&game, cur_tex, "imgs/paddle.png", PPG_IMG_TEX);
  if (err) { freeup_ppg(&game); return EXIT_FAILURE; }
  cur_tex++;

  game.texture[cur_tex].name = "ball";
  err = ppg_load_texture(&game, cur_tex, "imgs/ball.png", PPG_IMG_TEX);
  if (err) { freeup_ppg(&game); return EXIT_FAILURE; }

  ppg_log_me(PPG_INFO, "Player start position (%d, %d)", game.player.box.x, game.player.box.y);
  ppg_screen_refresh(&game, 1, 0);

  /* read user input and handle it */
  int key = 0;
  SDL_Event e;
  bool quit = false;
  while (!quit) {
    quit = ppg_poll_ev(&e, &key);
    switch (key) {
      case 4:
        if (key != KEY_RELEASED) {
          ppg_player_move_down(&game);
          ppg_log_me(PPG_INFO, "Player moved down to position (%d, %d)", game.player.box.x, game.player.box.y);
          ppg_screen_refresh(&game, 1, 0);
        }
        break;
      case 5:
        if (key != KEY_RELEASED) {
          ppg_player_move_up(&game);
          ppg_log_me(PPG_INFO, "Player moved up to position (%d, %d)", game.player.box.x, game.player.box.y);
          ppg_screen_refresh(&game, 1, 0);
        }
        break;
      default: break;
    }
  }
  ppg_log_me(PPG_SUCCESS, "SDL Shutdown");
  return EXIT_SUCCESS;
}
