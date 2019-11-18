#include <common.h>
#include <ppg.h>

bool ppg_poll_ev(int *clip) {
  SDL_Event e;
  while (SDL_PollEvent(&e)){
    switch (e.type) {
      case SDL_QUIT: return true; /* If user closes the window */
      case SDL_KEYDOWN: /* Use number input to select which clip should be drawn */
        switch (e.key.keysym.sym) {
          case SDLK_DOWN: *clip = 0; break;
          case SDLK_UP: *clip = 1; break;
          case SDLK_LEFT: *clip = 2; break;
          case SDLK_RIGHT: *clip = 3; break;
          case SDLK_ESCAPE: return true;
          default: return false;
        }
        return false;
      case SDL_MOUSEBUTTONDOWN: return true; /* If user clicks the mouse */
      default: return false;
    }
  }
  return false;
}
