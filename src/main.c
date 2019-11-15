#include <common.h>

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    pp_log_me(PP_DANGER, "Could not initialize SDL: %s", SDL_GetError());
  pp_log_me(PP_SUCCESS, "SDL Initialized");

  /* Exit */
  SDL_Quit();
  pp_log_me(PP_SUCCESS, "SDL Shutdown");
  return 0;
}
