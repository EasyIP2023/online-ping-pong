#include <check.h>
#include <unistd.h>

#include <common.h>
#include <ppg.h>

#define WIDTH 640
#define HEIGHT 480

ppg game;

START_TEST(test_user_input) {
  int err = 0;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    ppg_log_me(PPG_DANGER, "Could not initialize SDL: %s", SDL_GetError());
    ck_abort_msg(NULL);
  }

  err = ppg_otba(&game, 1, PPG_TEXTURE);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ck_abort_msg(NULL);
  }

  ppg_log_me(PPG_SUCCESS, "SDL Initialized");

  game.win = SDL_CreateWindow("Ping Pong Game", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (!game.win) {
    ppg_log_me(PPG_DANGER, "SDL_CreateWindow Error: %s", SDL_GetError());
    freeup_ppg(&game);
    ck_abort_msg(NULL);
  }
  ppg_log_me(PPG_SUCCESS, "SDL Created Window");

  game.ren = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!game.ren) {
    ppg_log_me(PPG_DANGER, "SDL_CreateRenderer Error: %s", SDL_GetError());
    freeup_ppg(&game);
    ck_abort_msg(NULL);
  }

  ppg_log_me(PPG_SUCCESS, "SDL Created Renderer");

  uint32_t cur_tex = 0;
  game.texture[cur_tex].name = "event_driven";
  err = ppg_load_texture(&game, cur_tex, "tests/event_driven.png", PPG_IMG_TEX);
  if (err) { freeup_ppg(&game); ck_abort_msg(NULL); }

  /* read user input and handle it */
  int iW = 0, iH = 0, x = 0, y = 0;
  bool quit = false;
  while (!quit) {
    quit = ppg_poll_ev();
    /* First clear the renderer */
    SDL_RenderClear(game.ren);
    SDL_QueryTexture(game.texture[cur_tex].tex, NULL, NULL, &iW, &iH);
    x = WIDTH / 2 - iW / 2;
    y = HEIGHT / 2 - iH / 2;
    ppg_render_texture(&game, cur_tex, x, y);
    /* Update the screen */
    SDL_RenderPresent(game.ren);
  }

  IMG_Quit();
  ppg_log_me(PPG_SUCCESS, "SDL Shutdown");
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestUserInput");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_user_input);
  suite_add_tcase(s, tc_core);

  return s;
}

int main (void) {
  int number_failed;
  SRunner *sr = NULL;

  sr = srunner_create(main_suite());
  sleep(6);
  srunner_run_all(sr, CK_NORMAL);
  freeup_ppg(&game);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  sr = NULL;
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
