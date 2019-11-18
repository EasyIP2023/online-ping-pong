#include <check.h>

#include <common.h>
#include <ppg.h>

#define WIDTH 640
#define HEIGHT 480
ppg game;

START_TEST(test_bmp) {
  int err = 0;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    ppg_log_me(PPG_DANGER, "Could not initialize SDL: %s", SDL_GetError());
    ck_abort_msg(NULL);
  }

  err = ppg_otba(&game, 2, PPG_TEXTURE);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    ck_abort_msg(NULL);
  }

  ppg_log_me(PPG_SUCCESS, "SDL Initialized");

  game.win = SDL_CreateWindow("Ping Pong Game", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (game.win == NULL){
    ppg_log_me(PPG_DANGER, "SDL_CreateWindow Error: %s", SDL_GetError());
    freeup_ppg(&game);
    ck_abort_msg(NULL);
  }
  ppg_log_me(PPG_SUCCESS, "SDL Created Window");

  game.ren = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (game.ren == NULL){
    ppg_log_me(PPG_DANGER, "SDL_CreateRenderer Error: %s", SDL_GetError());
    freeup_ppg(&game);
    ck_abort_msg(NULL);
  }

  ppg_log_me(PPG_SUCCESS, "SDL Created Renderer");

  uint32_t cur_tex = 0;
  game.texture[cur_tex].name = "background";
  err = ppg_load_texture(&game, cur_tex, "tests/background.bmp", PPG_BMP_TEX);
  if (err) { freeup_ppg(&game); ck_abort_msg(NULL); }
  cur_tex++;

  game.texture[cur_tex].name = "image";
  err = ppg_load_texture(&game, cur_tex, "tests/image.bmp", PPG_BMP_TEX);
  if (err) { freeup_ppg(&game); ck_abort_msg(NULL); }

  int bW = 0, bH = 0;
  int iW = 0, iH = 0;
  /* A sleepy rendering loop, wait for 3 seconds and render and present the screen each time */
  for (int i = 0; i < 3; ++i){
    /* First clear the renderer */
    SDL_RenderClear(game.ren);
    SDL_QueryTexture(game.texture[0].tex, NULL, NULL, &bW, &bH);
    render_texture(&game, 0, 0, 0);
    render_texture(&game, 0, bW, 0);
    render_texture(&game, 0, 0, bH);
    render_texture(&game, 0, bW, bH);

    SDL_QueryTexture(game.texture[cur_tex].tex, NULL, NULL, &iW, &iH);
    render_texture(&game, cur_tex, (WIDTH / 2 - iW / 2), (HEIGHT / 2 - iH / 2));
    /* Update the screen */
    SDL_RenderPresent(game.ren);
    /* Take a quick break after all that hard work */
    SDL_Delay(1000);
  }

  ppg_log_me(PPG_SUCCESS, "SDL Shutdown");
} END_TEST;

Suite *main_suite(void) {
  Suite *s = NULL;
  TCase *tc_core = NULL;

  s = suite_create("TestBmp");

  /* Core test case */
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_bmp);
  suite_add_tcase(s, tc_core);

  return s;
}

int main (void) {
  int number_failed;
  SRunner *sr = NULL;

  sr = srunner_create(main_suite());
  srunner_run_all(sr, CK_NORMAL);
  freeup_ppg(&game);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  sr = NULL;
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
