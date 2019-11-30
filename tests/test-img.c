#include <check.h>

#include <common.h>
#include <ppg.h>

#define TILE_SIZE 40

ppg game;

START_TEST(test_img) {
  bool err = false;
  ppg_reset_values(&game);

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    ppg_log_me(PPG_DANGER, "Could not initialize SDL: %s", SDL_GetError());
    ck_abort_msg(NULL);
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
    ppg_log_me(PPG_DANGER, "IMG_Init failed: %s", IMG_GetError());
    ppg_freeup_game(&game);
    ck_abort_msg(NULL);
  }

  err = ppg_otba(&game, 2, PPG_TEXTURE);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ppg_freeup_game(&game);
    ck_abort_msg(NULL);
  }

  ppg_log_me(PPG_SUCCESS, "SDL Initialized");

  game.win = SDL_CreateWindow("Ping Pong Game", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN);
  if (!game.win) {
    ppg_log_me(PPG_DANGER, "SDL_CreateWindow Error: %s", SDL_GetError());
    ppg_freeup_game(&game);
    ck_abort_msg(NULL);
  }
  ppg_log_me(PPG_SUCCESS, "SDL Created Window");

  game.ren = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!game.ren){
    ppg_log_me(PPG_DANGER, "SDL_CreateRenderer Error: %s", SDL_GetError());
    ppg_freeup_game(&game);
    ck_abort_msg(NULL);
  }

  ppg_log_me(PPG_SUCCESS, "SDL Created Renderer");

  uint32_t cur_di = 0;
  err = ppg_load_display_item(&game, "i:p:t", cur_di, "tests/background.png", PPG_IMG_TEX);
  if (err) { ppg_freeup_game(&game); ck_abort_msg(NULL); }
  cur_di++;

  err = ppg_load_display_item(&game, "i:p:t", cur_di, "tests/image.png", PPG_IMG_TEX);
  if (err) { ppg_freeup_game(&game); ck_abort_msg(NULL); }

  /* Determine the amount of tiles one needs on screen */
  int x_tiles = SCREEN_WIDTH / TILE_SIZE;
  int y_tiles = SCREEN_WIDTH / TILE_SIZE;
  int x = 0, y = 0;

  /* Draw the tiles by calculating their positions */
  for (int i = 0; i < x_tiles * y_tiles; i++) {
    x = i % x_tiles;
    y = i / y_tiles;
    ppg_render_texture_xywh(&game, 0, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
  }

  /**
  * Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
  * the texture's width and height
  */
  int iW = 0, iH = 0;
  SDL_QueryTexture(game.display_items[cur_di].tex, NULL, NULL, &iW, &iH);
  x = SCREEN_WIDTH / 2 - iW / 2;
  y = SCREEN_HEIGHT / 2 - iH / 2;
  ppg_render_texture(&game, cur_di, x, y, NULL);
  SDL_RenderPresent(game.ren);
  SDL_Delay(2000);

  ppg_log_me(PPG_SUCCESS, "SDL Shutdown");
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestImg");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_img);
  suite_add_tcase(s, tc_core);

  return s;
}

int main (void) {
  int number_failed;
  SRunner *sr = NULL;

  sr = srunner_create(main_suite());
  sleep(3);
  srunner_run_all(sr, CK_NORMAL);
  ppg_freeup_game(&game);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  sr = NULL;
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
