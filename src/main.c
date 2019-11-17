#include <common.h>
#include <ppg.h>

#define WIDTH 640
#define HEIGHT 480

int main(void) {
  ppg game;
  int err = 0;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    ppg_log_me(PPG_DANGER, "Could not initialize SDL: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  err = ppg_otba(&game, 2, PPG_TEXTURE);
  if (err) {
    ppg_log_me(PPG_DANGER, "Failed to allocate space");
    return EXIT_FAILURE;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Initialized");

  game.win = SDL_CreateWindow("Ping Pong Game", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (game.win == NULL){
  	ppg_log_me(PPG_DANGER, "SDL_CreateWindow Error: %s", SDL_GetError());
  	return EXIT_FAILURE;
  }
  ppg_log_me(PPG_SUCCESS, "SDL Created Window");

  game.ren = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (game.ren == NULL){
  	ppg_log_me(PPG_DANGER, "SDL_CreateRenderer Error: %s", SDL_GetError());
    freeup_ppg(&game);
  	return EXIT_FAILURE;
  }

  ppg_log_me(PPG_SUCCESS, "SDL Created Renderer");

  uint32_t cur_tex = 0;
  err = ppg_load_texture(&game, cur_tex, "background", "bmps/background.bmp");
  if (err) { freeup_ppg(&game); return err; }

  cur_tex++;
  err = ppg_load_texture(&game, cur_tex, "image", "bmps/image.bmp");
  if (err) { freeup_ppg(&game); return err; }

  //A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
  for (int i = 0; i < 3; ++i){
  	//First clear the renderer
  	SDL_RenderClear(game.ren);
  	//Draw the texture
  	SDL_RenderCopy(game.ren, game.texture[0].tex, NULL, NULL);
  	//Update the screen
  	SDL_RenderPresent(game.ren);
  	//Take a quick break after all that hard work
  	SDL_Delay(1000);
  }

  freeup_ppg(&game);
  ppg_log_me(PPG_SUCCESS, "SDL Shutdown");
  return EXIT_SUCCESS;
}
