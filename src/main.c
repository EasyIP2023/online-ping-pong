#include <common.h>
#include <ppg.h>

const int WIDTH  = 640;
const int HEIGHT = 480;

int main(void) {
  ppg game;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    pp_log_me(PP_DANGER, "Could not initialize SDL: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  pp_log_me(PP_SUCCESS, "SDL Initialized");

  game.win = SDL_CreateWindow("Ping Pong Game", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if (game.win == NULL){
  	pp_log_me(PP_DANGER, "SDL_CreateWindow Error: %s", SDL_GetError());
  	return EXIT_FAILURE;
  }
  pp_log_me(PP_SUCCESS, "SDL Created Window");

  game.ren = SDL_CreateRenderer(game.win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (game.ren == NULL){
  	pp_log_me(PP_DANGER, "SDL_CreateRenderer Error: %s", SDL_GetError());
    freeup_ppg(&game);
  	return EXIT_FAILURE;
  }

  pp_log_me(PP_SUCCESS, "SDL Created Renderer");

  int ret = pp_load_texture(&game, "VENUS.BMP");
  if (ret) { freeup_ppg(&game); return ret; }

  //A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
  for (int i = 0; i < 3; ++i){
  	//First clear the renderer
  	SDL_RenderClear(game.ren);
  	//Draw the texture
  	SDL_RenderCopy(game.ren, game.tex, NULL, NULL);
  	//Update the screen
  	SDL_RenderPresent(game.ren);
  	//Take a quick break after all that hard work
  	SDL_Delay(1000);
  }

  freeup_ppg(&game);
  pp_log_me(PP_SUCCESS, "SDL Shutdown");
  return EXIT_SUCCESS;
}
